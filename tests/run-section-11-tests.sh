#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 11 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-11-test.XXXXXX")
MALFORMED=$(mktemp "${TMPDIR:-/tmp}/section-11-malformed.XXXXXX")
MALFORMED_LOG=$(mktemp "${TMPDIR:-/tmp}/section-11-malformed-log.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-11-incomplete-of.XXXXXX")
SENTINEL_DIR="$ROOT/exercises/11-noise-and-flow-fields/starter/bin"
mkdir -p "$SENTINEL_DIR"
SENTINEL=$(mktemp "$SENTINEL_DIR/wrapper-safety-sentinel.XXXXXX")
trap 'rm -f "$OUT" "$MALFORMED" "$MALFORMED_LOG" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/11-noise-and-flow-fields/shared" \
  -I"$ROOT/exercises/11-noise-and-flow-fields/starter/src/design" \
  "$ROOT/exercises/11-noise-and-flow-fields/shared/flow_field_model.cpp" \
  "$ROOT/exercises/11-noise-and-flow-fields/starter/src/design/flow_field_design.cpp" \
  "$ROOT/exercises/11-noise-and-flow-fields/tests/flow_field_model_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/11-noise-and-flow-fields/fixtures/flow-oracle.txt"
cat >"$MALFORMED" <<'EOF'
hash-origin hash 0 0 0 1 1244833532
hash-offset hash 3 5 7 42 1072089522
hash-negative hash -2 4 -6 99 930434099
bad-sample sample 0.5
bad-angle angle 0.25 0.0
EOF
if "$OUT" "$MALFORMED" >"$MALFORMED_LOG" 2>&1; then
  echo 'section 11 tests: malformed fixture unexpectedly passed' >&2
  exit 1
fi
grep -Fq 'bad-sample has sample fields' "$MALFORMED_LOG"
grep -Fq 'bad-angle has angle fields' "$MALFORMED_LOG"
printf '%s\n' 'section-11-fixture-safety: malformed sample/angle rows failed cleanly before indexing'
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"; chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-11.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then echo 'section 11 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1; fi
[[ -f "$SENTINEL" ]] || { echo 'section 11 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-11-wrapper-safety: incomplete OF_ROOT rejected before cleanup; unique sentinel preserved'
