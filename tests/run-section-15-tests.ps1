$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$BuildDirectory = Join-Path ([IO.Path]::GetTempPath()) ("section-15-test-" + [guid]::NewGuid().ToString("N"))
$Output = Join-Path $BuildDirectory "audio-input-model-test.exe"
New-Item -ItemType Directory -Force $BuildDirectory | Out-Null
try {
    Push-Location $BuildDirectory
    try {
        & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc `
            "/I$(Join-Path $Root 'exercises\15-embodied-audio-input\shared')" `
            "/I$(Join-Path $Root 'exercises\15-embodied-audio-input\starter\src\design')" `
            (Join-Path $Root "exercises\15-embodied-audio-input\shared\audio_input_model.cpp") `
            (Join-Path $Root "exercises\15-embodied-audio-input\starter\src\design\audio_instrument_design.cpp") `
            (Join-Path $Root "exercises\15-embodied-audio-input\tests\audio_input_model_test.cpp") `
            "/Fe:$Output"
        if ($LASTEXITCODE -ne 0) { throw "section 15 test compilation failed" }
    } finally { Pop-Location }

    & $Output (Join-Path $Root "exercises\15-embodied-audio-input\fixtures\amplitude-replay.txt")
    if ($LASTEXITCODE -ne 0) { throw "section 15 tests failed" }

    Push-Location $BuildDirectory
    try {
        & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc /c `
            "/I$(Join-Path $Root 'exercises\15-embodied-audio-input\shared')" `
            "/I$(Join-Path $Root 'exercises\15-embodied-audio-input\solution\src\design')" `
            (Join-Path $Root "exercises\15-embodied-audio-input\solution\src\design\audio_instrument_design.cpp") `
            "/Fo:$(Join-Path $BuildDirectory 'solution-design.obj')"
        if ($LASTEXITCODE -ne 0) { throw "solution design seam compilation failed" }
    } finally { Pop-Location }
    Write-Host "section-15-solution-seam: divergent solution design compiles independently"

    foreach ($Adapter in @(
        (Join-Path $Root "exercises\15-embodied-audio-input\starter\src\ofApp.cpp"),
        (Join-Path $Root "exercises\15-embodied-audio-input\solution\src\ofApp.cpp")
    )) {
        $Text = Get-Content -Raw -LiteralPath $Adapter
        foreach ($Needle in @("chooseNoDevice();  // Safe default", "key == 'f'", "key == 'l'", "key == 'n'", "sound_stream_.setup(settings)", "sound_stream_.close()", "pending_amplitude_.exchange(-1.0f)", "ACTIVITY: ACTIVE", "viewport_width < 680 || viewport_height < 360", "compactInputStatusForWidth(source, viewport_width)", "fittedInputStatus(state_.source, viewport_width)", "viewport_height >= 12", 'bitmapTextFits("RESIZE TO 680 x 360", viewport_width)')) {
            if (-not $Text.Contains($Needle)) { throw "$Adapter is missing adapter contract: $Needle" }
        }
    }
    Write-Host "section-15-adapter-contract: safe default, N/F/L routes, live open/close hooks, and tiny-window fallback are discoverable"

    $Malformed = Join-Path $BuildDirectory "malformed-amplitudes.txt"
    @(
        "short 0.1 0.05",
        "long 0.2 0.1 0.0 20.0 4 0 unexpected"
    ) | Set-Content -LiteralPath $Malformed
    $MalformedOutput = @(& $Output $Malformed 2>&1)
    $MalformedStatus = $LASTEXITCODE
    if ($MalformedStatus -eq 0) { throw "malformed fixture unexpectedly passed" }
    if (($MalformedOutput -join "`n") -notmatch "must have exactly seven fields") {
        throw "malformed fixture did not fail through cardinality checks"
    }
    Write-Host "section-15-fixture-safety: malformed rows failed cleanly before use"
} finally {
    Remove-Item -LiteralPath $BuildDirectory -Recurse -Force -ErrorAction SilentlyContinue
}

$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-15-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$SentinelDirectory = Join-Path $Root "exercises\15-embodied-audio-input\starter\bin"
$Sentinel = Join-Path $SentinelDirectory ("wrapper-safety-sentinel-" + [guid]::NewGuid().ToString("N"))
$OwnsSentinel = $false
try {
    $FakeGenerator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
    New-Item -ItemType Directory -Force (Split-Path -Parent $FakeGenerator),$SentinelDirectory | Out-Null
    if (Test-Path -LiteralPath $Sentinel) { throw "unique wrapper-safety sentinel path already exists" }
    Set-Content -LiteralPath $FakeGenerator -Value "not an executable"
    Set-Content -LiteralPath $Sentinel -Value "preserve me"
    $OwnsSentinel = $true
    $Rejected = $false
    try {
        & (Join-Path $Root "scripts\section-15.ps1") generate -OfRoot $FakeOf -Project starter
    } catch { $Rejected = $true }
    if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed generation" }
    if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) {
        throw "failed doctor removed existing output"
    }
    Write-Host "section-15-wrapper-safety: incomplete OF_ROOT rejected before cleanup; unique sentinel preserved"
} finally {
    Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue
    if ($OwnsSentinel) { Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue }
}

# The deliberately rejected wrapper probe leaves LASTEXITCODE nonzero.
exit 0
