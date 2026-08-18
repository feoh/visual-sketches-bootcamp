$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$BuildDirectory = Join-Path ([IO.Path]::GetTempPath()) ("section-12-test-" + [guid]::NewGuid().ToString("N"))
$Output = Join-Path $BuildDirectory "color-trail-model-test.exe"
New-Item -ItemType Directory -Force $BuildDirectory | Out-Null
try {
    Push-Location $BuildDirectory
    try {
        & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc "/I$(Join-Path $Root 'exercises\12-color-blending-and-trails\shared')" "/I$(Join-Path $Root 'exercises\12-color-blending-and-trails\starter\src\design')" (Join-Path $Root "exercises\12-color-blending-and-trails\shared\color_trail_model.cpp") (Join-Path $Root "exercises\12-color-blending-and-trails\starter\src\design\trail_design.cpp") (Join-Path $Root "exercises\12-color-blending-and-trails\tests\color_trail_model_test.cpp") "/Fe:$Output"
        if ($LASTEXITCODE -ne 0) { throw "section 12 test compilation failed" }
    } finally { Pop-Location }
    & $Output (Join-Path $Root "exercises\12-color-blending-and-trails\fixtures\trail-oracle.txt")
    if ($LASTEXITCODE -ne 0) { throw "section 12 tests failed" }
    $Malformed = Join-Path $BuildDirectory "malformed-oracle.txt"
    @(
        "palette-start palette 0.0 0.10 0.20 0.30 1.0",
        "palette-middle palette 0.5 0.50 0.40 0.30 0.75",
        "palette-end palette 1.0 0.90 0.60 0.30 0.5",
        "bad-over over 1.0 0.0",
        "bad-decay decay 0.75"
    ) | Set-Content -LiteralPath $Malformed
    $MalformedOutput = @(& $Output $Malformed 2>&1)
    $MalformedStatus = $LASTEXITCODE
    if ($MalformedStatus -eq 0) { throw "malformed fixture unexpectedly passed" }
    $MalformedText = $MalformedOutput -join "`n"
    if ($MalformedText -notmatch "bad-over has over fields" -or
        $MalformedText -notmatch "bad-decay has decay fields") {
        throw "malformed fixture did not fail through cardinality checks"
    }
    Write-Host "section-12-fixture-safety: malformed over/decay rows failed cleanly before indexing"
} finally { Remove-Item -LiteralPath $BuildDirectory -Recurse -Force -ErrorAction SilentlyContinue }
foreach ($Section in @("12-color-blending-and-trails", "13-time-as-a-drawable-axis", "14-images-and-type-as-geometry", "15-embodied-audio-input")) {
    foreach ($Variant in @("starter", "solution")) {
        $Main = Get-Content -Raw -LiteralPath (Join-Path $Root "exercises\$Section\$Variant\src\main.cpp")
        foreach ($Needle in @("ofCreateWindow(settings)", "ofRunApp(window, std::make_shared<ofApp>())", "ofRunMainLoop()")) {
            if (-not $Main.Contains($Needle)) { throw "$Section/$Variant main.cpp is missing window contract: $Needle" }
        }
    }
}
Write-Host "unit-4-window-contract: every entry point creates and runs its configured window"
$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-12-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$SentinelDirectory = Join-Path $Root "exercises\12-color-blending-and-trails\starter\bin"
$Sentinel = Join-Path $SentinelDirectory ("wrapper-safety-sentinel-" + [guid]::NewGuid().ToString("N"))
$OwnsSentinel = $false
try {
    $FakeGenerator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
    New-Item -ItemType Directory -Force (Split-Path -Parent $FakeGenerator),$SentinelDirectory | Out-Null
    if (Test-Path -LiteralPath $Sentinel) { throw "unique wrapper-safety sentinel path already exists" }
    Set-Content -LiteralPath $FakeGenerator -Value "not an executable"; Set-Content -LiteralPath $Sentinel -Value "preserve me"
    $OwnsSentinel = $true
    $Rejected = $false
    try { & (Join-Path $Root "scripts\section-12.ps1") generate -OfRoot $FakeOf -Project starter } catch { $Rejected = $true }
    if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed generation" }
    if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) { throw "failed doctor removed existing output" }
    Write-Host "section-12-wrapper-safety: incomplete OF_ROOT rejected before cleanup; unique sentinel preserved"
} finally {
    Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue
    if ($OwnsSentinel) { Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue }
}
exit 0
