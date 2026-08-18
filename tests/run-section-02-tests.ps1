$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$Output = Join-Path ([IO.Path]::GetTempPath()) ("section-02-test-" + [guid]::NewGuid().ToString("N") + ".exe")
try {
    & $Compiler.Source /nologo /std:c++17 /W4 /WX /EHsc "/I$(Join-Path $Root 'exercises\02-python-to-cpp-survival-kit\shared')" "/I$(Join-Path $Root 'exercises\02-python-to-cpp-survival-kit\starter\src\design')" (Join-Path $Root "exercises\02-python-to-cpp-survival-kit\shared\mark_family.cpp") (Join-Path $Root "exercises\02-python-to-cpp-survival-kit\starter\src\design\family_design.cpp") (Join-Path $Root "exercises\02-python-to-cpp-survival-kit\tests\mark_family_test.cpp") "/Fe:$Output"
    if ($LASTEXITCODE -ne 0) { throw "section 02 test compilation failed" }
    & $Output (Join-Path $Root "exercises\02-python-to-cpp-survival-kit\fixtures\family-cases.tsv")
    if ($LASTEXITCODE -ne 0) { throw "section 02 tests failed" }
} finally { Remove-Item -LiteralPath $Output -Force -ErrorAction SilentlyContinue }

$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-02-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$Sentinel = Join-Path $Root "exercises\02-python-to-cpp-survival-kit\starter\bin\wrapper-safety-sentinel"
try {
    $FakeGenerator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
    New-Item -ItemType Directory -Force (Split-Path -Parent $FakeGenerator),(Split-Path -Parent $Sentinel) | Out-Null
    Set-Content -LiteralPath $FakeGenerator -Value "not an executable"
    Set-Content -LiteralPath $Sentinel -Value "preserve me"
    $Rejected = $false
    try { & (Join-Path $Root "scripts\section-02.ps1") generate -OfRoot $FakeOf -Project starter } catch { $Rejected = $true }
    if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed generation" }
    if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) { throw "failed doctor removed existing output" }
    Write-Host "section-02-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved"
} finally {
    Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue
}
