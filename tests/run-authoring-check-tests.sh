#!/usr/bin/env bash
set -euo pipefail
script_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
root=$(CDPATH='' cd -- "$script_dir/.." && pwd)
work=$(mktemp -d "${TMPDIR:-/tmp}/authoring-check-tests.XXXXXX")
trap 'rm -rf "$work"' EXIT HUP INT TERM
"$root/scripts/check-authoring.sh" --require-hugo

make_fixture(){
  local fixture=$1 relative destination
  mkdir -p "$fixture/scripts"
  # Copy the current tracked/untracked authoring inputs, but not ignored native
  # metadata or binaries left by local CachyOS/macOS builds. A recursive copy
  # can multiply gigabytes of disposable products across every negative test.
  while IFS= read -r -d '' relative; do
    destination="$fixture/$relative"
    mkdir -p "$(dirname "$destination")"
    cp -P "$root/$relative" "$destination"
  done < <(git -C "$root" ls-files -z --cached --others --exclude-standard -- authoring exercises docs/pilot docs/pilot-protocol-and-evidence.md)
  cp "$root/scripts/check-authoring.sh" "$fixture/scripts/"
  chmod +x "$fixture/scripts/check-authoring.sh"
}
expect_failure(){ name=$1 expected=$2; fixture="$work/$name"; shift 2; make_fixture "$fixture"; "$@" "$fixture"; if "$fixture/scripts/check-authoring.sh" >"$fixture/output.log" 2>&1; then echo "negative test unexpectedly passed: $name" >&2; exit 1; fi; grep -Fq "$expected" "$fixture/output.log" || { cat "$fixture/output.log" >&2; echo "negative test lacked diagnostic: $expected" >&2; exit 1; }; }
remove_transcript(){ rm "$1/authoring/examples/instructional/media/moving-mark-transcript.txt"; }
remove_license(){ sed '0,/^    license: /{/^    license: /d;}' "$1/authoring/examples/instructional/assets.yaml" > "$1/a"; mv "$1/a" "$1/authoring/examples/instructional/assets.yaml"; }
add_wikilink(){ printf '\n[[non-portable-link]]\n' >> "$1/authoring/examples/instructional/index.md"; }
body_only_frontmatter(){ sed '/^objectives:/d;/^  - Locate a leaf bundle/d;/^  - Follow a standard Markdown/d;/^  - Audit an animation/d' "$1/authoring/examples/instructional/index.md" > "$1/i"; printf '\nobjectives:\n' >> "$1/i"; mv "$1/i" "$1/authoring/examples/instructional/index.md"; }
remove_source_citation(){ sed 's#\[openFrameworks project guide\](https://openframeworks.cc/learning/01_basics/create_a_new_project/)#openFrameworks project guide#' "$1/authoring/examples/instructional/index.md" > "$1/i"; mv "$1/i" "$1/authoring/examples/instructional/index.md"; }
add_unrecorded_source(){ printf '\n[Unrecorded source](https://example.invalid/unrecorded)\n' >> "$1/authoring/examples/instructional/index.md"; }
add_external_image(){ printf '\n![External example](https://example.invalid/image.svg)\n\n*External example caption.*\n' >> "$1/authoring/examples/instructional/index.md"; }
add_reference_image(){ printf '\n![Unsupported][preview]\n\n[preview]: media/moving-mark-still.svg\n' >> "$1/authoring/examples/instructional/index.md"; }
duplicate_id(){ sed 's/id: moving-mark-motion/id: moving-mark-still/' "$1/authoring/examples/instructional/assets.yaml" > "$1/a"; mv "$1/a" "$1/authoring/examples/instructional/assets.yaml"; }
duplicate_field(){ sed '0,/^    license: /s//    license: CC-BY-4.0\n    license: CC-BY-4.0/' "$1/authoring/examples/instructional/assets.yaml" > "$1/a"; mv "$1/a" "$1/authoring/examples/instructional/assets.yaml"; }
unreference_transcript(){ sed '/\[Motion transcript\]/d' "$1/authoring/examples/instructional/index.md" > "$1/i"; mv "$1/i" "$1/authoring/examples/instructional/index.md"; }
add_unrecorded_media(){ printf 'extra\n' > "$1/authoring/examples/instructional/media/extra.txt"; }
escape_repo(){ printf 'outside\n' > "$1/../escaped.txt"; printf '\n[Escape](../../../../escaped.txt)\n' >> "$1/authoring/examples/instructional/index.md"; }
symlink_escape(){ printf 'outside\n' > "$1/../symlink-target.txt"; ln -s "$1/../symlink-target.txt" "$1/authoring/examples/instructional/outside-link.txt"; printf '\n[Symlink escape](outside-link.txt)\n' >> "$1/authoring/examples/instructional/index.md"; }
malformed_header(){ sed '1s/sources:/sourcez:/' "$1/authoring/examples/instructional/sources.yaml" > "$1/s"; mv "$1/s" "$1/authoring/examples/instructional/sources.yaml"; }
unexpected_field(){ sed '0,/^    note: /s//    surprise: nope\n    note: /' "$1/authoring/examples/instructional/sources.yaml" > "$1/s"; mv "$1/s" "$1/authoring/examples/instructional/sources.yaml"; }
unexpected_record_line(){ printf '\nthis is not a record\n' >> "$1/authoring/examples/instructional/sources.yaml"; }
duplicate_source_url(){ cat >> "$1/authoring/examples/instructional/sources.yaml" <<'EOF'
  - id: duplicate-url
    title: Duplicate URL
    creator: Course authors
    url: https://OPENFRAMEWORKS.CC/learning/01_basics/create_a_new_project/
    accessed: 2026-08-18
    note: This normalized duplicate must be rejected.
EOF
}
frontmatter_only_citation(){ remove_source_citation "$1"; sed '1a\
citation_probe: https://openframeworks.cc/learning/01_basics/create_a_new_project/' "$1/authoring/examples/instructional/index.md" > "$1/i"; mv "$1/i" "$1/authoring/examples/instructional/index.md"; }
fenced_only_citation(){ remove_source_citation "$1"; cat >> "$1/authoring/examples/instructional/index.md" <<'EOF'

```markdown
[Not prose](https://openframeworks.cc/learning/01_basics/create_a_new_project/)
```
EOF
}
remove_interlude_route(){ awk -F '\t' '$4!="00-first-cpp-test-interlude"' "$1/docs/pilot/routes.tsv" > "$1/r"; mv "$1/r" "$1/docs/pilot/routes.tsv"; }
unknown_route_lesson(){ sed '0,/00-cross-platform-setup-and-first-frame/s//missing-lesson/' "$1/docs/pilot/routes.tsv" > "$1/r"; mv "$1/r" "$1/docs/pilot/routes.tsv"; }
add_route_field(){ sed '2s/$/\textra/' "$1/docs/pilot/routes.tsv" > "$1/r"; mv "$1/r" "$1/docs/pilot/routes.tsv"; }
invalid_route_sequence(){ sed '2s/\t1\t/\tone\t/' "$1/docs/pilot/routes.tsv" > "$1/r"; mv "$1/r" "$1/docs/pilot/routes.tsv"; }
reorder_route_rows(){ awk 'NR==2{first=$0;next} NR==3{print;print first;next} {print}' "$1/docs/pilot/routes.tsv" > "$1/r"; mv "$1/r" "$1/docs/pilot/routes.tsv"; }

expect_failure missing-transcript "missing asset 'media/moving-mark-transcript.txt'" remove_transcript
expect_failure missing-license "missing license" remove_license
expect_failure wikilink "Obsidian wikilinks are not portable" add_wikilink
expect_failure body-only-frontmatter "missing front matter key 'objectives'" body_only_frontmatter
expect_failure source-not-cited "is not cited in prose" remove_source_citation
expect_failure source-not-recorded "has no source record" add_unrecorded_source
expect_failure external-image "has no asset record" add_external_image
expect_failure reference-image "reference-style images are not supported" add_reference_image
expect_failure duplicate-id "duplicate record id" duplicate_id
expect_failure duplicate-field "duplicate field license" duplicate_field
expect_failure unreferenced-companion "is not referenced from index.md" unreference_transcript
expect_failure unrecorded-media "every media file must have exactly one asset record" add_unrecorded_media
expect_failure traversal-escape "escapes the repository root" escape_repo
expect_failure symlink-escape "escapes the repository root" symlink_escape
expect_failure malformed-header "expected header sources:" malformed_header
expect_failure unexpected-field "unexpected field surprise" unexpected_field
expect_failure unexpected-record-line "unexpected nonblank line" unexpected_record_line
expect_failure duplicate-source-url "duplicate source URL" duplicate_source_url
expect_failure frontmatter-only-citation "is not cited in prose" frontmatter_only_citation
expect_failure fenced-only-citation "is not cited in prose" fenced_only_citation
expect_failure route-missing-interlude "must list all 19 bundles" remove_interlude_route
expect_failure route-unknown-lesson "unknown lesson slug missing-lesson" unknown_route_lesson
expect_failure route-extra-field "must have six tab-delimited fields" add_route_field
expect_failure route-invalid-sequence "sequence must be an ASCII decimal integer" invalid_route_sequence
expect_failure route-reordered-rows "sequence must be contiguous from 1" reorder_route_rows

# Force the no-realpath branch and prove it conservatively rejects a final
# symlink. Native Windows tests omit symlink creation because runner policy can
# deny it; containment traversal remains shared across both implementations.
fixture="$work/symlink-fallback"; make_fixture "$fixture"; symlink_escape "$fixture"
if AUTHORING_CHECK_NO_REALPATH=1 "$fixture/scripts/check-authoring.sh" >"$fixture/output.log" 2>&1; then echo 'negative test unexpectedly passed: symlink-fallback' >&2; exit 1; fi
grep -Fq "cannot resolve local link 'outside-link.txt'" "$fixture/output.log" || { cat "$fixture/output.log" >&2; exit 1; }
printf '%s\n' 'authoring checker tests: positive Hugo fixture/publication builds and 26 negative contracts passed'
