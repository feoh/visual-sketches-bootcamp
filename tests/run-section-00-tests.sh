#!/bin/sh
set -eu
root=$(CDPATH='' cd -- "$(dirname -- "$0")/.." && pwd -P)
build=$(mktemp -d "${TMPDIR:-/tmp}/section-00-tests.XXXXXX")
trap 'rm -rf "$build"' EXIT HUP INT TERM
cxx=${CXX:-c++}
shared="$root/exercises/00-visual-signature/shared"
starter="$root/exercises/00-visual-signature/starter/src/design"
test_source="$root/exercises/00-visual-signature/tests/signature_geometry_test.cpp"

if "$cxx" -std=c++14 -I"$shared" -I"$starter" -c "$test_source" \
    -o "$build/should-not-compile.o" >"$build/cxx14.log" 2>&1; then
  printf '%s\n' 'section-00-tests: ERROR: test source unexpectedly compiled below C++17' >&2
  exit 1
fi
"$cxx" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$shared" -I"$starter" \
  "$shared/signature_geometry.cpp" \
  "$starter/signature_design.cpp" \
  "$test_source" \
  -o "$build/signature_geometry_test"
"$build/signature_geometry_test" "$root/exercises/00-visual-signature/fixtures/viewports.tsv"
