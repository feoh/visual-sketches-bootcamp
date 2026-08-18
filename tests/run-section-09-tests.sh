#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 09 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-09-test.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-09-incomplete-of.XXXXXX")
SENTINEL="$ROOT/exercises/09-particles-with-memory/starter/bin/wrapper-safety-sentinel"
trap 'rm -f "$OUT" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/09-particles-with-memory/shared" \
  -I"$ROOT/exercises/09-particles-with-memory/starter/src/design" \
  "$ROOT/exercises/09-particles-with-memory/shared/particle_model.cpp" \
  "$ROOT/exercises/09-particles-with-memory/starter/src/design/particle_design.cpp" \
  "$ROOT/exercises/09-particles-with-memory/tests/particle_model_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/09-particles-with-memory/fixtures/particle-oracle.txt"
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app" "$(dirname "$SENTINEL")"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"; chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-09.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then echo 'section 09 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1; fi
[[ -f "$SENTINEL" ]] || { echo 'section 09 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-09-wrapper-safety: incomplete OF_ROOT rejected before cleanup; sentinel preserved'
