$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$BuildDirectory = Join-Path ([IO.Path]::GetTempPath()) ("section-13-test-" + [guid]::NewGuid().ToString("N"))
$Output = Join-Path $BuildDirectory "temporal-history-test.exe"
New-Item -ItemType Directory -Force $BuildDirectory | Out-Null
try {
    Push-Location $BuildDirectory
    try {
        & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc `
            "/I$(Join-Path $Root 'exercises\13-time-as-a-drawable-axis\shared')" `
            "/I$(Join-Path $Root 'exercises\13-time-as-a-drawable-axis\starter\src\design')" `
            (Join-Path $Root "exercises\13-time-as-a-drawable-axis\shared\temporal_history.cpp") `
            (Join-Path $Root "exercises\13-time-as-a-drawable-axis\starter\src\design\temporal_design.cpp") `
            (Join-Path $Root "exercises\13-time-as-a-drawable-axis\tests\temporal_history_test.cpp") "/Fe:$Output"
        if ($LASTEXITCODE -ne 0) { throw "section 13 test compilation failed" }
    } finally { Pop-Location }
    & $Output (Join-Path $Root "exercises\13-time-as-a-drawable-axis\fixtures\temporal-oracle.txt")
    if ($LASTEXITCODE -ne 0) { throw "section 13 tests failed" }
    $Malformed = Join-Path $BuildDirectory "malformed-oracle.txt"
    @("bad-motion motion 0 0 100", "bad-selection selection 5") | Set-Content -LiteralPath $Malformed
    $MalformedOutput = @(& $Output $Malformed 2>&1)
    if ($LASTEXITCODE -eq 0) { throw "malformed fixture unexpectedly passed" }
    if (($MalformedOutput -join "`n") -notmatch "bad-motion has motion fields") { throw "malformed fixture did not fail through cardinality check" }
    Write-Host "section-13-fixture-safety: malformed fixture rejected before field indexing"
} finally { Remove-Item -LiteralPath $BuildDirectory -Recurse -Force -ErrorAction SilentlyContinue }
$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-13-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$SentinelDirectory = Join-Path $Root "exercises\13-time-as-a-drawable-axis\starter\bin"
$Sentinel = Join-Path $SentinelDirectory ("wrapper-safety-sentinel-" + [guid]::NewGuid().ToString("N"))
try {
    $FakeGenerator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
    New-Item -ItemType Directory -Force (Split-Path -Parent $FakeGenerator),$SentinelDirectory | Out-Null
    Set-Content -LiteralPath $FakeGenerator -Value "not an executable"
    Set-Content -LiteralPath $Sentinel -Value "preserve me"
    $Rejected = $false
    try { & (Join-Path $Root "scripts\section-13.ps1") generate -OfRoot $FakeOf -Project starter } catch { $Rejected = $true }
    if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed generation" }
    if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) { throw "failed doctor removed existing output" }
    $StarterDesign = Get-Content -Raw -LiteralPath (Join-Path $Root "exercises\13-time-as-a-drawable-axis\starter\src\design\temporal_design.cpp")
    $SolutionDesign = Get-Content -Raw -LiteralPath (Join-Path $Root "exercises\13-time-as-a-drawable-axis\solution\src\design\temporal_design.cpp")
    if ($StarterDesign -ceq $SolutionDesign) { throw "starter and solution design seams must diverge" }
    Write-Host "section-13-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved; designs diverge"
} finally {
    Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue
}
exit 0
