#!/usr/bin/env bash
set -euo pipefail
repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
out=$(mktemp "${TMPDIR:-/tmp}/core-probe.XXXXXX")
trap 'rm -f "$out"' EXIT
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$repo_root/shared/core" -I"$repo_root/shared/test-support" \
  "$repo_root/shared/core/course_probe.cpp" "$repo_root/tests/core_probe_test.cpp" \
  -o "$out"
"$out"
