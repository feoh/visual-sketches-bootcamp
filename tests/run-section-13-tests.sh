#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 13 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-13-test.XXXXXX")
MALFORMED=$(mktemp "${TMPDIR:-/tmp}/section-13-malformed.XXXXXX")
MALFORMED_LOG=$(mktemp "${TMPDIR:-/tmp}/section-13-malformed-log.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-13-incomplete-of.XXXXXX")
SENTINEL_DIR="$ROOT/exercises/13-time-as-a-drawable-axis/starter/bin"
mkdir -p "$SENTINEL_DIR"
SENTINEL=$(mktemp "$SENTINEL_DIR/wrapper-safety-sentinel.XXXXXX")
trap 'rm -f "$OUT" "$MALFORMED" "$MALFORMED_LOG" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/13-time-as-a-drawable-axis/shared" \
  -I"$ROOT/exercises/13-time-as-a-drawable-axis/starter/src/design" \
  "$ROOT/exercises/13-time-as-a-drawable-axis/shared/temporal_history.cpp" \
  "$ROOT/exercises/13-time-as-a-drawable-axis/starter/src/design/temporal_design.cpp" \
  "$ROOT/exercises/13-time-as-a-drawable-axis/tests/temporal_history_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/13-time-as-a-drawable-axis/fixtures/temporal-oracle.txt"
cat >"$MALFORMED" <<'EOF'
bad-motion motion 0 0 100
bad-selection selection 5
EOF
if "$OUT" "$MALFORMED" >"$MALFORMED_LOG" 2>&1; then
  echo 'section 13 tests: malformed fixture unexpectedly passed' >&2
  exit 1
fi
grep -Fq 'bad-motion has motion fields' "$MALFORMED_LOG"
printf '%s\n' 'section-13-fixture-safety: malformed fixture rejected before field indexing'
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-13.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then
  echo 'section 13 tests: incomplete OF_ROOT unexpectedly passed generation' >&2
  exit 1
fi
[[ -f "$SENTINEL" ]] || { echo 'section 13 tests: failed doctor removed existing output' >&2; exit 1; }
! cmp -s "$ROOT/exercises/13-time-as-a-drawable-axis/starter/src/design/temporal_design.cpp" \
         "$ROOT/exercises/13-time-as-a-drawable-axis/solution/src/design/temporal_design.cpp" || {
  echo 'section 13 tests: starter and solution design seams must diverge' >&2; exit 1;
}
printf '%s\n' 'section-13-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved; designs diverge'
