[CmdletBinding()]
param(
    [Parameter(Position = 0, Mandatory = $true)]
    [ValidateSet("doctor", "generate", "build")]
    [string]$Command,
    [string]$OfRoot,
    [ValidateSet("starter", "solution")]
    [string]$Project = "starter",
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$ProjectPath = Join-Path $Root "exercises\00-visual-signature\$Project"
function Fail([string]$Message) { throw "section-00: $Message" }
function Info([string]$Message) { Write-Host "section-00: $Message" }
function Assert-SourceInventory {
    $expected = @(
        "exercises/00-visual-signature/$Project/src/main.cpp",
        "exercises/00-visual-signature/$Project/src/ofApp.cpp",
        "exercises/00-visual-signature/$Project/src/ofApp.h",
        "exercises/00-visual-signature/$Project/src/design/signature_design.cpp",
        "exercises/00-visual-signature/$Project/src/design/signature_design.h",
        "exercises/00-visual-signature/shared/signature_geometry.cpp",
        "exercises/00-visual-signature/shared/signature_geometry.h"
    ) | Sort-Object
    $actual = @()
    foreach ($sourceRoot in @((Join-Path $ProjectPath "src"), (Join-Path $Root "exercises\00-visual-signature\shared"))) {
        $actual += Get-ChildItem -LiteralPath $sourceRoot -Recurse -File | Where-Object {
            $_.Extension -in @(".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp")
        } | ForEach-Object { $_.FullName.Substring($Root.Length + 1).Replace('\', '/') }
    }
    $difference = @(Compare-Object -ReferenceObject $expected -DifferenceObject @($actual | Sort-Object) -CaseSensitive)
    if ($difference.Count -ne 0) {
        $difference | Format-Table | Out-String | Write-Error
        Fail "course source inventory has missing or stale files"
    }
}
function Resolve-Environment {
    $candidate = if ($OfRoot) { $OfRoot } else { $env:OF_ROOT }
    if (-not $candidate -or -not (Test-Path -LiteralPath $candidate -PathType Container)) { Fail "set OF_ROOT or pass -OfRoot PATH" }
    $script:ResolvedOfRoot = (Resolve-Path -LiteralPath $candidate).Path
    $script:PG = Join-Path $script:ResolvedOfRoot "projectGenerator\resources\app\app\projectGenerator.exe"
    if (-not (Test-Path -LiteralPath $script:PG -PathType Leaf)) { Fail "packaged Project Generator is missing: $script:PG" }
}
function Get-VersionMacro([string]$Name) {
    $file = Join-Path $script:ResolvedOfRoot "libs\openFrameworks\utils\ofConstants.h"
    $match = Select-String -LiteralPath $file -Pattern "^#define\s+OF_VERSION_$Name\s+(\d+)\s*$"
    if ($match.Count -ne 1) { Fail "could not parse OF_VERSION_$Name" }
    return $match.Matches[0].Groups[1].Value
}
function Invoke-Doctor {
    Resolve-Environment
    $version = "$(Get-VersionMacro MAJOR).$(Get-VersionMacro MINOR).$(Get-VersionMacro PATCH)"
    if ($version -ne "0.12.1") { Fail "expected openFrameworks 0.12.1, observed $version" }
    $pgVersion = (& $script:PG --version 2>&1 | Out-String)
    if ($LASTEXITCODE -ne 0 -or $pgVersion -notmatch "(?<!\d)0\.103\.0(?!\d)") { Fail "expected Project Generator 0.103.0, observed $pgVersion" }
    if (-not (Test-Path (Join-Path $ProjectPath "src") -PathType Container) -or -not (Test-Path (Join-Path $ProjectPath "addons.make") -PathType Leaf)) { Fail "canonical project inputs are missing" }
    if ((Get-Item (Join-Path $ProjectPath "addons.make")).Length -ne 0) { Fail "section 00 projects must not enable addons" }
    if (-not (Test-Path (Join-Path $Root "exercises\00-visual-signature\shared\signature_geometry.cpp") -PathType Leaf)) { Fail "shared geometry source is missing" }
    Assert-SourceInventory
    Info "host=vs project=$Project OF=$version PG=0.103.0"
}
function Test-ReparsePoint([IO.FileSystemInfo]$Item) {
    return (($Item.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0)
}
function Assert-DeletionTarget([string]$CanonicalProject, [string]$Target) {
    $canonicalTarget = [IO.Path]::GetFullPath($Target)
    $prefix = $CanonicalProject.TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
    if (-not $canonicalTarget.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) {
        Fail "refusing deletion target outside canonical project: $canonicalTarget"
    }
    if (Test-Path -LiteralPath $canonicalTarget) {
        $item = Get-Item -LiteralPath $canonicalTarget -Force
        if (Test-ReparsePoint $item) { Fail "refusing reparse-point deletion target: $canonicalTarget" }
    }
    return $canonicalTarget
}
function Remove-Generated {
    $expected = [IO.Path]::GetFullPath((Join-Path $Root "exercises\00-visual-signature\$Project"))
    $canonicalProject = (Resolve-Path -LiteralPath $ProjectPath).Path
    $projectItem = Get-Item -LiteralPath $canonicalProject -Force
    if ($canonicalProject -cne $expected -or (Test-ReparsePoint $projectItem)) { Fail "unsafe project path: $canonicalProject" }

    $bin = Join-Path $canonicalProject "bin"
    if (Test-Path -LiteralPath $bin) {
        $binItem = Get-Item -LiteralPath $bin -Force
        if (Test-ReparsePoint $binItem) { Fail "refusing reparse-point bin directory: $bin" }
        foreach ($child in Get-ChildItem -LiteralPath $bin -Force | Where-Object Name -ne "data") {
            $target = Assert-DeletionTarget $canonicalProject $child.FullName
            Remove-Item -LiteralPath $target -Recurse -Force
        }
    }
    foreach ($relative in @("$Project.sln", "$Project.vcxproj", "$Project.vcxproj.filters", "$Project.vcxproj.user", "icon.rc", "obj")) {
        $target = Assert-DeletionTarget $canonicalProject (Join-Path $canonicalProject $relative)
        if (Test-Path -LiteralPath $target) { Remove-Item -LiteralPath $target -Recurse -Force }
    }
}
function Assert-ExactPathSet([string[]]$Actual, [string[]]$Expected, [string]$Label) {
    $duplicates = @($Actual | Group-Object | Where-Object Count -ne 1)
    if ($duplicates.Count -ne 0) { Fail "$Label contains duplicate source paths: $($duplicates.Name -join ', ')" }
    $difference = @(Compare-Object -ReferenceObject @($Expected | Sort-Object) -DifferenceObject @($Actual | Sort-Object) -CaseSensitive)
    if ($difference.Count -ne 0) {
        $difference | Format-Table | Out-String | Write-Error
        Fail "$Label has missing or stale course source membership"
    }
}
function Get-CourseMembership([xml]$Xml, [string]$ElementName) {
    $separator = [IO.Path]::DirectorySeparatorChar
    $projectSource = ((Join-Path $ProjectPath "src").ToLowerInvariant().TrimEnd($separator) + $separator)
    $sharedSource = ((Join-Path $Root "exercises\00-visual-signature\shared").ToLowerInvariant().TrimEnd($separator) + $separator)
    return @($Xml.Project.ItemGroup.$ElementName | Where-Object { $null -ne $_ } | ForEach-Object {
        $include = $_.GetAttribute("Include")
        if ([string]::IsNullOrWhiteSpace($include)) { return }
        [IO.Path]::GetFullPath((Join-Path $ProjectPath $include)).ToLowerInvariant()
    } | Where-Object { $_.StartsWith($projectSource) -or $_.StartsWith($sharedSource) })
}
function Assert-VisualStudioMembership {
    $compileRelative = @("src\main.cpp", "src\ofApp.cpp", "src\design\signature_design.cpp", "..\shared\signature_geometry.cpp")
    $includeRelative = @("src\ofApp.h", "src\design\signature_design.h", "..\shared\signature_geometry.h")
    $expectedCompile = @($compileRelative | ForEach-Object { [IO.Path]::GetFullPath((Join-Path $ProjectPath $_)).ToLowerInvariant() })
    $expectedInclude = @($includeRelative | ForEach-Object { [IO.Path]::GetFullPath((Join-Path $ProjectPath $_)).ToLowerInvariant() })
    foreach ($metadata in @("$Project.vcxproj", "$Project.vcxproj.filters")) {
        [xml]$xml = Get-Content -Raw -LiteralPath (Join-Path $ProjectPath $metadata)
        Assert-ExactPathSet (Get-CourseMembership $xml "ClCompile") $expectedCompile "$metadata ClCompile"
        Assert-ExactPathSet (Get-CourseMembership $xml "ClInclude") $expectedInclude "$metadata ClInclude"
    }
}
function Get-CanonicalSnapshot {
    $files = @(
        Get-ChildItem -LiteralPath (Join-Path $ProjectPath "src"),(Join-Path $ProjectPath "bin\data"),(Join-Path $Root "exercises\00-visual-signature\shared") -File -Recurse
        Get-Item -LiteralPath (Join-Path $ProjectPath "addons.make")
    ) | Sort-Object FullName
    return (($files | ForEach-Object { "$(($_ | Get-FileHash -Algorithm SHA256).Hash) $($_.FullName)" }) -join "`n")
}
function Invoke-Generate {
    Invoke-Doctor
    if (-not (Get-Command git -ErrorAction SilentlyContinue)) { Fail "git is required" }
    $before = (& git -C $Root diff --binary --no-ext-diff HEAD -- | Out-String)
    $beforeInputs = Get-CanonicalSnapshot
    Remove-Generated
    $logDirectory = Join-Path $Root ".harness\logs"; New-Item -ItemType Directory -Force $logDirectory | Out-Null
    $old = $env:PG_OF_PATH
    try {
        $env:PG_OF_PATH = $script:ResolvedOfRoot
        Push-Location $Root
        try { $output = & $script:PG "-o$script:ResolvedOfRoot" "-pvs" "-s../shared" "exercises/00-visual-signature/$Project" 2>&1; $status = $LASTEXITCODE } finally { Pop-Location }
    } finally { $env:PG_OF_PATH = $old }
    $text = $output | Out-String
    Set-Content -LiteralPath (Join-Path $logDirectory "generate-vs-section-00-$Project.log") -Value $text
    if ($status -ne 0 -or $text -match "(?i)\[\s*error\s*\]") { Fail "Project Generator failed: $text" }
    foreach ($file in @("$Project.sln", "$Project.vcxproj", "$Project.vcxproj.filters")) {
        if (-not (Test-Path (Join-Path $ProjectPath $file) -PathType Leaf)) { Fail "generator omitted $file" }
    }
    Assert-VisualStudioMembership
    Assert-SourceInventory
    $after = (& git -C $Root diff --binary --no-ext-diff HEAD -- | Out-String)
    $afterInputs = Get-CanonicalSnapshot
    if ($before -cne $after) { Fail "generation changed tracked files" }
    if ($beforeInputs -cne $afterInputs) { Fail "generation changed canonical project inputs" }
    $global:LASTEXITCODE = 0
    Info "generated $Project for vs"
}
function Find-MSBuild {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) { Fail "vswhere.exe is required" }
    $installation = (& $vswhere -version "[17.0,18.0)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1)
    if (-not $installation) { Fail "Visual Studio 2022 Desktop development with C++ is required" }
    return Join-Path $installation "MSBuild\Current\Bin\MSBuild.exe"
}
function Invoke-Build {
    Invoke-Doctor
    $solution = Join-Path $ProjectPath "$Project.sln"
    if (-not (Test-Path $solution)) { Fail "generate before build" }
    & (Find-MSBuild) $solution /m "/p:Configuration=$Configuration" /p:Platform=x64 /p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0.26100.0
    if ($LASTEXITCODE -ne 0) { Fail "MSBuild failed" }
    $suffix = if ($Configuration -eq "Debug") { "_debug" } else { "" }
    $product = Join-Path $ProjectPath "bin\$Project$suffix.exe"
    if (-not (Test-Path $product -PathType Leaf)) { Fail "expected product is missing: $product" }
    Info "built $product"
}
switch ($Command) { doctor { Invoke-Doctor } generate { Invoke-Generate } build { Invoke-Build } }
