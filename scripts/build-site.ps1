[CmdletBinding()]
param()
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path -LiteralPath (Split-Path -Parent $PSScriptRoot)).Path
$Destination = Join-Path $Root ".hugo-public"

& (Join-Path $Root "scripts/check-authoring.ps1") -RequireHugo
if ($LASTEXITCODE -ne 0) { throw "publication build: authoring validation failed" }

& hugo --source $Root `
    --config site/hugo.toml `
    --destination $Destination `
    --cleanDestinationDir `
    --panicOnWarning `
    --printPathWarnings
if ($LASTEXITCODE -ne 0) { throw "publication build: Hugo failed" }
Write-Host "publication build: $Destination"
