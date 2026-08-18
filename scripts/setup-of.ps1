[CmdletBinding()]
param([string]$Destination = (Join-Path (Get-Location) ".openframeworks"))

$ErrorActionPreference = "Stop"
$Asset = "of_v0.12.1_vs_64_release.zip"
$RootName = "of_v0.12.1_vs_64_release"
$ExpectedSha256 = "059f5210dd86beaee9dd17cfe4afe9eb5929376412b9baffda68ccd5d0b4d8a8"
$Url = "https://github.com/openframeworks/openFrameworks/releases/download/0.12.1/$Asset"
New-Item -ItemType Directory -Force -Path $Destination | Out-Null
$Destination = (Resolve-Path -LiteralPath $Destination).Path
$Archive = Join-Path $Destination $Asset
if (-not (Test-Path -LiteralPath $Archive -PathType Leaf)) {
    Invoke-WebRequest -Uri $Url -OutFile $Archive
}
$Observed = (Get-FileHash -LiteralPath $Archive -Algorithm SHA256).Hash.ToLowerInvariant()
if ($Observed -ne $ExpectedSha256) {
    throw "setup-of: openFrameworks archive checksum mismatch: expected $ExpectedSha256, observed $Observed"
}

# Never trust a cached extracted tree. Recreate it from the verified archive in
# a fresh sibling and replace the selected root only after validating identity.
$Staging = Join-Path $Destination ".of-extract-$([Guid]::NewGuid().ToString('N'))"
$Backup = Join-Path $Destination ".$RootName.backup-$([Guid]::NewGuid().ToString('N'))"
$Installed = Join-Path $Destination $RootName
New-Item -ItemType Directory -Path $Staging | Out-Null
try {
    Expand-Archive -LiteralPath $Archive -DestinationPath $Staging
    $StagedRoot = Join-Path $Staging $RootName
    foreach ($required in @("addons", "libs", "scripts")) {
        if (-not (Test-Path -LiteralPath (Join-Path $StagedRoot $required) -PathType Container)) {
            throw "setup-of: verified archive omitted $RootName/$required"
        }
    }
    $Constants = Join-Path $StagedRoot "libs\openFrameworks\utils\ofConstants.h"
    if (-not (Test-Path -LiteralPath $Constants -PathType Leaf)) {
        throw "setup-of: verified archive omitted openFrameworks version constants"
    }
    $VersionParts = @{}
    foreach ($line in Get-Content -LiteralPath $Constants) {
        if ($line -match '^#define\s+OF_VERSION_(MAJOR|MINOR|PATCH)\s+(\d+)\s*$') {
            $VersionParts[$Matches[1]] = $Matches[2]
        }
    }
    $Version = "$($VersionParts.MAJOR).$($VersionParts.MINOR).$($VersionParts.PATCH)"
    if ($Version -ne "0.12.1") {
        throw "setup-of: verified archive contains openFrameworks $Version, expected 0.12.1"
    }

    if (Get-Item -LiteralPath $Backup -Force -ErrorAction SilentlyContinue) { throw "setup-of: refusing unexpected backup path: $Backup" }
    $HadInstalled = $null -ne (Get-Item -LiteralPath $Installed -Force -ErrorAction SilentlyContinue)
    if ($HadInstalled) { Move-Item -LiteralPath $Installed -Destination $Backup }
    try {
        Move-Item -LiteralPath $StagedRoot -Destination $Installed
    } catch {
        if ($HadInstalled -and (Get-Item -LiteralPath $Backup -Force -ErrorAction SilentlyContinue)) {
            try { Move-Item -LiteralPath $Backup -Destination $Installed }
            catch { throw "setup-of: install failed and the prior tree remains at $Backup" }
        }
        throw "setup-of: could not install freshly verified openFrameworks tree"
    }
    if (Get-Item -LiteralPath $Backup -Force -ErrorAction SilentlyContinue) {
        $backupItem = Get-Item -LiteralPath $Backup -Force
        if ($backupItem.LinkType) { Remove-Item -LiteralPath $Backup -Force }
        else { Remove-Item -LiteralPath $Backup -Recurse -Force }
    }
} finally {
    if (Test-Path -LiteralPath $Staging) { Remove-Item -LiteralPath $Staging -Recurse -Force }
}
Write-Output "OF_ROOT=$Installed"
