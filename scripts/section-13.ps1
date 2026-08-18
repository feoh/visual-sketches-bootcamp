[CmdletBinding()]
param(
    [Parameter(Position = 0, Mandatory = $true)][ValidateSet("doctor", "generate", "build")][string]$Command,
    [string]$OfRoot,
    [ValidateSet("starter", "solution")][string]$Project = "starter",
    [ValidateSet("Debug", "Release")][string]$Configuration = "Release"
)
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Exercise = "13-time-as-a-drawable-axis"
$ProjectPath = Join-Path $Root "exercises\$Exercise\$Project"
function Fail([string]$Message) { throw "section-13: $Message" }
function Assert-Inventory {
    $expected = @(
        "exercises/$Exercise/$Project/src/main.cpp",
        "exercises/$Exercise/$Project/src/ofApp.cpp",
        "exercises/$Exercise/$Project/src/ofApp.h",
        "exercises/$Exercise/$Project/src/design/temporal_design.cpp",
        "exercises/$Exercise/$Project/src/design/temporal_design.h",
        "exercises/$Exercise/shared/temporal_history.cpp",
        "exercises/$Exercise/shared/temporal_history.h"
    ) | Sort-Object
    $actual = @()
    foreach ($sourceRoot in @((Join-Path $ProjectPath "src"), (Join-Path $Root "exercises\$Exercise\shared"))) {
        $actual += Get-ChildItem -LiteralPath $sourceRoot -Recurse -File | Where-Object { $_.Extension -in @(".cpp", ".h") } |
            ForEach-Object { $_.FullName.Substring($Root.Length + 1).Replace('\', '/') }
    }
    if (@(Compare-Object $expected @($actual | Sort-Object) -CaseSensitive).Count -ne 0) { Fail "source inventory is incomplete or stale" }
}
function Resolve-Environment {
    $candidate = if ($OfRoot) { $OfRoot } else { $env:OF_ROOT }
    if (-not $candidate -or -not (Test-Path -LiteralPath $candidate -PathType Container)) { Fail "set OF_ROOT or pass -OfRoot PATH" }
    $script:ResolvedOfRoot = (Resolve-Path -LiteralPath $candidate).Path
    $script:PG = Join-Path $script:ResolvedOfRoot "projectGenerator\resources\app\app\projectGenerator.exe"
    if (-not (Test-Path -LiteralPath $script:PG -PathType Leaf)) { Fail "packaged Project Generator is missing: $script:PG" }
}
function Invoke-Doctor {
    Resolve-Environment
    foreach ($directory in @("addons", "libs\openFrameworks", "libs\openFrameworksCompiled", "scripts", "scripts\templates", "scripts\templates\vs")) {
        if (-not (Test-Path -LiteralPath (Join-Path $script:ResolvedOfRoot $directory) -PathType Container)) { Fail "openFrameworks directory is missing: $directory" }
    }
    foreach ($file in @("libs\openFrameworks\utils\ofConstants.h", "libs\openFrameworks\ofMain.h", "libs\openFrameworks\app\ofAppRunner.h", "scripts\templates\vs\emptyExample.vcxproj")) {
        if (-not (Test-Path -LiteralPath (Join-Path $script:ResolvedOfRoot $file) -PathType Leaf)) { Fail "openFrameworks file is missing: $file" }
    }
    $constants = Get-Content -LiteralPath (Join-Path $script:ResolvedOfRoot "libs\openFrameworks\utils\ofConstants.h")
    $parts = @{}
    foreach ($name in @("MAJOR", "MINOR", "PATCH")) {
        $match = @($constants | Select-String -Pattern "^#define\s+OF_VERSION_$name\s+(\d+)\s*$")
        if ($match.Count -ne 1) { Fail "could not parse OF_VERSION_$name" }
        $parts[$name] = $match[0].Matches[0].Groups[1].Value
    }
    $version = "$($parts.MAJOR).$($parts.MINOR).$($parts.PATCH)"
    if ($version -ne "0.12.1") { Fail "expected openFrameworks 0.12.1, observed $version" }
    $pgVersion = (& $script:PG --version 2>&1 | Out-String)
    if ($LASTEXITCODE -ne 0 -or $pgVersion -notmatch "(?<!\d)0\.103\.0(?!\d)") { Fail "expected Project Generator 0.103.0, observed $pgVersion" }
    if (-not (Test-Path (Join-Path $ProjectPath "src") -PathType Container) -or
        -not (Test-Path (Join-Path $ProjectPath "addons.make") -PathType Leaf) -or
        (Get-Item (Join-Path $ProjectPath "addons.make")).Length -ne 0) { Fail "canonical project inputs are missing or addons.make is not empty" }
    Assert-Inventory
    Write-Host "section-13: host=vs project=$Project OF=$version PG=0.103.0"
}
function Remove-Generated {
    $expected = [IO.Path]::GetFullPath((Join-Path $Root "exercises\$Exercise\$Project"))
    $canonical = (Resolve-Path -LiteralPath $ProjectPath).Path
    if ($canonical -cne $expected -or ((Get-Item $canonical).Attributes -band [IO.FileAttributes]::ReparsePoint)) { Fail "unsafe project path" }
    $bin = Join-Path $canonical "bin"
    if (Test-Path -LiteralPath $bin) {
        if ((Get-Item $bin).Attributes -band [IO.FileAttributes]::ReparsePoint) { Fail "unsafe bin path" }
        Get-ChildItem -LiteralPath $bin -Force | Where-Object Name -ne "data" | Remove-Item -Recurse -Force
    }
    foreach ($relative in @("$Project.sln", "$Project.vcxproj", "$Project.vcxproj.filters", "$Project.vcxproj.user", "icon.rc", "obj")) {
        $target = Join-Path $canonical $relative
        if (Test-Path -LiteralPath $target) { Remove-Item -LiteralPath $target -Recurse -Force }
    }
}
function Invoke-Generate {
    Invoke-Doctor
    if (-not (Get-Command git -ErrorAction SilentlyContinue)) { Fail "git is required" }
    $before = (& git -C $Root diff --binary --no-ext-diff -- | Out-String)
    Remove-Generated
    $old = $env:PG_OF_PATH
    try {
        $env:PG_OF_PATH = $script:ResolvedOfRoot
        Push-Location $Root
        try { $output = & $script:PG "-o$script:ResolvedOfRoot" "-pvs" "-s../shared" "exercises/$Exercise/$Project" 2>&1; $status = $LASTEXITCODE } finally { Pop-Location }
    } finally { $env:PG_OF_PATH = $old }
    $text = $output | Out-String
    if ($status -ne 0 -or $text -match "(?i)\[\s*error\s*\]") { Fail "Project Generator failed: $text" }
    foreach ($file in @("$Project.sln", "$Project.vcxproj", "$Project.vcxproj.filters")) {
        if (-not (Test-Path (Join-Path $ProjectPath $file) -PathType Leaf)) { Fail "generator omitted $file" }
    }
    Assert-Inventory
    $after = (& git -C $Root diff --binary --no-ext-diff -- | Out-String)
    if ($before -cne $after) { Fail "generation changed tracked files" }
    Write-Host "section-13: generated $Project for vs"
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
    if (-not (Test-Path $solution -PathType Leaf)) { Fail "generate before build" }
    & (Find-MSBuild) $solution /m "/p:Configuration=$Configuration" /p:Platform=x64 /p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0.26100.0
    if ($LASTEXITCODE -ne 0) { Fail "MSBuild failed" }
    $suffix = if ($Configuration -eq "Debug") { "_debug" } else { "" }
    $product = Join-Path $ProjectPath "bin\$Project$suffix.exe"
    if (-not (Test-Path $product -PathType Leaf)) { Fail "expected product is missing: $product" }
    Write-Host "section-13: built $product"
}
switch ($Command) { doctor { Invoke-Doctor } generate { Invoke-Generate } build { Invoke-Build } }
