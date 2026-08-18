#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 10 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-10-test.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-10-incomplete-of.XXXXXX")
SENTINEL="$ROOT/exercises/10-forces-steering-and-springs/starter/bin/wrapper-safety-sentinel"
trap 'rm -f "$OUT" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/10-forces-steering-and-springs/shared" \
  -I"$ROOT/exercises/10-forces-steering-and-springs/starter/src/design" \
  "$ROOT/exercises/10-forces-steering-and-springs/shared/force_model.cpp" \
  "$ROOT/exercises/10-forces-steering-and-springs/starter/src/design/force_design.cpp" \
  "$ROOT/exercises/10-forces-steering-and-springs/tests/force_model_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/10-forces-steering-and-springs/fixtures/spring-oracle.txt"
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app" "$(dirname "$SENTINEL")"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"; chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-10.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then echo 'section 10 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1; fi
[[ -f "$SENTINEL" ]] || { echo 'section 10 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-10-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved'
