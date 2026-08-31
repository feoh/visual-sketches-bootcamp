#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 16 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-16-test.XXXXXX")
SOLUTION_OBJECT=$(mktemp "${TMPDIR:-/tmp}/section-16-solution-design.XXXXXX.o")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-16-incomplete-of.XXXXXX")
SENTINEL="$ROOT/exercises/16-structured-chance/starter/bin/wrapper-safety-sentinel"
trap 'rm -f "$OUT" "$SOLUTION_OBJECT" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/16-structured-chance/shared" \
  -I"$ROOT/exercises/16-structured-chance/starter/src/design" \
  "$ROOT/exercises/16-structured-chance/shared/structured_chance_model.cpp" \
  "$ROOT/exercises/16-structured-chance/starter/src/design/structured_chance_design.cpp" \
  "$ROOT/exercises/16-structured-chance/tests/structured_chance_model_test.cpp" -o "$OUT"
"$OUT"
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/16-structured-chance/shared" \
  -I"$ROOT/exercises/16-structured-chance/solution/src/design" \
  -c "$ROOT/exercises/16-structured-chance/solution/src/design/structured_chance_design.cpp" -o "$SOLUTION_OBJECT"
printf '%s\n' 'section-16-solution-seam: divergent solution design compiles independently'
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app" "$(dirname "$SENTINEL")"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-16.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then
  echo 'section 16 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1
fi
[[ -f "$SENTINEL" ]] || { echo 'section 16 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-16-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved'
