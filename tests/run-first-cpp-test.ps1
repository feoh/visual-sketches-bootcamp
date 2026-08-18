param(
    [ValidateSet("starter", "solution")]
    [string]$Variant = "starter"
)

$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$Output = Join-Path ([IO.Path]::GetTempPath()) ("first-cpp-test-" + [guid]::NewGuid().ToString("N") + ".exe")
try {
    & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /DNDEBUG /W4 /WX /EHsc `
        "/I$(Join-Path $Root 'shared\core')" `
        "/I$(Join-Path $Root 'shared\test-support')" `
        "/I$(Join-Path $Root "exercises\00-first-cpp-test\$Variant")" `
        (Join-Path $Root "shared\core\course_probe.cpp") `
        (Join-Path $Root "exercises\00-first-cpp-test\$Variant\learner_known_case.cpp") `
        (Join-Path $Root "exercises\00-first-cpp-test\tests\learner_known_case_test.cpp") `
        "/Fe:$Output"
    if ($LASTEXITCODE -ne 0) { throw "first C++ test compilation failed" }
    & $Output
    if ($LASTEXITCODE -ne 0) { throw "first C++ test failed" }
} finally {
    Remove-Item -LiteralPath $Output -Force -ErrorAction SilentlyContinue
}
