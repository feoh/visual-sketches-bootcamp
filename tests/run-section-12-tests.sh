#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 12 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-12-test.XXXXXX")
MALFORMED=$(mktemp "${TMPDIR:-/tmp}/section-12-malformed.XXXXXX")
MALFORMED_LOG=$(mktemp "${TMPDIR:-/tmp}/section-12-malformed-log.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-12-incomplete-of.XXXXXX")
SENTINEL_DIR="$ROOT/exercises/12-color-blending-and-trails/starter/bin"
mkdir -p "$SENTINEL_DIR"
SENTINEL=$(mktemp "$SENTINEL_DIR/wrapper-safety-sentinel.XXXXXX")
trap 'rm -f "$OUT" "$MALFORMED" "$MALFORMED_LOG" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/12-color-blending-and-trails/shared" \
  -I"$ROOT/exercises/12-color-blending-and-trails/starter/src/design" \
  "$ROOT/exercises/12-color-blending-and-trails/shared/color_trail_model.cpp" \
  "$ROOT/exercises/12-color-blending-and-trails/starter/src/design/trail_design.cpp" \
  "$ROOT/exercises/12-color-blending-and-trails/tests/color_trail_model_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/12-color-blending-and-trails/fixtures/trail-oracle.txt"
cat >"$MALFORMED" <<'EOF'
palette-start palette 0.0 0.10 0.20 0.30 1.0
palette-middle palette 0.5 0.50 0.40 0.30 0.75
palette-end palette 1.0 0.90 0.60 0.30 0.5
bad-over over 1.0 0.0
bad-decay decay 0.75
EOF
if "$OUT" "$MALFORMED" >"$MALFORMED_LOG" 2>&1; then
  echo 'section 12 tests: malformed fixture unexpectedly passed' >&2
  exit 1
fi
grep -Fq 'bad-over has over fields' "$MALFORMED_LOG"
grep -Fq 'bad-decay has decay fields' "$MALFORMED_LOG"
printf '%s\n' 'section-12-fixture-safety: malformed over/decay rows failed cleanly before indexing'
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"; chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-12.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then echo 'section 12 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1; fi
[[ -f "$SENTINEL" ]] || { echo 'section 12 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-12-wrapper-safety: incomplete OF_ROOT rejected before cleanup; unique sentinel preserved'
