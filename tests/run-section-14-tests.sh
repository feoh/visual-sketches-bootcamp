#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 14 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-14-test.XXXXXX")
MALFORMED=$(mktemp "${TMPDIR:-/tmp}/section-14-malformed.XXXXXX")
LOG=$(mktemp "${TMPDIR:-/tmp}/section-14-log.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-14-incomplete-of.XXXXXX")
SENTINEL_DIR="$ROOT/exercises/14-images-and-type-as-geometry/starter/bin"
mkdir -p "$SENTINEL_DIR"
SENTINEL=$(mktemp "$SENTINEL_DIR/wrapper-safety-sentinel.XXXXXX")
trap 'rm -f "$OUT" "$MALFORMED" "$LOG" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
 -I"$ROOT/exercises/14-images-and-type-as-geometry/shared" \
 -I"$ROOT/exercises/14-images-and-type-as-geometry/starter/src/design" \
 "$ROOT/exercises/14-images-and-type-as-geometry/shared/image_geometry_model.cpp" \
 "$ROOT/exercises/14-images-and-type-as-geometry/starter/src/design/image_geometry_design.cpp" \
 "$ROOT/exercises/14-images-and-type-as-geometry/tests/image_geometry_model_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/14-images-and-type-as-geometry/fixtures/mask-oracle.txt"
printf '%s\n' 'broken 3 3 1' >"$MALFORMED"
if "$OUT" "$MALFORMED" >"$LOG" 2>&1; then
 echo 'section 14 tests: malformed fixture unexpectedly passed' >&2; exit 1
fi
grep -Fq 'broken oracle cardinality' "$LOG"
printf '%s\n' 'section-14-fixture-safety: malformed row failed cardinality check before allocation/indexing'
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"; chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-14.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then
 echo 'section 14 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1
fi
[[ -f "$SENTINEL" ]] || { echo 'section 14 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-14-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved'
