[CmdletBinding()]
param(
    [Parameter(Position = 0, Mandatory = $true)]
    [ValidateSet("doctor", "generate", "build", "test")]
    [string]$Command,
    [string]$OfRoot,
    [ValidateSet("all", "windowed", "unit")]
    [string]$Project = "all",
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$ExpectedTests = 12

function Fail([string]$Message) { throw "foundation: $Message" }
function Info([string]$Message) { Write-Host "foundation: $Message" }

function Resolve-FoundationEnvironment {
    $candidate = $OfRoot
    if ([string]::IsNullOrWhiteSpace($candidate)) { $candidate = $env:OF_ROOT }
    if ([string]::IsNullOrWhiteSpace($candidate)) { Fail "set OF_ROOT or pass -OfRoot PATH" }
    if (-not (Test-Path -LiteralPath $candidate -PathType Container)) { Fail "openFrameworks root does not exist: $candidate" }
    $script:ResolvedOfRoot = (Resolve-Path -LiteralPath $candidate).Path
    foreach ($directory in @("addons", "libs", "scripts")) {
        if (-not (Test-Path -LiteralPath (Join-Path $script:ResolvedOfRoot $directory) -PathType Container)) {
            Fail "OF_ROOT is missing $directory/: $script:ResolvedOfRoot"
        }
    }
    if (-not [Environment]::Is64BitOperatingSystem -or $env:PROCESSOR_ARCHITECTURE -notin @("AMD64", "x86")) {
        Fail "Windows foundation lane requires an x64 OS"
    }
    $script:HostPlatform = "vs"
    $script:PG = Join-Path $script:ResolvedOfRoot "projectGenerator\resources\app\app\projectGenerator.exe"
    if (-not (Test-Path -LiteralPath $script:PG -PathType Leaf)) { Fail "packaged Project Generator is missing: $script:PG" }
}

function Get-VersionMacro([string]$Name) {
    $file = Join-Path $script:ResolvedOfRoot "libs\openFrameworks\utils\ofConstants.h"
    $match = Select-String -LiteralPath $file -Pattern "^#define\s+OF_VERSION_$Name\s+(\d+)\s*$"
    if ($match.Count -ne 1) { Fail "could not parse OF_VERSION_$Name from $file" }
    return $match.Matches[0].Groups[1].Value
}

function Assert-AddonsFile([string]$Name) {
    $file = Join-Path $RepoRoot "foundation\$Name\addons.make"
    $seen = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
    $addons = [System.Collections.Generic.List[string]]::new()
    foreach ($raw in Get-Content -LiteralPath $file) {
        $line = $raw.Trim()
        if ([string]::IsNullOrWhiteSpace($line)) { continue }
        if ($line -cne $raw) { Fail "$file contains surrounding whitespace: $raw" }
        if ([IO.Path]::IsPathRooted($line) -or $line -match '^[A-Za-z]:' -or
            $line -match '^[\\/]{1,2}' -or $line -match '^~[\\/]') {
            Fail "$file contains an absolute or drive-qualified path: $line"
        }
        if (-not $seen.Add($line)) { Fail "$file contains duplicate addon: $line" }
        $addons.Add($line)
    }
    if ($Name -eq "unit") {
        if ($addons.Count -ne 1 -or $addons[0] -cne "ofxUnitTests") {
            Fail "$file must contain only ofxUnitTests exactly once"
        }
    } elseif ($addons.Count -ne 0) {
        Fail "$file must be empty for the addon-free windowed probe"
    }
}

function Assert-SourceInventory {
    $expected = @(
        "foundation/unit/src/main.cpp",
        "foundation/unit/src/ofApp.cpp",
        "foundation/unit/src/ofApp.h",
        "foundation/windowed/src/main.cpp",
        "foundation/windowed/src/ofApp.cpp",
        "foundation/windowed/src/ofApp.h",
        "shared/core/course_probe.cpp",
        "shared/core/course_probe.h",
        "shared/test-support/deterministic_fixture.h",
        "shared/test-support/expect_near.h"
    ) | Sort-Object
    $actual = @()
    foreach ($root in @("foundation\unit\src", "foundation\windowed\src", "shared\core", "shared\test-support")) {
        $actual += Get-ChildItem -LiteralPath (Join-Path $RepoRoot $root) -Recurse -File |
            Where-Object { $_.Extension -in @(".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp") } |
            ForEach-Object { $_.FullName.Substring($RepoRoot.Length + 1).Replace('\', '/') }
    }
    $actual = @($actual | Sort-Object)
    $difference = @(Compare-Object -ReferenceObject $expected -DifferenceObject $actual -CaseSensitive)
    if ($difference.Count -ne 0) {
        $difference | Format-Table | Out-String | Write-Error
        Fail "course source inventory has missing or stale files"
    }
}

function Invoke-Doctor {
    Resolve-FoundationEnvironment
    if (-not (Get-Command git -ErrorAction SilentlyContinue)) { Fail "git is required to verify generation does not change tracked inputs" }
    $version = "$(Get-VersionMacro MAJOR).$(Get-VersionMacro MINOR).$(Get-VersionMacro PATCH)"
    if ($version -ne "0.12.1") { Fail "expected openFrameworks 0.12.1, observed $version" }
    $pgVersion = (& $script:PG --version 2>&1 | Out-String)
    if ($LASTEXITCODE -ne 0) { Fail "Project Generator could not run: $pgVersion" }
    if ($pgVersion -notmatch "(?<!\d)0\.103\.0(?!\d)") { Fail "expected Project Generator 0.103.0, observed: $pgVersion" }
    foreach ($name in @("windowed", "unit")) {
        $path = Join-Path $RepoRoot "foundation\$name"
        if (-not (Test-Path (Join-Path $path "src") -PathType Container)) { Fail "missing foundation/$name/src" }
        if (-not (Test-Path (Join-Path $path "addons.make") -PathType Leaf)) { Fail "missing foundation/$name/addons.make" }
        Assert-AddonsFile $name
    }
    if (-not (Test-Path (Join-Path $RepoRoot "shared\core") -PathType Container) -or
        -not (Test-Path (Join-Path $RepoRoot "shared\test-support") -PathType Container)) {
        Fail "shared source directories are missing or have incorrect case"
    }
    Assert-SourceInventory
    if (-not (Test-Path (Join-Path $script:ResolvedOfRoot "addons\ofxUnitTests\src") -PathType Container)) { Fail "pinned OF_ROOT is missing the shipped ofxUnitTests addon" }
    Info "host=vs arch=x64"
    Info "OF_ROOT=$script:ResolvedOfRoot openFrameworks=0.12.1"
    Info "Project Generator=$script:PG version=0.103.0"
}

function Get-Projects {
    if ($Project -eq "all") { return @("windowed", "unit") }
    return @($Project)
}

function Remove-Generated([string]$Name) {
    $path = Join-Path $RepoRoot "foundation\$Name"
    $resolvedParent = (Resolve-Path (Split-Path $path -Parent)).Path
    $binPath = Join-Path $path "bin"
    if ($resolvedParent -ne (Join-Path $RepoRoot "foundation") -or (Get-Item $path).LinkType -or ((Test-Path $binPath) -and (Get-Item $binPath).LinkType)) { Fail "unsafe project path: $path" }
    if (Test-Path $binPath -PathType Container) {
        Get-ChildItem -LiteralPath $binPath | Where-Object { $_.Name -ne "data" } | Remove-Item -Recurse -Force
    }
    $targets = @(
        "Makefile", "config.make", ".vscode", "$Name.code-workspace", "$Name.xcodeproj",
        "Project.xcconfig", "of.entitlements", "openFrameworks-Info.plist", "$Name.sln",
        "$Name.vcxproj", "$Name.vcxproj.filters", "$Name.vcxproj.user", "icon.rc", "obj",
        "bin\$Name.exe", "bin\${Name}_debug.exe", "bin\$Name.pdb", "bin\${Name}_debug.pdb",
        "bin\$Name.ilk", "bin\${Name}_debug.ilk"
    )
    foreach ($relative in $targets) {
        $target = Join-Path $path $relative
        if (Test-Path -LiteralPath $target) { Remove-Item -LiteralPath $target -Recurse -Force }
    }
}

function Assert-ExactPathSet([string[]]$Actual, [string[]]$Expected, [string]$Label) {
    $duplicates = @($Actual | Group-Object | Where-Object { $_.Count -ne 1 })
    if ($duplicates.Count -ne 0) { Fail "$Label contains duplicate source paths: $($duplicates.Name -join ', ')" }
    $difference = @(Compare-Object -ReferenceObject @($Expected | Sort-Object) -DifferenceObject @($Actual | Sort-Object) -CaseSensitive)
    if ($difference.Count -ne 0) {
        $difference | Format-Table | Out-String | Write-Error
        Fail "$Label has missing or stale source membership"
    }
}

function Assert-Generated([string]$Name) {
    $path = Join-Path $RepoRoot "foundation\$Name"
    $projectFile = Join-Path $path "$Name.vcxproj"
    $solution = Join-Path $path "$Name.sln"
    if (-not (Test-Path $projectFile -PathType Leaf) -or (Get-Item $projectFile).Length -eq 0 -or
        -not (Test-Path $solution -PathType Leaf) -or (Get-Item $solution).Length -eq 0) {
        Fail "$Name generation omitted Visual Studio metadata"
    }
    [xml]$xml = Get-Content -LiteralPath $projectFile -Raw
    $actualCompile = @($xml.Project.ItemGroup.ClCompile | Where-Object { $null -ne $_ } | ForEach-Object {
        $include = $_.GetAttribute("Include")
        if ([string]::IsNullOrWhiteSpace($include)) { Fail "$Name ClCompile contains an empty source path" }
        [IO.Path]::GetFullPath((Join-Path $path $include)).ToLowerInvariant()
    })
    $actualInclude = @($xml.Project.ItemGroup.ClInclude | Where-Object { $null -ne $_ } | ForEach-Object {
        $include = $_.GetAttribute("Include")
        if ([string]::IsNullOrWhiteSpace($include)) { Fail "$Name ClInclude contains an empty source path" }
        [IO.Path]::GetFullPath((Join-Path $path $include)).ToLowerInvariant()
    })
    # Project Generator also adds addon headers from OF_ROOT. Exact-set
    # validation applies to course-owned files; shipped addon membership is
    # independently constrained by addons.make and doctor.
    $separator = [IO.Path]::DirectorySeparatorChar
    $coursePrefixes = @(
        (Join-Path $RepoRoot "foundation\$Name\src"),
        (Join-Path $RepoRoot "shared")
    ) | ForEach-Object { $_.ToLowerInvariant().TrimEnd($separator) + $separator }
    $actualCompile = @($actualCompile | Where-Object {
        $candidate = $_
        @($coursePrefixes | Where-Object { $candidate.StartsWith($_, [StringComparison]::OrdinalIgnoreCase) }).Count -gt 0
    })
    $actualInclude = @($actualInclude | Where-Object {
        $candidate = $_
        @($coursePrefixes | Where-Object { $candidate.StartsWith($_, [StringComparison]::OrdinalIgnoreCase) }).Count -gt 0
    })
    if ($Name -eq "windowed") {
        $compileRelative = @("src\main.cpp", "src\ofApp.cpp", "..\..\shared\core\course_probe.cpp")
        $includeRelative = @("src\ofApp.h", "..\..\shared\core\course_probe.h")
    } else {
        $compileRelative = @("src\main.cpp", "src\ofApp.cpp", "..\..\shared\core\course_probe.cpp")
        $includeRelative = @("src\ofApp.h", "..\..\shared\core\course_probe.h", "..\..\shared\test-support\deterministic_fixture.h", "..\..\shared\test-support\expect_near.h")
    }
    $expectedCompile = @($compileRelative | ForEach-Object { [IO.Path]::GetFullPath((Join-Path $path $_)).ToLowerInvariant() })
    $expectedInclude = @($includeRelative | ForEach-Object { [IO.Path]::GetFullPath((Join-Path $path $_)).ToLowerInvariant() })
    Assert-ExactPathSet $actualCompile $expectedCompile "$Name ClCompile"
    Assert-ExactPathSet $actualInclude $expectedInclude "$Name ClInclude"
}

function Get-CanonicalInputSnapshot {
    $files = @()
    foreach ($root in @("foundation\windowed\src", "foundation\windowed\bin\data", "foundation\unit\src", "foundation\unit\bin\data", "shared")) {
        $files += Get-ChildItem -LiteralPath (Join-Path $RepoRoot $root) -Recurse -File
    }
    foreach ($relative in @("foundation\windowed\addons.make", "foundation\windowed\README.md", "foundation\unit\addons.make", "foundation\unit\README.md")) {
        $files += Get-Item -LiteralPath (Join-Path $RepoRoot $relative)
    }
    return (@($files | Sort-Object FullName | ForEach-Object {
        "$($_.FullName.Substring($RepoRoot.Length + 1).Replace('\', '/')) $((Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash)"
    }) -join "`n")
}

function Invoke-Generate {
    Invoke-Doctor
    $logDirectory = Join-Path $RepoRoot ".harness\logs"
    New-Item -ItemType Directory -Force $logDirectory | Out-Null
    $before = (& git -C $RepoRoot diff --binary --no-ext-diff HEAD -- | Out-String)
    $beforeInputs = Get-CanonicalInputSnapshot
    foreach ($name in Get-Projects) {
        Remove-Generated $name
        $sources = "../../shared/core"
        # A single root avoids OF 0.12.1 GNU Make's broken multi-root object rule.
        if ($name -eq "unit") { $sources = "../../shared" }
        $log = Join-Path $logDirectory "generate-vs-$name.log"
        $oldPgPath = $env:PG_OF_PATH
        try {
            $env:PG_OF_PATH = $script:ResolvedOfRoot
            Push-Location $RepoRoot
            try {
                $output = & $script:PG "-o$script:ResolvedOfRoot" "-pvs" "-s$sources" "foundation/$name" 2>&1
                $status = $LASTEXITCODE
            } finally { Pop-Location }
        } finally { $env:PG_OF_PATH = $oldPgPath }
        $text = $output | Out-String
        Set-Content -LiteralPath $log -Value $text
        if ($status -ne 0) { Write-Error $text; Fail "Project Generator exited $status for $name" }
        if ($text -match "(?i)(\[\s*error\s*\]|`"errorMessage`"\s*:\s*`"[^`"]+)") { Fail "Project Generator reported an error for $name (log: $log)" }
        Assert-Generated $name
        Assert-SourceInventory
    }
    $after = (& git -C $RepoRoot diff --binary --no-ext-diff HEAD -- | Out-String)
    $afterInputs = Get-CanonicalInputSnapshot
    if ($before -cne $after) { Fail "generation changed tracked files" }
    if ($beforeInputs -cne $afterInputs) { Fail "generation changed canonical project inputs" }
    $embeddedRoot = (& git -C $RepoRoot grep -F -- $script:ResolvedOfRoot -- foundation shared scripts tests .github 2>$null | Out-String)
    if ($LASTEXITCODE -eq 0) { Fail "tracked implementation files embed the selected OF_ROOT: $embeddedRoot" }
    if ($LASTEXITCODE -gt 1) { Fail "could not scan tracked files for embedded OF_ROOT" }
    Info "generation passed for $Project"
}

function Get-Product([string]$Name) {
    $suffix = if ($Configuration -eq "Debug") { "_debug" } else { "" }
    return Join-Path $RepoRoot "foundation\$Name\bin\$Name$suffix.exe"
}

function Find-MSBuild {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) { Fail "vswhere.exe is required with Visual Studio 2022" }
    $installation = (& $vswhere -version "[17.0,18.0)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1)
    if (-not $installation) { Fail "Visual Studio 2022 Desktop development with C++ is required" }
    $msbuild = Join-Path $installation "MSBuild\Current\Bin\MSBuild.exe"
    if (-not (Test-Path $msbuild)) { Fail "MSBuild.exe is missing: $msbuild" }
    return $msbuild
}

function Invoke-Build {
    Invoke-Doctor
    $msbuild = Find-MSBuild
    foreach ($name in Get-Projects) {
        $solution = Join-Path $RepoRoot "foundation\$name\$name.sln"
        if (-not (Test-Path $solution)) { Fail "generate foundation/$name before build" }
        & $msbuild $solution /m "/p:Configuration=$Configuration" /p:Platform=x64 /p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0.26100.0
        if ($LASTEXITCODE -ne 0) { Fail "MSBuild failed for $name $Configuration" }
        $product = Get-Product $name
        if (-not (Test-Path $product -PathType Leaf) -or (Get-Item $product).Length -eq 0) { Fail "expected product is missing: $product" }
        Info "built $product"
    }
}

function Invoke-Test {
    if ($Project -eq "windowed") { Fail "test only supports -Project unit" }
    Invoke-Doctor
    $executable = Get-Product unit
    if (-not (Test-Path $executable -PathType Leaf)) { Fail "build the unit $Configuration product before test: $executable" }
    $logDirectory = Join-Path $RepoRoot ".harness\logs"
    New-Item -ItemType Directory -Force $logDirectory | Out-Null
    Push-Location (Join-Path $RepoRoot "foundation\unit\bin")
    try { $output = & $executable 2>&1; $status = $LASTEXITCODE } finally { Pop-Location }
    $text = $output | Out-String
    $log = Join-Path $logDirectory "test-vs-$Configuration.log"
    Set-Content -LiteralPath $log -Value $text
    Write-Host $text
    if ($status -ne 0) { Fail "unit executable returned $status" }
    $summaries = [regex]::Matches($text, "\d+/\d+ tests passed")
    if ($summaries.Count -ne 1) { Fail "expected one unit summary, observed $($summaries.Count)" }
    if ($summaries[0].Value -ne "$ExpectedTests/$ExpectedTests tests passed") { Fail "expected exactly $ExpectedTests discovered passing tests" }
    if ($text -match "(?i)(\d+/\d+ tests failed|\[\s*error\s*\])") { Fail "unit log contains a failure diagnostic" }
    Info "unit runtime passed with exactly $ExpectedTests tests"
}

switch ($Command) {
    "doctor" { Invoke-Doctor }
    "generate" { Invoke-Generate }
    "build" { Invoke-Build }
    "test" { Invoke-Test }
}
