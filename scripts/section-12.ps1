[CmdletBinding()]
param(
    [Parameter(Position = 0, Mandatory = $true)][ValidateSet("doctor", "generate", "build")][string]$Command,
    [string]$OfRoot,
    [ValidateSet("starter", "solution")][string]$Project = "starter",
    [ValidateSet("Debug", "Release")][string]$Configuration = "Release"
)
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$CommonPath = Join-Path $PSScriptRoot "course-project.ps1"
$Common = Get-Content -Raw -LiteralPath $CommonPath
$Common = $Common.Replace(
    '[ValidateSet("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11")]',
    '[ValidateSet("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12")]')
$Common = $Common.Replace(
    '$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path',
    '$Root = $env:SECTION_12_ROOT')
$Common = $Common.Replace(
    'else { $Exercise = "11-noise-and-flow-fields"; $Design = "flow_field_design"; $Shared = "flow_field_model" }',
    'elseif ($Section -eq "11") { $Exercise = "11-noise-and-flow-fields"; $Design = "flow_field_design"; $Shared = "flow_field_model" }' + "`n" +
    'else { $Exercise = "12-color-blending-and-trails"; $Design = "trail_design"; $Shared = "color_trail_model" }')
$Temp = Join-Path ([IO.Path]::GetTempPath()) ("section-12-course-project-" + [guid]::NewGuid().ToString("N") + ".ps1")
$PreviousRoot = $env:SECTION_12_ROOT
try {
    Set-Content -LiteralPath $Temp -Value $Common -NoNewline
    $env:SECTION_12_ROOT = $Root
    & $Temp -Section "12" -Command $Command -OfRoot $OfRoot -Project $Project -Configuration $Configuration
} finally {
    $env:SECTION_12_ROOT = $PreviousRoot
    Remove-Item -LiteralPath $Temp -Force -ErrorAction SilentlyContinue
}
