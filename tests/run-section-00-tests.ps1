[CmdletBinding()]
param(
    [ValidateSet("starter", "solution")]
    [string]$Variant = "starter"
)
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Build = Join-Path ([IO.Path]::GetTempPath()) ("section-00-tests-" + [guid]::NewGuid().ToString("N"))
New-Item -ItemType Directory -Path $Build | Out-Null
try {
    $Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
    if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
    $Shared = Join-Path $Root "exercises/00-visual-signature/shared"
    $Design = Join-Path $Root "exercises/00-visual-signature/$Variant/src/design"
    $TestSource = Join-Path $Root "exercises/00-visual-signature/tests/signature_geometry_test.cpp"
    Push-Location $Build
    try {
        & $Compiler.Source /nologo /std:c++14 /Zc:__cplusplus /EHsc "/I$Shared" "/I$Design" `
            /c $TestSource "/Fo:$(Join-Path $Build 'should-not-compile.obj')" *> (Join-Path $Build "cxx14.log")
        if ($LASTEXITCODE -eq 0) { throw "section 00 test source unexpectedly compiled below C++17" }
        & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /EHsc /W4 /WX "/I$Shared" "/I$Design" `
            (Join-Path $Shared "signature_geometry.cpp") `
            (Join-Path $Design "signature_design.cpp") `
            $TestSource `
            "/Fe:$(Join-Path $Build 'signature_geometry_test.exe')"
        if ($LASTEXITCODE -ne 0) { throw "section 00 test compilation failed with exit code $LASTEXITCODE" }
    } finally { Pop-Location }
    & (Join-Path $Build "signature_geometry_test.exe") (Join-Path $Root "exercises/00-visual-signature/fixtures/viewports.tsv")
    if ($LASTEXITCODE -ne 0) { throw "section 00 tests failed with exit code $LASTEXITCODE" }
} finally {
    Remove-Item -LiteralPath $Build -Recurse -Force -ErrorAction SilentlyContinue
}
