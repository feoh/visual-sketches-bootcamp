[CmdletBinding()]
param(
    [Parameter(Position = 0, Mandatory = $true)][ValidateSet("doctor", "generate", "build")][string]$Command,
    [string]$OfRoot,
    [ValidateSet("starter", "solution")][string]$Project = "starter",
    [ValidateSet("Debug", "Release")][string]$Configuration = "Release"
)
& (Join-Path $PSScriptRoot "course-project.ps1") -Section "02" -Command $Command -OfRoot $OfRoot -Project $Project -Configuration $Configuration
