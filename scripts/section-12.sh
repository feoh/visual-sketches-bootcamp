#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)
ROOT=$(cd "$SCRIPT_DIR/.." && pwd -P)
PATCHED=$(mktemp "${TMPDIR:-/tmp}/section-12-course-project.XXXXXX")
trap 'rm -f "$PATCHED"' EXIT
awk '
  /^ROOT=\$\(cd / {
    print "ROOT=${SECTION_12_ROOT:?section 12 root is required}"
    next
  }
  /^  \*\) echo "course-project: unsupported section/ {
    print "  12) EXERCISE=12-color-blending-and-trails; DESIGN=trail_design; SHARED=color_trail_model ;;"
  }
  { print }
' "$SCRIPT_DIR/course-project.sh" >"$PATCHED"
chmod +x "$PATCHED"
export SECTION_12_ROOT="$ROOT"
set +e
"$PATCHED" 12 "$@"
status=$?
set -e
exit "$status"
