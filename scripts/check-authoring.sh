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

bundle_number=0
find "$root/authoring/templates" "$root/authoring/examples" -name index.md -type f | sort > "$work/bundles"
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

find "$root/authoring" -name '*.md' -type f | sort > "$work/markdown"
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
  cat > "$site/hugo.toml" <<'EOF'
baseURL = "https://example.invalid/"
title = "Authoring smoke check"
disableKinds = ["taxonomy", "term", "rss", "sitemap", "robotsTXT", "404"]
EOF
  printf '%s\n' '<!doctype html><html><body><main>{{ .Content }}</main></body></html>' > "$site/layouts/_default/single.html"
  printf '%s\n' '<!doctype html><html><body><main>{{ range .Pages }}<a href="{{ .RelPermalink }}">{{ .Title }}</a>{{ end }}</main></body></html>' > "$site/layouts/_default/list.html"
  hugo --quiet --buildDrafts --source "$site" --destination "$site/public" || fail 'isolated Hugo smoke build failed'
  for page in portable-moving-mark-example stable-section-slug stable-project-slug; do [ -f "$site/public/course/$page/index.html" ] || fail "Hugo did not emit fixture $page"; done
  for spec in 'portable-moving-mark-example:moving-mark.svg moving-mark-still.svg moving-mark-transcript.txt' 'stable-section-slug:preview-motion.svg preview-still.svg preview-motion-transcript.txt' 'stable-project-slug:process-still.svg'; do
    page=${spec%%:*}; names=${spec#*:}; rendered="$site/public/course/$page/index.html"
    for name in $names; do grep -Fq "$name" "$rendered" || fail "Hugo output omitted reference to $name"; [ -f "$site/public/course/$page/media/$name" ] || fail "Hugo did not publish $name"; done
  done
  printf '%s\n' 'authoring check: contracts and Hugo fixture builds passed'
else
  printf '%s\n' 'authoring check: contracts passed (Hugo not installed; smoke build skipped)'
fi
