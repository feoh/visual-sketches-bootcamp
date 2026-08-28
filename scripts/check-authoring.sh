#!/bin/sh
set -eu

usage() { printf '%s\n' 'usage: scripts/check-authoring.sh [--require-hugo]'; }
require_hugo=0
case "${1-}" in '') ;; --require-hugo) require_hugo=1 ;; -h|--help) usage; exit 0 ;; *) usage >&2; exit 2 ;; esac
[ "$#" -le 1 ] || { usage >&2; exit 2; }

script_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
root=$(CDPATH='' cd -- "$script_dir/.." && pwd -P)
work=$(mktemp -d "${TMPDIR:-/tmp}/authoring-check.XXXXXX")
trap 'rm -rf "$work"' EXIT HUP INT TERM
fail() { printf 'authoring check: %s\n' "$*" >&2; exit 1; }

frontmatter_value() {
  awk -v key="$1" 'NR==1 && $0=="---" {front=1; next} front && $0=="---" {exit} front && index($0,key ":")==1 {sub("^[^:]*:[[:space:]]*",""); print; exit}' "$2"
}

validate_frontmatter() {
  file=$1
  [ "$(sed -n '1p' "$file")" = '---' ] || fail "$file: YAML front matter must begin on line 1"
  awk 'NR==1&&$0=="---"{front=1;next} front&&$0=="---"{closed=1;exit} front&&/^[A-Za-z_]+:/{key=$0;sub(/:.*/,"",key);if(seen[key]++)bad=1} END{exit !(closed&&!bad)}' "$file" || fail "$file: front matter is unclosed or has a duplicate key"
  for key in title slug weight draft course_kind objectives prerequisites source_records asset_records; do
    awk -v key="$key" 'NR==1 && $0=="---" {front=1; next} front && $0=="---" {exit} front && index($0,key ":")==1 {found=1} END {exit !found}' "$file" || fail "$file: missing front matter key '$key'"
  done
  case "$(frontmatter_value course_kind "$file")" in instructional|synthesis) ;; *) fail "$file: course_kind must be instructional or synthesis" ;; esac
  case "$(frontmatter_value draft "$file")" in true|false) ;; *) fail "$file: draft must be true or false" ;; esac
  case "$(frontmatter_value weight "$file")" in ''|*[!0-9]*) fail "$file: weight must be a non-negative integer" ;; esac
  awk 'NR==1&&$0=="---"{front=1;next} front&&$0=="---"{exit} front&&$0=="objectives:"{s="o";next} front&&$0=="prerequisites:"{s="p";next} front&&/^[A-Za-z_]+:/{s=""} front&&s!=""&&/^  - [^[:space:]]/{n[s]++} END{exit !(n["o"]&&n["p"])}' "$file" || fail "$file: objectives and prerequisites must each contain at least one item"
}

# Emit tab-delimited normalized records. This is a deliberately strict YAML
# subset: one exact header and one unquoted scalar per known field.
parse_records() {
  type=$1 file=$2 output=$3
  awk -v type="$type" -v file="$file" '
    function err(message){print file ": " message > "/dev/stderr"; bad=1}
    function known(key){
      if(type=="source") return index(" id title creator url accessed note "," " key " ")>0
      return index(" id path url kind creator source license alt caption poster transcript reduced_motion "," " key " ")>0
    }
    function normalized_url(url,   splitat,scheme,rest,cut,authority,suffix){
      splitat=index(url,"://"); scheme=tolower(substr(url,1,splitat-1)); rest=substr(url,splitat+3)
      cut=match(rest,/[\/?#]/); if(cut){authority=substr(rest,1,cut-1);suffix=substr(rest,cut)}else{authority=rest;suffix="/"}
      authority=tolower(authority)
      if(scheme=="http")sub(/:80$/,"",authority); if(scheme=="https")sub(/:443$/,"",authority)
      return scheme "://" authority suffix
    }
    function finish(   required,i,n,a,loc){
      if(!active)return
      if(seenid[id]++) err("duplicate record id " id)
      if(type=="source") required="id title creator url accessed note"
      else required="id kind creator source license alt caption"
      n=split(required,a," "); for(i=1;i<=n;i++) if(value[a[i]]=="") err("record " id " missing " a[i])
      if(type=="source"){
        if(value["url"] !~ /^https?:\/\//) err("source URL must use http or https")
        if(value["accessed"] !~ /^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]$/) err("accessed must be YYYY-MM-DD")
        print "S\t" id "\t" value["url"] "\t" normalized_url(value["url"])
      } else {
        loc=(value["path"]!="")+(value["url"]!="")
        if(loc!=1) err("asset " id " must declare exactly one of path or url")
        if(value["kind"]!~ /^(image|animation|video|audio|transcript)$/) err("unsupported asset kind " value["kind"])
        if(value["kind"] ~ /^(animation|video)$/){split("poster transcript reduced_motion",a," "); for(i=1;i<=3;i++) if(value[a[i]]=="")err("motion asset " id " missing " a[i])}
        print "A\t" id "\t" value["path"] "\t" value["url"] "\t" value["kind"] "\t" value["poster"] "\t" value["transcript"] "\t" value["reduced_motion"]
      }
      delete value; delete fieldseen; active=0
    }
    NR==1 {expected=(type=="source"?"sources:":"assets:"); if($0!=expected)err("expected header " expected); header=1; next}
    /^[[:space:]]*$/ || /^[[:space:]]*#/ {next}
    /^  - id: [^[:space:]].*$/ {finish(); active=1; id=substr($0,9); value["id"]=id; fieldseen["id"]=1; next}
    active && /^    [a-z_]+: [^[:space:]].*$/ {
      line=$0; sub(/^    /,"",line); key=line; sub(/:.*/,"",key); val=line; sub(/^[^:]+: /,"",val)
      if(!known(key)) err("record " id " has unexpected field " key)
      if(fieldseen[key]++) err("record " id " has duplicate field " key)
      value[key]=val; next
    }
    active && /^    [a-z_]+:[[:space:]]*$/ {err("record " id " has an empty field"); next}
    /^  - / {err("record is missing id"); next}
    {err("unexpected nonblank line " NR)}
    END {finish(); if(bad)exit 1}
  ' "$file" > "$output" || fail "$file: invalid records"
  [ -s "$output" ] || fail "$file: no records"
}

canonical_target() {
  target=$1
  if [ "${AUTHORING_CHECK_NO_REALPATH:-0}" != 1 ] && command -v realpath >/dev/null 2>&1; then realpath "$target"; return; fi
  # cd -P resolves every parent component. Without realpath, conservatively
  # reject a symlink in the final component rather than pretending it is safe.
  [ ! -L "$target" ] || return 1
  parent=$(CDPATH='' cd -P -- "$(dirname -- "$target")" && pwd -P) || return 1
  printf '%s/%s\n' "$parent" "$(basename -- "$target")"
}

normal_markdown() {
  awk '
    NR==1&&$0=="---"{front=1;next}
    front&&$0=="---"{front=0;next}
    front{next}
    /^[[:space:]]{0,3}(```|~~~)/{fence=!fence;next}
    fence{next}
    {
      line=$0
      while(1){
        if(comment){end=index(line,"-->");if(!end){line="";break};line=substr(line,end+3);comment=0;continue}
        start=index(line,"<!--");if(!start)break
        before=substr(line,1,start-1);rest=substr(line,start+4);end=index(rest,"-->")
        if(end){line=before substr(rest,end+3);continue}
        line=before;comment=1;break
      }
      print line
    }
  ' "$1"
}

validate_pilot_contract() {
  routes="$root/docs/pilot/routes.tsv"
  [ -f "$routes" ] || fail 'docs/pilot/routes.tsv is required'
  [ "$(sed -n '1p' "$routes")" = "protocol_version	route_id	sequence	lesson_slug	status	checkpoint_after" ] || fail "$routes: invalid header"
  grep -Fqx '**Protocol version:** 1.2' "$root/docs/pilot-protocol-and-evidence.md" || fail 'pilot protocol version must agree with routes.tsv'

  : > "$work/pilot-lessons"
  find "$root/authoring/sections" -mindepth 2 -maxdepth 2 -name index.md -type f | sort | while IFS= read -r lesson; do
    [ "$(frontmatter_value draft "$lesson")" = false ] || continue
    printf '%s\t%s\n' "$(frontmatter_value weight "$lesson")" "$(frontmatter_value slug "$lesson")"
  done | sort -n | awk -F '\t' '{print $2 "\t" NR}' > "$work/pilot-lessons"
  [ "$(wc -l < "$work/pilot-lessons" | tr -d ' ')" -eq 19 ] || fail 'pilot routes require exactly 19 published lesson bundles'
  cut -f1 "$work/pilot-lessons" | sort | uniq -d | grep . >/dev/null 2>&1 && fail 'published lesson slugs must be unique'

  awk -F '\t' -v file="$routes" '
    NR==1 {next}
    function die(message){print file ": " message > "/dev/stderr"; bad=1}
    {
      if(NF!=6){die("row " NR " must have six tab-delimited fields");next}
      version=$1; route=$2; sequence=$3; slug=$4; status=$5; checkpoint=$6
      if(version!="1.2")die("row " NR " protocol version must be 1.2")
      if(route!~/^(complete-18|core-12|accelerated-8-plus-2)$/)die("row " NR " has unknown route " route)
      if(sequence!~/^[0-9]+$/)die("row " NR " sequence must be an ASCII decimal integer")
      else if(sequence!=++next_sequence[route])die("route " route " sequence must be contiguous from 1")
      if(seen_slug[route SUBSEP slug]++)die("route " route " repeats lesson " slug)
      if(status!~/^(required|optional)$/)die("row " NR " has invalid status " status)
      if(checkpoint!~/^(none|unit-0|unit-2|complete-path)$/)die("row " NR " has invalid checkpoint " checkpoint)
      expected_checkpoint="none"
      if(slug=="00-first-cpp-test-interlude")expected_checkpoint="unit-0"
      else if(slug=="08-gesture-as-geometry")expected_checkpoint="unit-2"
      else if(slug=="17-original-visual-instrument-capstone")expected_checkpoint="complete-path"
      if(checkpoint!=expected_checkpoint)die("lesson " slug " must use checkpoint " expected_checkpoint)
      expected_status="required"
      if(route!="complete-18" && slug~/^(13-time-as-a-drawable-axis|14-images-and-type-as-geometry|15-embodied-audio-input)$/)expected_status="optional"
      if(status!=expected_status)die("lesson " slug " must be " expected_status " on route " route)
      count[route]++
    }
    END {
      split("complete-18 core-12 accelerated-8-plus-2", routes, " ")
      for(i in routes)if(count[routes[i]]!=19)die("route " routes[i] " must list all 19 bundles")
      exit bad
    }
  ' "$routes" || fail "$routes: invalid route contract"

  awk -F '\t' -v file="$routes" '
    NR==FNR {sequence[$1]=$2; next}
    FNR==1 {next}
    !($4 in sequence){print file ": unknown lesson slug " $4 > "/dev/stderr";bad=1;next}
    sequence[$4]!=$3 {print file ": lesson " $4 " is out of published weight order" > "/dev/stderr";bad=1}
    END {exit bad}
  ' "$work/pilot-lessons" "$routes" || fail "$routes: route lessons must resolve in published order"

  for template in README.md progress-log.md lesson-notes.md checkpoints.md revision-log.md pacing-log.tsv; do
    [ -s "$root/docs/pilot/$template" ] || fail "docs/pilot/$template is required and must not be empty"
  done
  grep -Fq 'Help used:' "$root/docs/pilot/lesson-notes.md" || fail 'lesson notes template is missing help level'
  grep -Fq 'Complete path' "$root/docs/pilot/progress-log.md" || fail 'progress log is missing complete-path checkpoint'
  find "$root/authoring/sections" -mindepth 2 -maxdepth 2 -name index.md -type f | sort | while IFS= read -r lesson; do
    [ "$(frontmatter_value course_kind "$lesson")" = instructional ] || continue
    body="$work/phase-$(basename "$(dirname "$lesson")")"
    normal_markdown "$lesson" > "$body"
    [ "$(grep -Fxc '## Lesson' "$body" || true)" -eq 1 ] &&
      [ "$(grep -Fxc '## Practice' "$body" || true)" -eq 1 ] &&
      [ "$(grep -Fxc '## Exercise' "$body" || true)" -eq 1 ] ||
      fail "$lesson: phase structure requires exactly one Lesson, Practice, and Exercise heading"
    lesson_line=$(grep -Fn '## Lesson' "$body" | cut -d: -f1)
    practice_line=$(grep -Fn '## Practice' "$body" | cut -d: -f1)
    exercise_line=$(grep -Fn '## Exercise' "$body" | cut -d: -f1)
    [ "$lesson_line" -lt "$practice_line" ] && [ "$practice_line" -lt "$exercise_line" ] ||
      fail "$lesson: phases must appear in Lesson, Practice, Exercise order"
    awk '/^## Exercise$/{exit}{print}' "$body" | grep -Eq 'run-section-[0-9][0-9]-tests\.(sh|ps1)' &&
      fail "$lesson: section unit-test commands belong in Exercise, not Lesson or Practice"
    grep -Eq '^#{2,3} (Quick visual|Manual)' "$body" || fail "$lesson: instructional section requires a visual-review heading"
    grep -Eq '^#{2,3} If you get stuck$' "$body" || fail "$lesson: instructional section requires a troubleshooting heading"
  done
}

validate_pilot_contract
bundle_number=0
find "$root/authoring/templates" "$root/authoring/examples" "$root/authoring/sections" -name index.md -type f | sort > "$work/bundles"
[ -s "$work/bundles" ] || fail 'no leaf-bundle index.md files found'
while IFS= read -r index; do
  bundle_number=$((bundle_number + 1)); dir=$(dirname -- "$index")
  validate_frontmatter "$index"
  source_rel=$(frontmatter_value source_records "$index"); asset_rel=$(frontmatter_value asset_records "$index")
  for rel in "$source_rel" "$asset_rel"; do
    case "$rel" in ''|/*|[A-Za-z]:*|*\\*|*..*) fail "$index: record path must be a local relative POSIX path" ;; esac
    [ -f "$dir/$rel" ] || fail "$index: missing record file '$rel'"
  done
  sources="$work/sources.$bundle_number"; assets="$work/assets.$bundle_number"
  parse_records source "$dir/$source_rel" "$sources"
  parse_records asset "$dir/$asset_rel" "$assets"
  source_urls="$work/source-urls.$bundle_number"; asset_urls="$work/asset-urls.$bundle_number"
  cut -f3 "$sources" > "$source_urls"
  cut -f4 "$assets" | sed '/^$/d' > "$asset_urls"
  cut -f4 "$sources" | sort | uniq -d | grep . >/dev/null 2>&1 && fail "$dir/$source_rel: duplicate source URL"

  # Validate asset paths, path uniqueness, media inventory, and motion relations.
  cut -f3 "$assets" | sed '/^$/d' | sort | uniq -d | grep . >/dev/null 2>&1 && fail "$dir/$asset_rel: duplicate asset path"
  cut -f4 "$assets" | sed '/^$/d' | sort | uniq -d | grep . >/dev/null 2>&1 && fail "$dir/$asset_rel: duplicate asset url"
  cut -f2,5 "$assets" > "$work/asset-kinds.$bundle_number"
  tab=$(printf '\t')
  while IFS="$tab" read -r marker id path url kind poster transcript reduced; do
    [ "$marker" = A ] || continue
    if [ -n "$path" ]; then
      case "$path" in media/*) ;; *) fail "$dir/$asset_rel: asset path '$path' must be under media/" ;; esac
      case "$path" in *\\*|*..*) fail "$dir/$asset_rel: unsafe asset path '$path'" ;; esac
      [ -f "$dir/$path" ] || fail "$dir/$asset_rel: missing asset '$path'"
    else
      case "$url" in http://*|https://*) ;; *) fail "$dir/$asset_rel: external asset URL must use http or https" ;; esac
    fi
    if [ "$kind" = animation ] || [ "$kind" = video ]; then
      for pair in "poster:$poster:image" "transcript:$transcript:transcript" "reduced_motion:$reduced:image"; do
        field=${pair%%:*}; rest=${pair#*:}; companion=${rest%%:*}; expected=${rest##*:}
        [ -n "$companion" ] || fail "$dir/$asset_rel: motion asset '$id' missing $field"
        actual=$(awk -F '\t' -v id="$companion" '$1==id{print $2}' "$work/asset-kinds.$bundle_number")
        [ "$actual" = "$expected" ] || fail "$dir/$asset_rel: motion $field '$companion' must identify one $expected asset"
      done
    fi
  done < "$assets"
  if [ -d "$dir/media" ]; then
    find "$dir/media" -type f | sed "s|^$dir/||" | sort > "$work/media.$bundle_number"
  else : > "$work/media.$bundle_number"; fi
  cut -f3 "$assets" | sed '/^$/d' | sort > "$work/paths.$bundle_number"
  cmp -s "$work/media.$bundle_number" "$work/paths.$bundle_number" || fail "$dir: every media file must have exactly one asset record and no asset path may be stale"

  # Citation and image checks inspect normal Markdown prose only, excluding YAML
  # front matter, fenced code examples, and HTML comments.
  body="$work/body.$bundle_number"; normal_markdown "$index" > "$body"
  grep -Eq '!\[[^]]*\]\[[^]]*\]' "$body" && fail "$index: reference-style images are not supported"
  awk 'BEGIN{pending=0} pending&&/^[[:space:]]*$/{next} pending{if($0!~/^\*[^*].*\*$/)exit 1;pending=0} /!\[[^]]+\]\([^)]+\)/{pending=1} END{exit pending}' "$body" || fail "$index: every image requires a visible italic caption on the next nonblank line"

  : > "$work/prose.$bundle_number"; : > "$work/images.$bundle_number"
  while IFS= read -r line; do
    count=$(printf '%s\n' "$line" | awk '{print gsub(/\]\(/,"&")}')
    [ "$count" -le 1 ] || fail "$index: keep one Markdown link or image per source line"
    dest=$(printf '%s\n' "$line" | sed -n 's/.*](\([^)]*\)).*/\1/p')
    [ -n "$dest" ] || continue
    dest=${dest%% \"*}; dest=${dest#<}; dest=${dest%>}
    case "$line" in *'!['*) kind=images ;; *) kind=prose ;; esac
    case "$dest" in http://*|https://*) printf '%s\n' "$dest" >> "$work/$kind.$bundle_number" ;; esac
  done < "$body"
  while IFS= read -r url; do grep -Fqx "$url" "$work/prose.$bundle_number" || fail "$dir/$source_rel: source URL '$url' is not cited in prose"; done < "$source_urls"
  while IFS= read -r url; do grep -Fqx "$url" "$source_urls" || fail "$index: external prose link '$url' has no source record"; done < "$work/prose.$bundle_number"
  while IFS= read -r url; do grep -Fqx "$url" "$asset_urls" || fail "$index: external image '$url' has no asset record"; done < "$work/images.$bundle_number"
  while IFS= read -r url; do grep -Fqx "$url" "$work/images.$bundle_number" || fail "$dir/$asset_rel: external asset '$url' is not referenced as an image"; done < "$asset_urls"
  cut -f3 "$assets" | sed '/^$/d' > "$work/asset-paths.$bundle_number"
  while IFS= read -r path; do grep -Fq "]($path" "$body" || fail "$dir/$asset_rel: local asset '$path' is not referenced from index.md"; done < "$work/asset-paths.$bundle_number"
done < "$work/bundles"

find "$root/authoring" "$root/exercises" -name '*.md' -type f | sort > "$work/markdown"
markdown_number=0
while IFS= read -r file; do
  markdown_number=$((markdown_number + 1)); clean="$work/markdown-body.$markdown_number"; normal_markdown "$file" > "$clean"
  grep -Fq ']]' "$clean" && fail "$file: Obsidian wikilinks are not portable"
  grep -Eq '\{\{[<%]' "$clean" && fail "$file: Hugo shortcodes are not allowed"
  grep -Fq '![](' "$clean" && fail "$file: Markdown images require descriptive alt text"
  grep -Eq '!\[[^]]*\]\[[^]]*\]' "$clean" && fail "$file: reference-style images are not supported"
  awk '{line=$0; if(gsub(/\]\(/,"](",line)>1)exit 1}' "$clean" || fail "$file: keep one Markdown link or image per source line"
  sed -n 's/.*](\([^)]*\)).*/\1/p' "$clean" | while IFS= read -r raw; do
    dest=${raw%% \"*}; dest=${dest#<}; dest=${dest%>}
    case "$dest" in ''|'#'*|http://*|https://*|mailto:*) continue ;; esac
    case "$dest" in /*|[A-Za-z]:*|file:*|*\\*) fail "$file: local links must be relative POSIX paths" ;; esac
    target=${dest%%#*}; [ -e "$(dirname -- "$file")/$target" ] || fail "$file: local link target '$dest' does not exist"
    resolved=$(canonical_target "$(dirname -- "$file")/$target") || fail "$file: cannot resolve local link '$dest'"
    case "$resolved" in "$root"|"$root"/*) ;; *) fail "$file: local link '$dest' escapes the repository root" ;; esac
  done
done < "$work/markdown"

run_hugo=0
if command -v hugo >/dev/null 2>&1; then run_hugo=1; elif [ "$require_hugo" -eq 1 ]; then fail 'Hugo is required but was not found on PATH'; fi
if [ "$run_hugo" -eq 1 ]; then
  site="$work/site"; mkdir -p "$site/content/course" "$site/layouts/_default"
  cp -R "$root/authoring/examples/instructional" "$site/content/course/example"
  cp -R "$root/authoring/templates/instructional" "$site/content/course/instructional-template"
  cp -R "$root/authoring/templates/synthesis" "$site/content/course/synthesis-template"
  cp -R "$root/authoring/sections/00-cross-platform-setup" "$site/content/course/section-00"
  cp -R "$root/authoring/sections/00-first-cpp-test-interlude" "$site/content/course/first-cpp-test-interlude"
  cat > "$site/hugo.toml" <<'EOF'
baseURL = "https://example.invalid/"
title = "Authoring smoke check"
disableKinds = ["taxonomy", "term", "rss", "sitemap", "robotsTXT", "404"]
EOF
  printf '%s\n' '<!doctype html><html><body><main>{{ .Content }}</main></body></html>' > "$site/layouts/_default/single.html"
  printf '%s\n' '<!doctype html><html><body><main>{{ range .Pages }}<a href="{{ .RelPermalink }}">{{ .Title }}</a>{{ end }}</main></body></html>' > "$site/layouts/_default/list.html"
  hugo --quiet --buildDrafts --source "$site" --destination "$site/public" || fail 'isolated Hugo smoke build failed'
  for page in portable-moving-mark-example stable-section-slug stable-project-slug 00-cross-platform-setup-and-first-frame 00-first-cpp-test-interlude; do [ -f "$site/public/course/$page/index.html" ] || fail "Hugo did not emit fixture $page"; done
  for spec in 'portable-moving-mark-example:moving-mark.svg moving-mark-still.svg moving-mark-transcript.txt' 'stable-section-slug:preview-motion.svg preview-still.svg preview-motion-transcript.txt' 'stable-project-slug:process-still.svg' '00-cross-platform-setup-and-first-frame:five-primitive-preview.svg' '00-first-cpp-test-interlude:test-flow.svg'; do
    page=${spec%%:*}; names=${spec#*:}; rendered="$site/public/course/$page/index.html"
    for name in $names; do grep -Fq "$name" "$rendered" || fail "Hugo output omitted reference to $name"; [ -f "$site/public/course/$page/media/$name" ] || fail "Hugo did not publish $name"; done
  done
  if [ -f "$root/site/hugo.toml" ]; then
    publication="$work/publication"
    base_url=$(sed -n 's/^baseURL[[:space:]]*=[[:space:]]*"\([^"]*\)".*/\1/p' "$root/site/hugo.toml" | head -n 1)
    [ -n "$base_url" ] || fail 'site/hugo.toml does not define baseURL'
    base_path=${base_url#*://}
    case "$base_path" in
      */*) base_path="/${base_path#*/}" ;;
      *) base_path='/' ;;
    esac
    case "$base_path" in
      */) ;;
      *) base_path="$base_path/" ;;
    esac
    hugo --quiet --source "$root" --config site/hugo.toml \
      --destination "$publication" --cleanDestinationDir \
      --panicOnWarning --printPathWarnings || fail 'checked publication build failed'
    [ -f "$publication/index.html" ] || fail 'publication did not emit its home page'
    [ -f "$publication/course/index.html" ] || fail 'publication did not emit course navigation'
    [ -f "$publication/css/course.css" ] || fail 'publication did not emit its stylesheet'
    grep -Fq "src=\"${base_path}course/12-color-blending-and-trails/media/trail-preview.svg\"" "$publication/index.html" || fail 'publication home omitted its hero preview image'
    grep -Fq 'alt=""' "$publication/index.html" && fail 'publication home emitted an image with empty alt text'
    published_count=0
    find "$root/authoring/sections" -mindepth 2 -maxdepth 2 -name index.md -type f | sort | while IFS= read -r lesson; do
      [ "$(frontmatter_value draft "$lesson")" = false ] || continue
      slug=$(frontmatter_value slug "$lesson")
      page="$publication/course/$slug/index.html"
      [ -f "$page" ] || fail "publication did not emit lesson $slug"
      grep -Fq 'class="lesson"' "$page" || fail "publication lesson $slug did not use the accessible lesson layout"
      [ "$(grep -c '<h1' "$page")" -eq 1 ] || fail "publication lesson $slug must contain exactly one h1"
      if [ "$(frontmatter_value course_kind "$lesson")" = instructional ]; then
        [ "$(grep -c '<h2 ' "$page")" -eq 3 ] || fail "publication instructional lesson $slug must contain exactly three h2 phase headings"
        for phase in lesson practice exercise; do
          grep -Fq "<h2 id=\"$phase\">" "$page" || fail "publication instructional lesson $slug omitted $phase heading"
        done
      fi
      grep -Fq "href=\"${base_path}course/$slug/\"" "$publication/index.html" || fail "publication home omitted lesson $slug"
      grep -Fq 'href="../../../' "$page" && fail "publication retained an unresolved repository link in $slug"
      grep -Fq 'href=""' "$page" && fail "publication emitted an empty link in $slug"
      printf '%s\t%s\n' "$(frontmatter_value weight "$lesson")" "$slug" >> "$work/publication-weights"
      published_count=$((published_count + 1))
      printf '%s\n' "$published_count" > "$work/published-count"
    done
    expected_count=$(find "$root/authoring/sections" -mindepth 2 -maxdepth 2 -name index.md -type f -exec grep -l '^draft: false$' {} + | wc -l | tr -d ' ')
    actual_count=$(cat "$work/published-count" 2>/dev/null || printf 0)
    [ "$actual_count" -eq "$expected_count" ] || fail "publication emitted $actual_count of $expected_count expected lessons"
    cut -f1 "$work/publication-weights" | sort | uniq -d | grep . >/dev/null 2>&1 && fail 'non-draft lesson weights must be unique'
    grep -Fq 'github.com/feoh/visual-sketches-bootcamp/blob/main/exercises/' "$publication/course/01-a-mark-that-moves/index.html" || fail 'publication did not rewrite repository file links'
    [ -f "$publication/course/01-a-mark-that-moves/media/traveler-time-preview.svg" ] || fail 'publication omitted representative bundle media'
    section16="$publication/course/16-three-cumulative-sketch-studies/index.html"
    section17="$publication/course/17-original-visual-instrument-capstone/index.html"
    grep -Fq 'github.com/feoh/visual-sketches-bootcamp/blob/main/authoring/sections/16-three-sketch-studies/templates/model-test-contract.md' "$section16" || fail 'publication did not rewrite section 16 Markdown resource links'
    grep -Fq 'github.com/feoh/visual-sketches-bootcamp/blob/main/authoring/sections/16-three-sketch-studies/fixtures/README.md' "$section16" || fail 'publication did not expose section 16 fixture provenance'
    grep -Fq 'github.com/feoh/visual-sketches-bootcamp/blob/main/authoring/sections/17-original-visual-instrument/fixtures/README.md' "$section17" || fail 'publication did not expose section 17 fixture provenance'
    grep -Fq "href=\"${base_path}course/16-three-cumulative-sketch-studies/#reuse-three-working-starters\"" "$section17" || fail 'publication did not resolve the section 17 sibling lesson link'
    grep -Fq 'CC0-1.0' "$section16" || fail 'publication omitted section 16 fixture license notice'
    grep -Fq 'CC0-1.0' "$section17" || fail 'publication omitted section 17 fixture license notice'
    first_lesson=$(sed -n 's|.*<li><a href="\([^"]*\)".*|\1|p' "$publication/course/index.html" | head -n 1)
    [ "$first_lesson" = "${base_path}course/00-cross-platform-setup-and-first-frame/" ] || fail 'publication course contents do not begin with section 00 setup'
    setup="$publication/course/00-cross-platform-setup-and-first-frame/index.html"
    grep -Fq '<span>Previous</span><a' "$setup" && fail 'section 00 setup unexpectedly has a previous lesson'
    grep -Fq "<span>Next</span><a href=\"${base_path}course/01-a-mark-that-moves/\">" "$setup" || fail 'section 00 setup does not lead to section 01'
    grep -Fq '<span>Next</span><a' "$section17" && fail 'section 17 unexpectedly has a next lesson'
    pagination="$publication/course/14-images-and-type-as-geometry/index.html"
    grep -Fq "<span>Previous</span><a href=\"${base_path}course/13-time-as-a-drawable-axis/\">" "$pagination" || fail 'publication Previous navigation does not follow increasing course weight'
    grep -Fq "<span>Next</span><a href=\"${base_path}course/15-embodied-audio-input/\">" "$pagination" || fail 'publication Next navigation does not follow increasing course weight'
    printf '%s\n' "authoring check: contracts, Hugo fixtures, and publication build passed ($actual_count lessons)"
  else
    printf '%s\n' 'authoring check: contracts and Hugo fixture builds passed (publication layer not present)'
  fi
else
  printf '%s\n' 'authoring check: contracts passed (Hugo not installed; smoke build skipped)'
fi
