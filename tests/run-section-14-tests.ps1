$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $Compiler) { throw "Visual Studio cl.exe is required; run from a Developer PowerShell" }
$Build = Join-Path ([IO.Path]::GetTempPath()) ("section-14-test-" + [guid]::NewGuid().ToString("N"))
$Output = Join-Path $Build "image-geometry-model-test.exe"
New-Item -ItemType Directory -Force $Build | Out-Null
try {
 Push-Location $Build
 try {
  & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc "/I$(Join-Path $Root 'exercises\14-images-and-type-as-geometry\shared')" "/I$(Join-Path $Root 'exercises\14-images-and-type-as-geometry\starter\src\design')" (Join-Path $Root "exercises\14-images-and-type-as-geometry\shared\image_geometry_model.cpp") (Join-Path $Root "exercises\14-images-and-type-as-geometry\starter\src\design\image_geometry_design.cpp") (Join-Path $Root "exercises\14-images-and-type-as-geometry\tests\image_geometry_model_test.cpp") "/Fe:$Output"
  if ($LASTEXITCODE -ne 0) { throw "section 14 test compilation failed" }
 } finally { Pop-Location }
 & $Output (Join-Path $Root "exercises\14-images-and-type-as-geometry\fixtures\mask-oracle.txt")
 if ($LASTEXITCODE -ne 0) { throw "section 14 tests failed" }
 Push-Location $Build
 try {
  & $Compiler.Source /nologo /std:c++17 /Zc:__cplusplus /W4 /WX /EHsc /c `
    "/I$(Join-Path $Root 'exercises\14-images-and-type-as-geometry\shared')" `
    "/I$(Join-Path $Root 'exercises\14-images-and-type-as-geometry\solution\src\design')" `
    (Join-Path $Root "exercises\14-images-and-type-as-geometry\solution\src\design\image_geometry_design.cpp") `
    "/Fo:$(Join-Path $Build 'solution-design.obj')"
  if ($LASTEXITCODE -ne 0) { throw "section 14 solution design compilation failed" }
 } finally { Pop-Location }
 Write-Host "section-14-solution-seam: divergent bounded design compiles independently"
 foreach ($Adapter in @(
  (Join-Path $Root "exercises\14-images-and-type-as-geometry\starter\src\ofApp.cpp"),
  (Join-Path $Root "exercises\14-images-and-type-as-geometry\solution\src\ofApp.cpp")
 )) {
  $AdapterText = Get-Content -Raw -LiteralPath $Adapter
  foreach ($Needle in @('imageGeometryDesignIsValid(design_)', 'Transform error:')) {
   if (-not $AdapterText.Contains($Needle)) { throw "$Adapter is missing adapter contract: $Needle" }
  }
 }
 Write-Host "section-14-adapter-contract: invalid design and transform failures are visibly reported"
 $Malformed = Join-Path $Build "malformed.txt"
 Set-Content -LiteralPath $Malformed -Value "broken 3 3 1"
 $Text = @(& $Output $Malformed 2>&1) -join "`n"; $Status = $LASTEXITCODE
 if ($Status -eq 0 -or $Text -notmatch "broken oracle cardinality") { throw "malformed fixture did not fail safely" }
 Write-Host "section-14-fixture-safety: malformed row failed cardinality check before allocation/indexing"
} finally { Remove-Item -LiteralPath $Build -Recurse -Force -ErrorAction SilentlyContinue }
$FakeOf = Join-Path ([IO.Path]::GetTempPath()) ("section-14-incomplete-of-" + [guid]::NewGuid().ToString("N"))
$SentinelDirectory = Join-Path $Root "exercises\14-images-and-type-as-geometry\starter\bin"
$Sentinel = Join-Path $SentinelDirectory ("wrapper-safety-sentinel-" + [guid]::NewGuid().ToString("N"))
try {
 $Generator = Join-Path $FakeOf "projectGenerator\resources\app\app\projectGenerator.exe"
 New-Item -ItemType Directory -Force (Split-Path -Parent $Generator),$SentinelDirectory | Out-Null
 Set-Content -LiteralPath $Generator -Value "not executable"; Set-Content -LiteralPath $Sentinel -Value "preserve me"
 $Rejected=$false
 try { & (Join-Path $Root "scripts\section-14.ps1") generate -OfRoot $FakeOf -Project starter } catch { $Rejected=$true }
 if (-not $Rejected) { throw "incomplete OF_ROOT unexpectedly passed" }
 if (-not (Test-Path -LiteralPath $Sentinel -PathType Leaf)) { throw "failed doctor removed output" }
 Write-Host "section-14-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved"
} finally { Remove-Item -LiteralPath $FakeOf -Recurse -Force -ErrorAction SilentlyContinue; Remove-Item -LiteralPath $Sentinel -Force -ErrorAction SilentlyContinue }
exit 0
