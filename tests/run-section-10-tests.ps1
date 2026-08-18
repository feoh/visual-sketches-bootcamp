$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$BuildDirectory = Join-Path ([IO.Path]::GetTempPath()) ("section-10-test-" + [guid]::NewGuid().ToString("N"))
$Output = Join-Path $BuildDirectory "force-model-test.exe"
New-Item -ItemType Directory -Force $BuildDirectory | Out-Null
try {
    Push-Location $BuildDirectory
    try {
        & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc "/I$(Join-Path $Root 'exercises\10-forces-steering-and-springs\shared')" "/I$(Join-Path $Root 'exercises\10-forces-steering-and-springs\starter\src\design')" (Join-Path $Root "exercises\10-forces-steering-and-springs\shared\force_model.cpp") (Join-Path $Root "exercises\10-forces-steering-and-springs\starter\src\design\force_design.cpp") (Join-Path $Root "exercises\10-forces-steering-and-springs\tests\force_model_test.cpp") "/Fe:$Output"
        if ($LASTEXITCODE -ne 0) { throw "section 10 test compilation failed" }
    } finally { Pop-Location }
    & $Output (Join-Path $Root "exercises\10-forces-steering-and-springs\fixtures\spring-oracle.txt")
    if ($LASTEXITCODE -ne 0) { throw "section 10 tests failed" }
} finally { Remove-Item -LiteralPath $BuildDirectory -Recurse -Force -ErrorAction SilentlyContinue }
$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-10-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$Sentinel = Join-Path $Root "exercises\10-forces-steering-and-springs\starter\bin\wrapper-safety-sentinel"
try {
    $FakeGenerator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
    New-Item -ItemType Directory -Force (Split-Path -Parent $FakeGenerator),(Split-Path -Parent $Sentinel) | Out-Null
    Set-Content -LiteralPath $FakeGenerator -Value "not an executable"; Set-Content -LiteralPath $Sentinel -Value "preserve me"
    $Rejected = $false
    try { & (Join-Path $Root "scripts\section-10.ps1") generate -OfRoot $FakeOf -Project starter } catch { $Rejected = $true }
    if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed generation" }
    if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) { throw "failed doctor removed existing output" }
    Write-Host "section-10-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved"
} finally { Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue; Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue }
