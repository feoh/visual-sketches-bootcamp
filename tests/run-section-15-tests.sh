#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 15 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-15-test.XXXXXX")
SOLUTION_OBJECT=$(mktemp "${TMPDIR:-/tmp}/section-15-solution-design.XXXXXX.o")
MALFORMED=$(mktemp "${TMPDIR:-/tmp}/section-15-malformed.XXXXXX")
MALFORMED_LOG=$(mktemp "${TMPDIR:-/tmp}/section-15-malformed-log.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-15-incomplete-of.XXXXXX")
SENTINEL_DIR="$ROOT/exercises/15-embodied-audio-input/starter/bin"
mkdir -p "$SENTINEL_DIR"
SENTINEL=$(mktemp "$SENTINEL_DIR/wrapper-safety-sentinel.XXXXXX")
trap 'rm -f "$OUT" "$SOLUTION_OBJECT" "$MALFORMED" "$MALFORMED_LOG" "$SENTINEL"; rm -rf "$FAKE_OF"' EXIT

"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/15-embodied-audio-input/shared" \
  -I"$ROOT/exercises/15-embodied-audio-input/starter/src/design" \
  "$ROOT/exercises/15-embodied-audio-input/shared/audio_input_model.cpp" \
  "$ROOT/exercises/15-embodied-audio-input/starter/src/design/audio_instrument_design.cpp" \
  "$ROOT/exercises/15-embodied-audio-input/tests/audio_input_model_test.cpp" -o "$OUT"
"$OUT" "$ROOT/exercises/15-embodied-audio-input/fixtures/amplitude-replay.txt"
"$CXX" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/exercises/15-embodied-audio-input/shared" \
  -I"$ROOT/exercises/15-embodied-audio-input/solution/src/design" \
  -c "$ROOT/exercises/15-embodied-audio-input/solution/src/design/audio_instrument_design.cpp" \
  -o "$SOLUTION_OBJECT"
printf '%s\n' 'section-15-solution-seam: divergent solution design compiles independently'

for adapter in \
  "$ROOT/exercises/15-embodied-audio-input/starter/src/ofApp.cpp" \
  "$ROOT/exercises/15-embodied-audio-input/solution/src/ofApp.cpp"; do
  grep -Fq 'chooseNoDevice();  // Safe default' "$adapter"
  grep -Fq "key == 'f'" "$adapter"
  grep -Fq "key == 'l'" "$adapter"
  grep -Fq "key == 'n'" "$adapter"
  grep -Fq 'sound_stream_.setup(settings)' "$adapter"
  grep -Fq 'sound_stream_.close()' "$adapter"
done
printf '%s\n' 'section-15-adapter-contract: safe default, N/F/L routes, and live open/close hooks are discoverable'

cat >"$MALFORMED" <<'EOF'
# deliberately short and long rows
short 0.1 0.05
long 0.2 0.1 0.0 20.0 4 0 unexpected
EOF
if "$OUT" "$MALFORMED" >"$MALFORMED_LOG" 2>&1; then
  echo 'section 15 tests: malformed fixture unexpectedly passed' >&2
  exit 1
fi
grep -Fq 'must have exactly seven fields' "$MALFORMED_LOG"
printf '%s\n' 'section-15-fixture-safety: malformed rows failed cleanly before use'

mkdir -p "$FAKE_OF/projectGenerator/resources/app/app"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-15.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then
  echo 'section 15 tests: incomplete OF_ROOT unexpectedly passed generation' >&2
  exit 1
fi
[[ -f "$SENTINEL" ]] || { echo 'section 15 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-15-wrapper-safety: incomplete OF_ROOT rejected before cleanup; unique sentinel preserved'
