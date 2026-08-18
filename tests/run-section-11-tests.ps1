$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$BuildDirectory = Join-Path ([IO.Path]::GetTempPath()) ("section-11-test-" + [guid]::NewGuid().ToString("N"))
$Output = Join-Path $BuildDirectory "flow-field-model-test.exe"
New-Item -ItemType Directory -Force $BuildDirectory | Out-Null
try {
    Push-Location $BuildDirectory
    try {
        & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc "/I$(Join-Path $Root 'exercises\11-noise-and-flow-fields\shared')" "/I$(Join-Path $Root 'exercises\11-noise-and-flow-fields\starter\src\design')" (Join-Path $Root "exercises\11-noise-and-flow-fields\shared\flow_field_model.cpp") (Join-Path $Root "exercises\11-noise-and-flow-fields\starter\src\design\flow_field_design.cpp") (Join-Path $Root "exercises\11-noise-and-flow-fields\tests\flow_field_model_test.cpp") "/Fe:$Output"
        if ($LASTEXITCODE -ne 0) { throw "section 11 test compilation failed" }
    } finally { Pop-Location }
    & $Output (Join-Path $Root "exercises\11-noise-and-flow-fields\fixtures\flow-oracle.txt")
    if ($LASTEXITCODE -ne 0) { throw "section 11 tests failed" }
    $Malformed = Join-Path $BuildDirectory "malformed-oracle.txt"
    @(
        "hash-origin hash 0 0 0 1 1244833532",
        "hash-offset hash 3 5 7 42 1072089522",
        "hash-negative hash -2 4 -6 99 930434099",
        "bad-sample sample 0.5",
        "bad-angle angle 0.25 0.0"
    ) | Set-Content -LiteralPath $Malformed
    $MalformedOutput = @(& $Output $Malformed 2>&1)
    $MalformedStatus = $LASTEXITCODE
    if ($MalformedStatus -eq 0) { throw "malformed fixture unexpectedly passed" }
    $MalformedText = $MalformedOutput -join "`n"
    if ($MalformedText -notmatch "bad-sample has sample fields" -or
        $MalformedText -notmatch "bad-angle has angle fields") {
        throw "malformed fixture did not fail through cardinality checks"
    }
    Write-Host "section-11-fixture-safety: malformed sample/angle rows failed cleanly before indexing"
} finally { Remove-Item -LiteralPath $BuildDirectory -Recurse -Force -ErrorAction SilentlyContinue }
$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-11-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$SentinelDirectory = Join-Path $Root "exercises\11-noise-and-flow-fields\starter\bin"
$Sentinel = Join-Path $SentinelDirectory ("wrapper-safety-sentinel-" + [guid]::NewGuid().ToString("N"))
$OwnsSentinel = $false
try {
    $FakeGenerator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
    New-Item -ItemType Directory -Force (Split-Path -Parent $FakeGenerator),$SentinelDirectory | Out-Null
    if (Test-Path -LiteralPath $Sentinel) { throw "unique wrapper-safety sentinel path already exists" }
    Set-Content -LiteralPath $FakeGenerator -Value "not an executable"; Set-Content -LiteralPath $Sentinel -Value "preserve me"
    $OwnsSentinel = $true
    $Rejected = $false
    try { & (Join-Path $Root "scripts\section-11.ps1") generate -OfRoot $FakeOf -Project starter } catch { $Rejected = $true }
    if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed generation" }
    if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) { throw "failed doctor removed existing output" }
    Write-Host "section-11-wrapper-safety: incomplete OF_ROOT rejected before cleanup; unique sentinel preserved"
} finally {
    Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue
    if ($OwnsSentinel) { Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue }
}
