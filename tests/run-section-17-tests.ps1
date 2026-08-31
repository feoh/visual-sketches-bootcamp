$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$Output = Join-Path ([IO.Path]::GetTempPath()) ("section-17-test-" + [guid]::NewGuid().ToString("N") + ".exe")
$SolutionObject = Join-Path ([IO.Path]::GetTempPath()) ("section-17-solution-" + [guid]::NewGuid().ToString("N") + ".obj")
try {
    & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc "/I$(Join-Path $Root 'exercises\17-depth-light-and-populations\shared')" "/I$(Join-Path $Root 'exercises\17-depth-light-and-populations\starter\src\design')" (Join-Path $Root "exercises\17-depth-light-and-populations\shared\population_model.cpp") (Join-Path $Root "exercises\17-depth-light-and-populations\starter\src\design\population_design.cpp") (Join-Path $Root "exercises\17-depth-light-and-populations\tests\population_model_test.cpp") "/Fe:$Output"
    if ($LASTEXITCODE -ne 0) { throw "section 17 test compilation failed" }
    & $Output
    if ($LASTEXITCODE -ne 0) { throw "section 17 tests failed" }
    & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc /c "/I$(Join-Path $Root 'exercises\17-depth-light-and-populations\shared')" "/I$(Join-Path $Root 'exercises\17-depth-light-and-populations\solution\src\design')" (Join-Path $Root "exercises\17-depth-light-and-populations\solution\src\design\population_design.cpp") "/Fo:$SolutionObject"
    if ($LASTEXITCODE -ne 0) { throw "section 17 solution design compilation failed" }
    Write-Host "section-17-solution-seam: divergent solution design compiles independently"
} finally { Remove-Item -LiteralPath $Output,$SolutionObject -Force -ErrorAction SilentlyContinue }

$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-17-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$Sentinel = Join-Path $Root "exercises\17-depth-light-and-populations\starter\bin\wrapper-safety-sentinel"
try {
    $FakeGenerator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
    New-Item -ItemType Directory -Force (Split-Path -Parent $FakeGenerator),(Split-Path -Parent $Sentinel) | Out-Null
    Set-Content -LiteralPath $FakeGenerator -Value "not an executable"
    Set-Content -LiteralPath $Sentinel -Value "preserve me"
    $Rejected = $false
    try { & (Join-Path $Root "scripts\section-17.ps1") generate -OfRoot $FakeOf -Project starter } catch { $Rejected = $true }
    if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed generation" }
    if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) { throw "failed doctor removed existing output" }
    Write-Host "section-17-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved"
} finally {
    Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue
}
