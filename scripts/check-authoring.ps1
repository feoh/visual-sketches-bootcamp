[CmdletBinding()]
param([switch]$RequireHugo)
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path -LiteralPath (Split-Path -Parent $PSScriptRoot)).Path
$Authoring = Join-Path $Root "authoring"
function Fail([string]$Message) { throw "authoring check: $Message" }

function Read-FrontMatter([string]$Path) {
    $lines = @(Get-Content -LiteralPath $Path)
    if ($lines.Count -eq 0 -or $lines[0] -ne "---") { Fail "$Path`: YAML front matter must begin on line 1" }
    $values = @{}; $lists = @{}; $section = ""; $closed = $false
    for ($i = 1; $i -lt $lines.Count; $i++) {
        $line = $lines[$i]
        if ($line -eq "---") { $closed = $true; break }
        if ($line -match '^([A-Za-z_]+):\s*(.*)$') { $section=$Matches[1]; if ($values.ContainsKey($section)) { Fail "$Path`: duplicate front matter key '$section'" }; $values[$section]=$Matches[2]; continue }
        if ($section -ne "" -and $line -match '^  - \S') { if (-not $lists.ContainsKey($section)){$lists[$section]=0}; $lists[$section]++ }
    }
    if (-not $closed) { Fail "$Path`: front matter is not closed" }
    foreach($key in @("title","slug","weight","draft","course_kind","objectives","prerequisites","source_records","asset_records")){if(-not $values.ContainsKey($key)){Fail "$Path`: missing front matter key '$key'"}}
    if($values.course_kind -notin @("instructional","synthesis")){Fail "$Path`: course_kind must be instructional or synthesis"}
    if($values.draft -notin @("true","false")){Fail "$Path`: draft must be true or false"}
    if($values.weight -notmatch '^\d+$'){Fail "$Path`: weight must be a non-negative integer"}
    foreach($key in @("objectives","prerequisites")){if(-not $lists.ContainsKey($key)-or $lists[$key]-lt 1){Fail "$Path`: objectives and prerequisites must each contain at least one item"}}
    return $values
}

function Read-Records([string]$Path,[ValidateSet("source","asset")][string]$Type) {
    $records=[System.Collections.Generic.List[hashtable]]::new(); $current=$null
    $lines=@(Get-Content -LiteralPath $Path)
    $expectedHeader=if($Type-eq"source"){"sources:"}else{"assets:"}
    if($lines.Count-eq0-or$lines[0]-ne$expectedHeader){Fail "$Path`: expected header $expectedHeader"}
    $known=if($Type-eq"source"){@("id","title","creator","url","accessed","note")}else{@("id","path","url","kind","creator","source","license","alt","caption","poster","transcript","reduced_motion")}
    for($lineNumber=1;$lineNumber-lt$lines.Count;$lineNumber++){
        $line=$lines[$lineNumber]
        if([string]::IsNullOrWhiteSpace($line)-or$line-match'^\s*#'){continue}
        if($line -match '^  - id: (\S.*)$'){
            if($null-ne$current){$records.Add($current)}
            $current=@{id=$Matches[1]}
        } elseif($null-ne$current -and $line -match '^    ([a-z_]+): (\S.*)$'){
            $key=$Matches[1]; if($key-notin$known){Fail "$Path`: record '$($current.id)' has unexpected field $key"}; if($current.ContainsKey($key)){Fail "$Path`: record '$($current.id)' has duplicate field $key"}; $current[$key]=$Matches[2]
        } elseif($null-ne$current -and $line -match '^    [a-z_]+:\s*$'){Fail "$Path`: record '$($current.id)' has an empty field"}
        elseif($line -match '^  - '){Fail "$Path`: record is missing id"}
        else{Fail "$Path`: unexpected nonblank line $($lineNumber+1)"}
    }
    if($null-ne$current){$records.Add($current)}
    if($records.Count-eq 0){Fail "$Path`: no records"}
    $ids=@{}; $urls=@{}; foreach($record in $records){
        if($ids.ContainsKey($record.id)){Fail "$Path`: duplicate record id $($record.id)"}; $ids[$record.id]=$true
        $required=if($Type-eq"source"){@("id","title","creator","url","accessed","note")}else{@("id","kind","creator","source","license","alt","caption")}
        foreach($key in $required){if(-not$record.ContainsKey($key)-or[string]::IsNullOrWhiteSpace($record[$key])){Fail "$Path`: record '$($record.id)' missing $key"}}
        if($Type-eq"source"){
            if($record.url-notmatch'^https?://'){Fail "$Path`: source URL must use http or https"}
            try{$normalizedUrl=([Uri]$record.url).AbsoluteUri}catch{Fail "$Path`: source URL is malformed"}
            if($urls.ContainsKey($normalizedUrl)){Fail "$Path`: duplicate source URL"}; $urls[$normalizedUrl]=$true
            if($record.accessed-notmatch'^\d{4}-\d{2}-\d{2}$'){Fail "$Path`: accessed must be YYYY-MM-DD"}
        } else {
            $locations=@("path","url")|Where-Object{$record.ContainsKey($_)-and-not[string]::IsNullOrWhiteSpace($record[$_])}
            if($locations.Count-ne1){Fail "$Path`: asset '$($record.id)' must declare exactly one of path or url"}
            if($record.kind-notin@("image","animation","video","audio","transcript")){Fail "$Path`: unsupported asset kind '$($record.kind)'"}
            if($record.kind-in@("animation","video")){foreach($key in @("poster","transcript","reduced_motion")){if(-not$record.ContainsKey($key)-or[string]::IsNullOrWhiteSpace($record[$key])){Fail "$Path`: motion asset '$($record.id)' missing $key"}}}
        }
    }
    return @($records)
}

function Get-NormalMarkdownLines([string]$Path) {
    $lines=@(Get-Content -LiteralPath $Path); $result=[System.Collections.Generic.List[string]]::new()
    $front=$lines.Count-gt0-and$lines[0]-eq'---'; $fence=$false; $comment=$false
    for($i=0;$i-lt$lines.Count;$i++){
        $line=$lines[$i]
        if($front){if($i-gt0-and$line-eq'---'){$front=$false};continue}
        if($line-match'^\s{0,3}(?:```|~~~)'){$fence=-not$fence;continue};if($fence){continue}
        while($true){
            if($comment){$end=$line.IndexOf('-->');if($end-lt0){$line='';break};$line=$line.Substring($end+3);$comment=$false;continue}
            $start=$line.IndexOf('<!--');if($start-lt0){break};$before=$line.Substring(0,$start);$rest=$line.Substring($start+4);$end=$rest.IndexOf('-->')
            if($end-ge0){$line=$before+$rest.Substring($end+3);continue};$line=$before;$comment=$true;break
        }
        $result.Add($line)
    }
    return @($result)
}

function Assert-ContainedLocalLink([string]$File,[string]$Destination){
    if($Destination-match'^(?:[A-Za-z]:|[/\\]{1,2}|file:)' -or $Destination.Contains('\')){Fail "$File`: local links must be relative POSIX paths"}
    $target=($Destination-split'#',2)[0].Replace('/',[IO.Path]::DirectorySeparatorChar)
    $candidate=Join-Path (Split-Path -Parent $File) $target
    if(-not(Test-Path -LiteralPath $candidate)){Fail "$File`: local link target '$Destination' does not exist"}
    $resolved=(Resolve-Path -LiteralPath $candidate).Path
    $prefix=$Root.TrimEnd([IO.Path]::DirectorySeparatorChar)+[IO.Path]::DirectorySeparatorChar
    if($resolved-ne$Root-and-not$resolved.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){Fail "$File`: local link '$Destination' escapes the repository root"}
}

$bundles=@(Get-ChildItem -LiteralPath (Join-Path $Authoring "templates"),(Join-Path $Authoring "examples"),(Join-Path $Authoring "sections") -Filter index.md -File -Recurse)
if($bundles.Count-eq0){Fail "no leaf-bundle index.md files found"}
$linkPattern=[regex]'\]\(([^)]+)\)'
foreach($bundle in $bundles){
    $front=Read-FrontMatter $bundle.FullName; $dir=$bundle.DirectoryName
    foreach($key in @("source_records","asset_records")){
        $rel=$front[$key]; if([string]::IsNullOrWhiteSpace($rel)-or$rel-match'^(?:[A-Za-z]:|[/\\]{1,2})'-or$rel.Contains('\')-or$rel.Contains('..')){Fail "$($bundle.FullName)`: record path must be a local relative POSIX path"}
        if(-not(Test-Path -LiteralPath (Join-Path $dir $rel) -PathType Leaf)){Fail "$($bundle.FullName)`: missing record file '$rel'"}
    }
    $sources=@(Read-Records (Join-Path $dir $front.source_records) source)
    $assets=@(Read-Records (Join-Path $dir $front.asset_records) asset)
    $sourceUrls=@($sources|ForEach-Object{$_.url}); $assetUrls=@($assets|Where-Object{$_.ContainsKey('url')}|ForEach-Object{$_.url})
    if(@($sourceUrls|Group-Object|Where-Object Count-gt1).Count-gt0){Fail "$dir`: duplicate source URL"}
    $paths=@($assets|Where-Object{$_.ContainsKey('path')}|ForEach-Object{$_.path})
    if(@($paths|Group-Object|Where-Object Count-gt1).Count-gt0){Fail "$dir`: duplicate asset path"}
    if(@($assetUrls|Group-Object|Where-Object Count-gt1).Count-gt0){Fail "$dir`: duplicate asset url"}
    $byId=@{}; foreach($asset in $assets){$byId[$asset.id]=$asset}
    foreach($asset in $assets){
        if($asset.ContainsKey('path')){
            if($asset.path-notmatch'^media/'-or$asset.path.Contains('\')-or$asset.path.Contains('..')){Fail "$dir`: asset path '$($asset.path)' must be a safe POSIX path under media/"}
            if(-not(Test-Path -LiteralPath (Join-Path $dir ($asset.path.Replace('/',[IO.Path]::DirectorySeparatorChar))) -PathType Leaf)){Fail "$dir`: missing asset '$($asset.path)'"}
        } elseif($asset.url-notmatch'^https?://'){Fail "$dir`: external asset URL must use http or https"}
        if($asset.kind-in@("animation","video")){
            foreach($spec in @(@("poster","image"),@("transcript","transcript"),@("reduced_motion","image"))){$field=$spec[0];$expected=$spec[1];$id=$asset[$field];if(-not$byId.ContainsKey($id)-or$byId[$id].kind-ne$expected){Fail "$dir`: motion $field '$id' must identify one $expected asset"}}
        }
    }
    $inventory=@(); $media=Join-Path $dir 'media'; if(Test-Path -LiteralPath $media){$inventory=@(Get-ChildItem -LiteralPath $media -File -Recurse|ForEach-Object{[IO.Path]::GetRelativePath($dir,$_.FullName).Replace('\','/')}|Sort-Object)}
    $recorded=@($paths|Sort-Object); if(($inventory-join"`n")-ne($recorded-join"`n")){Fail "$dir`: every media file must have exactly one asset record and no asset path may be stale"}

    $lines=@(Get-NormalMarkdownLines $bundle.FullName); $text=$lines-join"`n"
    if($text-match'!\[[^]]*\]\[[^]]*\]'){Fail "$($bundle.FullName)`: reference-style images are not supported"}
    for($i=0;$i-lt$lines.Count;$i++){
        if($lines[$i]-match'!\[[^]]+\]\([^)]+\)'){$j=$i+1;while($j-lt$lines.Count-and[string]::IsNullOrWhiteSpace($lines[$j])){$j++};if($j-ge$lines.Count-or$lines[$j]-notmatch'^\*[^*].*\*$'){Fail "$($bundle.FullName)`: every image requires a visible italic caption on the next nonblank line"}}
    }
    $prose=@();$images=@()
    foreach($line in $lines){$matches=@($linkPattern.Matches($line));if($matches.Count-gt1){Fail "$($bundle.FullName)`: keep one Markdown link or image per source line"};foreach($match in $matches){$dest=($match.Groups[1].Value-replace'\s+"[^"]*"$','').Trim('<','>');if($dest-match'^https?://'){if($line.Contains('![')){$images+=$dest}else{$prose+=$dest}}}}
    foreach($url in $sourceUrls){if($url-notin$prose){Fail "$dir`: source URL '$url' is not cited in prose"}}
    foreach($url in $prose){if($url-notin$sourceUrls){Fail "$($bundle.FullName)`: external prose link '$url' has no source record"}}
    foreach($url in $images){if($url-notin$assetUrls){Fail "$($bundle.FullName)`: external image '$url' has no asset record"}}
    foreach($url in $assetUrls){if($url-notin$images){Fail "$dir`: external asset '$url' is not referenced as an image"}}
    foreach($path in $paths){if($text-notmatch[regex]::Escape("]($path")){Fail "$dir`: local asset '$path' is not referenced from index.md"}}
}

foreach($file in Get-ChildItem -LiteralPath $Authoring,(Join-Path $Root 'exercises') -Filter '*.md' -File -Recurse){
    $lines=@(Get-NormalMarkdownLines $file.FullName);$text=$lines-join"`n"
    if($text.Contains(']]')){Fail "$($file.FullName)`: Obsidian wikilinks are not portable"}
    if($text-match'\{\{[<%]'){Fail "$($file.FullName)`: Hugo shortcodes are not allowed"}
    if($text.Contains('![](')){Fail "$($file.FullName)`: Markdown images require descriptive alt text"}
    if($text-match'!\[[^]]*\]\[[^]]*\]'){Fail "$($file.FullName)`: reference-style images are not supported"}
    foreach($line in $lines){$matches=@($linkPattern.Matches($line));if($matches.Count-gt1){Fail "$($file.FullName)`: keep one Markdown link or image per source line"};foreach($match in $matches){$dest=($match.Groups[1].Value-replace'\s+"[^"]*"$','').Trim('<','>');if($dest-eq''-or$dest-match'^(?:#|https?://|mailto:)'){continue};Assert-ContainedLocalLink $file.FullName $dest}}
}

$hugo=Get-Command hugo -ErrorAction SilentlyContinue
if($null-eq$hugo){if($RequireHugo){Fail "Hugo is required but was not found on PATH"};Write-Host "authoring check: contracts passed (Hugo not installed; smoke build skipped)";exit 0}
$temp=Join-Path ([IO.Path]::GetTempPath()) ("authoring-check-"+[guid]::NewGuid().ToString('N'))
try{
    $site=Join-Path $temp 'site';$content=Join-Path $site 'content/course';$layouts=Join-Path $site 'layouts/_default';New-Item -ItemType Directory -Force -Path $content, $layouts |Out-Null
    Copy-Item -Recurse -LiteralPath (Join-Path $Authoring 'examples/instructional') -Destination (Join-Path $content 'example')
    Copy-Item -Recurse -LiteralPath (Join-Path $Authoring 'templates/instructional') -Destination (Join-Path $content 'instructional-template')
    Copy-Item -Recurse -LiteralPath (Join-Path $Authoring 'templates/synthesis') -Destination (Join-Path $content 'synthesis-template')
    Copy-Item -Recurse -LiteralPath (Join-Path $Authoring 'sections/00-cross-platform-setup') -Destination (Join-Path $content 'section-00')
    Copy-Item -Recurse -LiteralPath (Join-Path $Authoring 'sections/00-first-cpp-test-interlude') -Destination (Join-Path $content 'first-cpp-test-interlude')
    "baseURL = `"https://example.invalid/`"`ntitle = `"Authoring smoke check`"`ndisableKinds = [`"taxonomy`", `"term`", `"rss`", `"sitemap`", `"robotsTXT`", `"404`"]"|Set-Content -LiteralPath (Join-Path $site 'hugo.toml') -Encoding utf8
    '<!doctype html><html><body><main>{{ .Content }}</main></body></html>'|Set-Content -LiteralPath (Join-Path $layouts 'single.html') -Encoding utf8
    '<!doctype html><html><body><main>{{ range .Pages }}<a href="{{ .RelPermalink }}">{{ .Title }}</a>{{ end }}</main></body></html>'|Set-Content -LiteralPath (Join-Path $layouts 'list.html') -Encoding utf8
    &$hugo.Source --quiet --buildDrafts --source $site --destination (Join-Path $site 'public');if($LASTEXITCODE-ne0){Fail 'isolated Hugo smoke build failed'}
    foreach($page in @('portable-moving-mark-example','stable-section-slug','stable-project-slug','00-cross-platform-setup-and-first-frame','00-first-cpp-test-interlude')){if(-not(Test-Path -LiteralPath (Join-Path $site "public/course/$page/index.html") -PathType Leaf)){Fail "Hugo did not emit fixture $page"}}
    $published = @{
        'portable-moving-mark-example' = @('moving-mark.svg','moving-mark-still.svg','moving-mark-transcript.txt')
        'stable-section-slug' = @('preview-motion.svg','preview-still.svg','preview-motion-transcript.txt')
        'stable-project-slug' = @('process-still.svg')
        '00-cross-platform-setup-and-first-frame' = @('five-primitive-preview.svg')
        '00-first-cpp-test-interlude' = @('test-flow.svg')
    }
    foreach ($page in $published.Keys) {
        $rendered = Get-Content -Raw -LiteralPath (Join-Path $site "public/course/$page/index.html")
        foreach ($name in $published[$page]) {
            if (-not $rendered.Contains($name)) { Fail "Hugo output omitted reference to $name" }
            if (-not (Test-Path -LiteralPath (Join-Path $site "public/course/$page/media/$name") -PathType Leaf)) { Fail "Hugo did not publish $name" }
        }
    }
    $publicationConfig = Join-Path $Root 'site/hugo.toml'
    if (Test-Path -LiteralPath $publicationConfig -PathType Leaf) {
        $publication = Join-Path $temp 'publication'
        &$hugo.Source --quiet --source $Root --config site/hugo.toml --destination $publication --cleanDestinationDir --panicOnWarning --printPathWarnings
        if ($LASTEXITCODE -ne 0) { Fail 'checked publication build failed' }
        foreach ($required in @('index.html','course/index.html','css/course.css')) {
            if (-not (Test-Path -LiteralPath (Join-Path $publication $required) -PathType Leaf)) { Fail "publication did not emit $required" }
        }
        $lessons = @(Get-ChildItem -LiteralPath (Join-Path $Authoring 'sections') -Filter index.md -File -Recurse | Where-Object { (Read-FrontMatter $_.FullName).draft -eq 'false' })
        foreach ($lesson in $lessons) {
            $slug = (Read-FrontMatter $lesson.FullName).slug
            $page = Join-Path $publication "course/$slug/index.html"
            if (-not (Test-Path -LiteralPath $page -PathType Leaf)) { Fail "publication did not emit lesson $slug" }
            $rendered = Get-Content -Raw -LiteralPath $page
            if (-not $rendered.Contains('class="lesson"')) { Fail "publication lesson $slug did not use the accessible lesson layout" }
            if (([regex]::Matches($rendered, '<h1')).Count -ne 1) { Fail "publication lesson $slug must contain exactly one h1" }
            if ($rendered.Contains('href="../../../')) { Fail "publication retained an unresolved repository link in $slug" }
            if ($rendered.Contains('href=""')) { Fail "publication emitted an empty link in $slug" }
            $home = Get-Content -Raw -LiteralPath (Join-Path $publication 'index.html')
            if (-not $home.Contains("href=`"/visual-sketches-bootcamp/course/$slug/`"")) { Fail "publication home omitted lesson $slug" }
        }
        $duplicateWeights = @($lessons | ForEach-Object { (Read-FrontMatter $_.FullName).weight } | Group-Object | Where-Object Count -gt 1)
        if ($duplicateWeights.Count -gt 0) { Fail 'non-draft lesson weights must be unique' }
        $traveler = Get-Content -Raw -LiteralPath (Join-Path $publication 'course/01-a-mark-that-moves/index.html')
        if (-not $traveler.Contains('github.com/feoh/visual_sketches_bootcamp/blob/main/exercises/')) { Fail 'publication did not rewrite repository file links' }
        if (-not (Test-Path -LiteralPath (Join-Path $publication 'course/01-a-mark-that-moves/media/traveler-time-preview.svg') -PathType Leaf)) { Fail 'publication omitted representative bundle media' }
        $section16 = Get-Content -Raw -LiteralPath (Join-Path $publication 'course/16-three-cumulative-sketch-studies/index.html')
        $section17 = Get-Content -Raw -LiteralPath (Join-Path $publication 'course/17-original-visual-instrument-capstone/index.html')
        if (-not $section16.Contains('github.com/feoh/visual_sketches_bootcamp/blob/main/authoring/sections/16-three-sketch-studies/templates/model-test-contract.md')) { Fail 'publication did not rewrite section 16 Markdown resource links' }
        if (-not $section16.Contains('github.com/feoh/visual_sketches_bootcamp/blob/main/authoring/sections/16-three-sketch-studies/fixtures/README.md')) { Fail 'publication did not expose section 16 fixture provenance' }
        if (-not $section17.Contains('github.com/feoh/visual_sketches_bootcamp/blob/main/authoring/sections/17-original-visual-instrument/fixtures/README.md')) { Fail 'publication did not expose section 17 fixture provenance' }
        if (-not $section16.Contains('CC0-1.0')) { Fail 'publication omitted section 16 fixture license notice' }
        if (-not $section17.Contains('CC0-1.0')) { Fail 'publication omitted section 17 fixture license notice' }
        $courseIndex = Get-Content -Raw -LiteralPath (Join-Path $publication 'course/index.html')
        $firstLesson = [regex]::Match($courseIndex, '<li><a href="([^"]+)"').Groups[1].Value
        if ($firstLesson -ne '/visual-sketches-bootcamp/course/00-cross-platform-setup-and-first-frame/') { Fail 'publication course contents do not begin with section 00 setup' }
        $setup = Get-Content -Raw -LiteralPath (Join-Path $publication 'course/00-cross-platform-setup-and-first-frame/index.html')
        if ($setup.Contains('<span>Previous</span><a')) { Fail 'section 00 setup unexpectedly has a previous lesson' }
        if (-not $setup.Contains('<span>Next</span><a href="/visual-sketches-bootcamp/course/01-a-mark-that-moves/">')) { Fail 'section 00 setup does not lead to section 01' }
        if ($section17.Contains('<span>Next</span><a')) { Fail 'section 17 unexpectedly has a next lesson' }
        $pagination = Get-Content -Raw -LiteralPath (Join-Path $publication 'course/14-images-and-type-as-geometry/index.html')
        if (-not $pagination.Contains('<span>Previous</span><a href="/visual-sketches-bootcamp/course/13-time-as-a-drawable-axis/">')) { Fail 'publication Previous navigation does not follow increasing course weight' }
        if (-not $pagination.Contains('<span>Next</span><a href="/visual-sketches-bootcamp/course/15-embodied-audio-input/">')) { Fail 'publication Next navigation does not follow increasing course weight' }
        Write-Host "authoring check: contracts, Hugo fixtures, and publication build passed ($($lessons.Count) lessons)"
    } else {
        Write-Host 'authoring check: contracts and Hugo fixture builds passed (publication layer not present)'
    }
}finally{if(Test-Path -LiteralPath $temp){Remove-Item -LiteralPath $temp -Recurse -Force}}
