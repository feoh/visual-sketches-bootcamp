[CmdletBinding()]
param([switch]$RequireHugo)
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Work = Join-Path ([IO.Path]::GetTempPath()) ("authoring-check-tests-" + [guid]::NewGuid().ToString("N"))
function Make-Fixture([string]$Name) {
    $fixture = Join-Path $Work $Name
    New-Item -ItemType Directory -Force -Path (Join-Path $fixture "scripts") | Out-Null

    # Copy only tracked and relevant untracked inputs. Recursive copies also pick
    # up ignored native build products under exercises/, multiplying gigabytes
    # of disposable files across every negative fixture.
    $relativePaths = @(& git -C $Root ls-files --cached --others --exclude-standard -- `
        authoring exercises docs/pilot docs/pilot-protocol-and-evidence.md `
        THIRD_PARTY_NOTICES.md docs/source-notes.md docs/foundation-harness-evidence.md `
        foundation/windowed/src/ofApp.cpp)
    if ($LASTEXITCODE -ne 0) { throw "git ls-files failed while creating fixture '$Name'" }
    foreach ($relative in $relativePaths) {
        if ([string]::IsNullOrWhiteSpace($relative)) { continue }
        $destination = Join-Path $fixture $relative
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
        Copy-Item -LiteralPath (Join-Path $Root $relative) -Destination $destination
    }

    Copy-Item -LiteralPath (Join-Path $Root "scripts/check-authoring.ps1") -Destination (Join-Path $fixture "scripts/check-authoring.ps1")
    return $fixture
}
function Expect-Failure([string]$Name, [string]$Expected, [scriptblock]$Mutate) {
    $fixture = Make-Fixture $Name
    & $Mutate $fixture
    $message = ""
    try { & (Join-Path $fixture "scripts/check-authoring.ps1"); throw "negative test unexpectedly passed: $Name" }
    catch { $message = $_.Exception.Message }
    if ($message -eq "negative test unexpectedly passed: $Name") { throw $message }
    if (-not $message.Contains($Expected)) { throw "negative test '$Name' lacked '$Expected': $message" }
}
try {
    & (Join-Path $Root "scripts/check-authoring.ps1") -RequireHugo:$RequireHugo
    Expect-Failure "missing-transcript" "missing asset 'media/moving-mark-transcript.txt'" { param($f) Remove-Item -LiteralPath (Join-Path $f "authoring/examples/instructional/media/moving-mark-transcript.txt") }
    Expect-Failure "missing-license" "missing license" { param($f) $p=Join-Path $f "authoring/examples/instructional/assets.yaml"; $s=Get-Content -Raw -LiteralPath $p; $s=[regex]::Replace($s,'(?m)^    license: .*(?:\r?\n)?','',1); Set-Content -NoNewline -LiteralPath $p -Value $s }
    Expect-Failure "wikilink" "Obsidian wikilinks are not portable" { param($f) Add-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/index.md") -Value "`n[[non-portable-link]]" }
    Expect-Failure "body-only-frontmatter" "missing front matter key 'objectives'" { param($f) $p=Join-Path $f "authoring/examples/instructional/index.md"; $s=Get-Content -Raw -LiteralPath $p; $s=$s -replace '(?m)^objectives:\r?\n(?:  - .*\r?\n){3}', ''; Set-Content -NoNewline -LiteralPath $p -Value ($s+"`nobjectives:`n") }
    Expect-Failure "source-not-cited" "is not cited in prose" { param($f) $p=Join-Path $f "authoring/examples/instructional/index.md"; (Get-Content -Raw -LiteralPath $p).Replace('[openFrameworks project guide](https://openframeworks.cc/learning/01_basics/create_a_new_project/)','openFrameworks project guide') | Set-Content -NoNewline -LiteralPath $p }
    Expect-Failure "source-not-recorded" "has no source record" { param($f) Add-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/index.md") -Value "`n[Unrecorded](https://example.invalid/unrecorded)" }
    Expect-Failure "external-image" "has no asset record" { param($f) Add-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/index.md") -Value "`n![External](https://example.invalid/image.svg)`n`n*External caption.*" }
    Expect-Failure "reference-image" "reference-style images are not supported" { param($f) Add-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/index.md") -Value "`n![Unsupported][preview]`n`n[preview]: media/moving-mark-still.svg" }
    Expect-Failure "duplicate-id" "duplicate record id" { param($f) $p=Join-Path $f "authoring/examples/instructional/assets.yaml"; (Get-Content -Raw -LiteralPath $p).Replace('id: moving-mark-motion','id: moving-mark-still') | Set-Content -NoNewline -LiteralPath $p }
    Expect-Failure "duplicate-field" "duplicate field license" { param($f) $p=Join-Path $f "authoring/examples/instructional/assets.yaml"; $s=Get-Content -Raw -LiteralPath $p; $needle='    license: CC-BY-4.0'; $at=$s.IndexOf($needle); $s=$s.Insert($at+$needle.Length,"`n$needle"); Set-Content -NoNewline -LiteralPath $p -Value $s }
    Expect-Failure "unreferenced-companion" "is not referenced from index.md" { param($f) $p=Join-Path $f "authoring/examples/instructional/index.md"; (Get-Content -LiteralPath $p | Where-Object {$_ -notmatch '\[Motion transcript\]'}) | Set-Content -LiteralPath $p }
    Expect-Failure "unrecorded-media" "every media file must have exactly one asset record" { param($f) Set-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/media/extra.txt") -Value extra }
    Expect-Failure "drive-link" "local links must be relative POSIX paths" { param($f) Add-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/index.md") -Value "`n[Drive](C:/outside.txt)" }
    Expect-Failure "unc-link" "local links must be relative POSIX paths" { param($f) Add-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/index.md") -Value "`n[UNC](//server/share/file.txt)" }
    Expect-Failure "traversal-escape" "escapes the repository root" { param($f) Set-Content -LiteralPath (Join-Path $f "../escaped.txt") -Value outside; Add-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/index.md") -Value "`n[Escape](../../../../escaped.txt)" }
    Expect-Failure "malformed-header" "expected header sources:" { param($f) $p=Join-Path $f "authoring/examples/instructional/sources.yaml"; $s=Get-Content -Raw -LiteralPath $p; Set-Content -NoNewline -LiteralPath $p -Value ($s-replace'^sources:','sourcez:') }
    Expect-Failure "unexpected-field" "unexpected field surprise" { param($f) $p=Join-Path $f "authoring/examples/instructional/sources.yaml"; $s=Get-Content -Raw -LiteralPath $p; $replacement="    surprise: nope`n    note: "; Set-Content -NoNewline -LiteralPath $p -Value ($s-replace'(?m)^    note: ',$replacement) }
    Expect-Failure "unexpected-record-line" "unexpected nonblank line" { param($f) Add-Content -LiteralPath (Join-Path $f "authoring/examples/instructional/sources.yaml") -Value "this is not a record" }
    Expect-Failure "duplicate-source-url" "duplicate source URL" { param($f) $p=Join-Path $f "authoring/examples/instructional/sources.yaml"; Add-Content -LiteralPath $p -Value "  - id: duplicate-url`n    title: Duplicate URL`n    creator: Course authors`n    url: https://OPENFRAMEWORKS.CC/learning/01_basics/create_a_new_project/`n    accessed: 2026-08-18`n    note: This normalized duplicate must be rejected." }
    Expect-Failure "frontmatter-only-citation" "is not cited in prose" { param($f) $p=Join-Path $f "authoring/examples/instructional/index.md"; $s=(Get-Content -Raw -LiteralPath $p).Replace('[openFrameworks project guide](https://openframeworks.cc/learning/01_basics/create_a_new_project/)','openFrameworks project guide'); $s=$s-replace'^---\r?\n',"---`ncitation_probe: https://openframeworks.cc/learning/01_basics/create_a_new_project/`n"; Set-Content -NoNewline -LiteralPath $p -Value $s }
    Expect-Failure "fenced-only-citation" "is not cited in prose" { param($f) $p=Join-Path $f "authoring/examples/instructional/index.md"; $s=(Get-Content -Raw -LiteralPath $p).Replace('[openFrameworks project guide](https://openframeworks.cc/learning/01_basics/create_a_new_project/)','openFrameworks project guide'); $fence=@'

```markdown
[Not prose](https://openframeworks.cc/learning/01_basics/create_a_new_project/)
```
'@; Set-Content -NoNewline -LiteralPath $p -Value ($s+$fence) }
    Expect-Failure "route-missing-interlude" "must list all 19 bundles" { param($f) $p=Join-Path $f "docs/pilot/routes.tsv"; @(Get-Content -LiteralPath $p | Where-Object {$_-notmatch"`t00-first-cpp-test-interlude`t"}) | Set-Content -LiteralPath $p }
    Expect-Failure "route-unknown-lesson" "unknown lesson slug missing-lesson" { param($f) $p=Join-Path $f "docs/pilot/routes.tsv"; $s=Get-Content -Raw -LiteralPath $p; Set-Content -NoNewline -LiteralPath $p -Value ([regex]::Replace($s,'00-cross-platform-setup-and-first-frame','missing-lesson',1)) }
    Expect-Failure "route-extra-field" "must have six tab-delimited fields" { param($f) $p=Join-Path $f "docs/pilot/routes.tsv"; $lines=@(Get-Content -LiteralPath $p); $lines[1]+="`textra"; $lines|Set-Content -LiteralPath $p }
    Expect-Failure "route-invalid-sequence" "sequence must be an ASCII decimal integer" { param($f) $p=Join-Path $f "docs/pilot/routes.tsv"; $s=Get-Content -Raw -LiteralPath $p; Set-Content -NoNewline -LiteralPath $p -Value ([regex]::Replace($s,"`t1`t","`tone`t",1)) }
    Expect-Failure "route-reordered-rows" "sequence must be contiguous from 1" { param($f) $p=Join-Path $f "docs/pilot/routes.tsv"; $lines=@(Get-Content -LiteralPath $p); $first=$lines[1]; $lines[1]=$lines[2]; $lines[2]=$first; $lines|Set-Content -LiteralPath $p }
    Expect-Failure "phase-missing-practice" "phase structure requires exactly one Lesson, Practice, and Exercise heading" { param($f) $p=Join-Path $f "authoring/sections/00-cross-platform-setup/index.md"; @(Get-Content -LiteralPath $p|Where-Object{$_-ne'## Practice'})|Set-Content -LiteralPath $p }
    Expect-Failure "phase-order" "phases must appear in Lesson, Practice, Exercise order" { param($f) $p=Join-Path $f "authoring/sections/00-cross-platform-setup/index.md"; $s=(Get-Content -Raw -LiteralPath $p).Replace('## Lesson','## TEMP').Replace('## Practice','## Lesson').Replace('## TEMP','## Practice'); Set-Content -NoNewline -LiteralPath $p -Value $s }
    Expect-Failure "tests-before-exercise" "section unit-test commands belong in Exercise, not Lesson or Practice" { param($f) $p=Join-Path $f "authoring/sections/00-cross-platform-setup/index.md"; $s=(Get-Content -Raw -LiteralPath $p).Replace('## Lesson',"## Lesson`n`ntests/run-section-00-tests.sh"); Set-Content -NoNewline -LiteralPath $p -Value $s }
    # Symlink escape is POSIX-only because Windows runner policy may deny link creation.
    $positive = if ($RequireHugo) { "positive Hugo fixture/publication builds" } else { "positive structural contract" }
    Write-Host "authoring PowerShell checker tests: $positive and 29 negative contracts passed"
} finally {
    if (Test-Path -LiteralPath $Work) { Remove-Item -LiteralPath $Work -Recurse -Force }
}
