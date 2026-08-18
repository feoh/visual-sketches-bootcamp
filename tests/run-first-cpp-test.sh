#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
CXX=${CXX:-c++}
VARIANT=${1:-starter}
if [[ $# -gt 1 || ( "$VARIANT" != starter && "$VARIANT" != solution ) ]]; then
  echo "usage: tests/run-first-cpp-test.sh [starter|solution]" >&2
  exit 2
fi
command -v "$CXX" >/dev/null || { echo "first C++ test: compiler not found: $CXX" >&2; exit 1; }
OUT=$(mktemp "${TMPDIR:-/tmp}/first-cpp-test.XXXXXX")
trap 'rm -f "$OUT"' EXIT
"$CXX" -std=c++17 -DNDEBUG -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/shared/core" -I"$ROOT/shared/test-support" \
  -I"$ROOT/exercises/00-first-cpp-test/$VARIANT" \
  "$ROOT/shared/core/course_probe.cpp" \
  "$ROOT/exercises/00-first-cpp-test/$VARIANT/learner_known_case.cpp" \
  "$ROOT/exercises/00-first-cpp-test/tests/learner_known_case_test.cpp" -o "$OUT"
"$OUT"
