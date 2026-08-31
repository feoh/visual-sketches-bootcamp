#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 17 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-17-test.XXXXXX")
SOLUTION_OBJECT=$(mktemp "${TMPDIR:-/tmp}/section-17-solution-design.XXXXXX.o")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-17-incomplete-of.XXXXXX")
SENTINEL="$ROOT/exercises/17-depth-light-and-populations/starter/bin/wrapper-safety-sentinel"
trap 'rm -f "$OUT" "$SOLUTION_OBJECT" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/17-depth-light-and-populations/shared" \
  -I"$ROOT/exercises/17-depth-light-and-populations/starter/src/design" \
  "$ROOT/exercises/17-depth-light-and-populations/shared/population_model.cpp" \
  "$ROOT/exercises/17-depth-light-and-populations/starter/src/design/population_design.cpp" \
  "$ROOT/exercises/17-depth-light-and-populations/tests/population_model_test.cpp" -o "$OUT"
"$OUT"
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/17-depth-light-and-populations/shared" \
  -I"$ROOT/exercises/17-depth-light-and-populations/solution/src/design" \
  -c "$ROOT/exercises/17-depth-light-and-populations/solution/src/design/population_design.cpp" -o "$SOLUTION_OBJECT"
printf '%s\n' 'section-17-solution-seam: divergent solution design compiles independently'
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app" "$(dirname "$SENTINEL")"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-17.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then
  echo 'section 17 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1
fi
[[ -f "$SENTINEL" ]] || { echo 'section 17 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-17-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved'
