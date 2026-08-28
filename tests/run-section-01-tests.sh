#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
VARIANT=${1:-starter}
case "$VARIANT" in starter|solution) ;; *) echo 'usage: tests/run-section-01-tests.sh [starter|solution]' >&2; exit 2 ;; esac
command -v "$CXX" >/dev/null || { echo "section 01 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-01-test.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-01-incomplete-of.XXXXXX")
SENTINEL="$ROOT/exercises/01-a-mark-that-moves/starter/bin/wrapper-safety-sentinel"
trap 'rm -f "$OUT" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/01-a-mark-that-moves/shared" \
  -I"$ROOT/exercises/01-a-mark-that-moves/$VARIANT/src/design" \
  "$ROOT/exercises/01-a-mark-that-moves/shared/traveler_model.cpp" \
  "$ROOT/exercises/01-a-mark-that-moves/$VARIANT/src/design/traveler_design.cpp" \
  "$ROOT/exercises/01-a-mark-that-moves/tests/traveler_model_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/01-a-mark-that-moves/fixtures/frame-streams.tsv"

mkdir -p "$FAKE_OF/projectGenerator/resources/app/app" "$(dirname "$SENTINEL")"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-01.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then
  echo 'section 01 tests: incomplete OF_ROOT unexpectedly passed generation' >&2
  exit 1
fi
[[ -f "$SENTINEL" ]] || { echo 'section 01 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-01-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved'
