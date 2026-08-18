#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
command -v "$CXX" >/dev/null || { echo "section 12 tests: C++ compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/section-12-test.XXXXXX")
MALFORMED=$(mktemp "${TMPDIR:-/tmp}/section-12-malformed.XXXXXX")
MALFORMED_LOG=$(mktemp "${TMPDIR:-/tmp}/section-12-malformed-log.XXXXXX")
FAKE_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-12-incomplete-of.XXXXXX")
SAFETY_ROOT=$(mktemp -d "${TMPDIR:-/tmp}/section-12-symlink-root.XXXXXX")
SAFETY_EXTERNAL=$(mktemp -d "${TMPDIR:-/tmp}/section-12-symlink-external.XXXXXX")
SAFETY_OF=$(mktemp -d "${TMPDIR:-/tmp}/section-12-symlink-of.XXXXXX")
SENTINEL_DIR="$ROOT/exercises/12-color-blending-and-trails/starter/bin"
mkdir -p "$SENTINEL_DIR"
SENTINEL=$(mktemp "$SENTINEL_DIR/wrapper-safety-sentinel.XXXXXX")
trap 'rm -f "$OUT" "$MALFORMED" "$MALFORMED_LOG" "$SENTINEL"; rm -rf "$FAKE_OF" "$SAFETY_ROOT" "$SAFETY_EXTERNAL" "$SAFETY_OF"' EXIT
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
for main in "$ROOT"/exercises/{12-color-blending-and-trails,13-time-as-a-drawable-axis,14-images-and-type-as-geometry,15-embodied-audio-input}/{starter,solution}/src/main.cpp; do
  grep -Fq 'ofCreateWindow(settings)' "$main"
  grep -Fq 'ofRunApp(window, std::make_shared<ofApp>())' "$main"
  grep -Fq 'ofRunMainLoop()' "$main"
done
printf '%s\n' 'unit-4-window-contract: every entry point creates and runs its configured window'
mkdir -p "$FAKE_OF/projectGenerator/resources/app/app"
printf '#!/bin/sh\nexit 99\n' >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"; chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
printf '%s\n' 'preserve me' >"$SENTINEL"
if "$ROOT/scripts/section-12.sh" generate --of-root "$FAKE_OF" --project starter >/dev/null 2>&1; then echo 'section 12 tests: incomplete OF_ROOT unexpectedly passed generation' >&2; exit 1; fi
[[ -f "$SENTINEL" ]] || { echo 'section 12 tests: failed doctor removed existing output' >&2; exit 1; }
printf '%s\n' 'section-12-wrapper-safety: incomplete OF_ROOT rejected before cleanup; unique sentinel preserved'

mkdir -p "$SAFETY_ROOT/scripts" "$SAFETY_ROOT/exercises"
cp "$ROOT/scripts/course-project.sh" "$SAFETY_ROOT/scripts/course-project.sh"
chmod +x "$SAFETY_ROOT/scripts/course-project.sh"
mkdir -p "$SAFETY_EXTERNAL/starter/src/design" "$SAFETY_EXTERNAL/starter/bin/data" \
  "$SAFETY_EXTERNAL/shared"
touch "$SAFETY_EXTERNAL/starter/addons.make" \
  "$SAFETY_EXTERNAL/starter/src/main.cpp" \
  "$SAFETY_EXTERNAL/starter/src/ofApp.cpp" \
  "$SAFETY_EXTERNAL/starter/src/ofApp.h" \
  "$SAFETY_EXTERNAL/starter/src/design/trail_design.cpp" \
  "$SAFETY_EXTERNAL/starter/src/design/trail_design.h" \
  "$SAFETY_EXTERNAL/shared/color_trail_model.cpp" \
  "$SAFETY_EXTERNAL/shared/color_trail_model.h"
printf '%s\n' 'preserve external data' >"$SAFETY_EXTERNAL/starter/bin/external-sentinel"
ln -s "$SAFETY_EXTERNAL" "$SAFETY_ROOT/exercises/12-color-blending-and-trails"
mkdir -p "$SAFETY_OF/projectGenerator/resources/app/app" \
  "$SAFETY_OF/addons" "$SAFETY_OF/libs/openFrameworks/utils" \
  "$SAFETY_OF/libs/openFrameworks/app" "$SAFETY_OF/libs/openFrameworksCompiled" \
  "$SAFETY_OF/scripts/templates/linux64"
cat >"$SAFETY_OF/libs/openFrameworks/utils/ofConstants.h" <<'EOF'
#define OF_VERSION_MAJOR 0
#define OF_VERSION_MINOR 12
#define OF_VERSION_PATCH 1
EOF
touch "$SAFETY_OF/libs/openFrameworks/ofMain.h" \
  "$SAFETY_OF/libs/openFrameworks/app/ofAppRunner.h" \
  "$SAFETY_OF/scripts/templates/linux64/config.make"
cat >"$SAFETY_OF/projectGenerator/resources/app/app/projectGenerator" <<EOF
#!/bin/sh
if [ "\${1:-}" = --version ]; then echo 0.103.0; exit 0; fi
touch "$SAFETY_OF/generator-ran"
exit 99
EOF
chmod +x "$SAFETY_OF/projectGenerator/resources/app/app/projectGenerator"
if "$SAFETY_ROOT/scripts/course-project.sh" 12 generate --of-root "$SAFETY_OF" \
    --project starter >/dev/null 2>&1; then
  echo 'section 12 tests: symlinked exercise ancestor unexpectedly passed cleanup guard' >&2
  exit 1
fi
[[ -f "$SAFETY_EXTERNAL/starter/bin/external-sentinel" &&
   ! -e "$SAFETY_OF/generator-ran" ]] || {
  echo 'section 12 tests: symlinked ancestor reached cleanup or generation' >&2
  exit 1
}
printf '%s\n' 'section-12-ancestor-safety: symlinked exercise rejected before cleanup or generation'
