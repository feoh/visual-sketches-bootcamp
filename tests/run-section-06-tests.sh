#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 06 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-06-test.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-06-incomplete-of.XXXXXX")
SENTINEL="$ROOT/exercises/06-controlled-chance/starter/bin/wrapper-safety-sentinel"
trap 'rm -f "$OUT" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/06-controlled-chance/shared" \
  -I"$ROOT/exercises/06-controlled-chance/starter/src/design" \
  "$ROOT/exercises/06-controlled-chance/shared/edition_model.cpp" \
  "$ROOT/exercises/06-controlled-chance/starter/src/design/edition_design.cpp" \
  "$ROOT/exercises/06-controlled-chance/tests/edition_model_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/06-controlled-chance/fixtures/portable-edition.txt"
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app" "$(dirname "$SENTINEL")"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-06.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then
  echo 'section 06 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1
fi
[[ -f "$SENTINEL" ]] || { echo 'section 06 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-06-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved'
