$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$Output = Join-Path ([IO.Path]::GetTempPath()) ("section-16-test-" + [guid]::NewGuid().ToString("N") + ".exe")
$SolutionObject = Join-Path ([IO.Path]::GetTempPath()) ("section-16-solution-" + [guid]::NewGuid().ToString("N") + ".obj")
try {
    & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc "/I$(Join-Path $Root 'exercises\16-structured-chance\shared')" "/I$(Join-Path $Root 'exercises\16-structured-chance\starter\src\design')" (Join-Path $Root "exercises\16-structured-chance\shared\structured_chance_model.cpp") (Join-Path $Root "exercises\16-structured-chance\starter\src\design\structured_chance_design.cpp") (Join-Path $Root "exercises\16-structured-chance\tests\structured_chance_model_test.cpp") "/Fe:$Output"
    if ($LASTEXITCODE -ne 0) { throw "section 16 test compilation failed" }
    & $Output
    if ($LASTEXITCODE -ne 0) { throw "section 16 tests failed" }
    & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc /c "/I$(Join-Path $Root 'exercises\16-structured-chance\shared')" "/I$(Join-Path $Root 'exercises\16-structured-chance\solution\src\design')" (Join-Path $Root "exercises\16-structured-chance\solution\src\design\structured_chance_design.cpp") "/Fo:$SolutionObject"
    if ($LASTEXITCODE -ne 0) { throw "section 16 solution design compilation failed" }
    Write-Host "section-16-solution-seam: divergent solution design compiles independently"
} finally { Remove-Item -LiteralPath $Output,$SolutionObject -Force -ErrorAction SilentlyContinue }

$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-16-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$Sentinel = Join-Path $Root "exercises\16-structured-chance\starter\bin\wrapper-safety-sentinel"
try {
    $FakeGenerator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
    New-Item -ItemType Directory -Force (Split-Path -Parent $FakeGenerator),(Split-Path -Parent $Sentinel) | Out-Null
    Set-Content -LiteralPath $FakeGenerator -Value "not an executable"
    Set-Content -LiteralPath $Sentinel -Value "preserve me"
    $Rejected = $false
    try { & (Join-Path $Root "scripts\section-16.ps1") generate -OfRoot $FakeOf -Project starter } catch { $Rejected = $true }
    if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed generation" }
    if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) { throw "failed doctor removed existing output" }
    Write-Host "section-16-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved"
} finally {
    Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue
}
