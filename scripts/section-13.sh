#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
EXERCISE=13-time-as-a-drawable-axis
PROJECT=starter
CONFIGURATION=Release
OF_ROOT_ARG=
fail() { printf 'section-13: ERROR: %s\n' "$*" >&2; exit 1; }
usage() { printf '%s\n' 'usage: scripts/section-13.sh <doctor|generate|build> [--of-root PATH] [--project starter|solution] [--configuration Debug|Release]'; }
[[ $# -gt 0 ]] || { usage; exit 2; }
COMMAND=$1; shift
while [[ $# -gt 0 ]]; do
  case "$1" in
    --of-root) [[ $# -ge 2 ]] || fail '--of-root requires a path'; OF_ROOT_ARG=$2; shift 2 ;;
    --project) [[ $# -ge 2 ]] || fail '--project requires a value'; PROJECT=$2; shift 2 ;;
    --configuration) [[ $# -ge 2 ]] || fail '--configuration requires a value'; CONFIGURATION=$2; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) fail "unknown argument: $1" ;;
  esac
done
[[ "$PROJECT" == starter || "$PROJECT" == solution ]] || fail 'project must be starter or solution'
[[ "$CONFIGURATION" == Debug || "$CONFIGURATION" == Release ]] || fail 'configuration must be Debug or Release'
PROJECT_PATH="$ROOT/exercises/$EXERCISE/$PROJECT"

inventory() {
  local expected actual
  expected=$(mktemp); actual=$(mktemp)
  cat >"$expected" <<EOF
exercises/$EXERCISE/$PROJECT/src/main.cpp
exercises/$EXERCISE/$PROJECT/src/ofApp.cpp
exercises/$EXERCISE/$PROJECT/src/ofApp.h
exercises/$EXERCISE/$PROJECT/src/design/temporal_design.cpp
exercises/$EXERCISE/$PROJECT/src/design/temporal_design.h
exercises/$EXERCISE/shared/temporal_history.cpp
exercises/$EXERCISE/shared/temporal_history.h
EOF
  find "$PROJECT_PATH/src" "$ROOT/exercises/$EXERCISE/shared" -type f \( -name '*.cpp' -o -name '*.h' \) -print |
    while IFS= read -r file; do printf '%s\n' "${file#"$ROOT"/}"; done >"$actual"
  LC_ALL=C sort -o "$expected" "$expected"; LC_ALL=C sort -o "$actual" "$actual"
  if ! cmp -s "$expected" "$actual"; then diff -u "$expected" "$actual" >&2 || true; rm -f "$expected" "$actual"; fail 'source inventory is incomplete or stale'; fi
  rm -f "$expected" "$actual"
}

resolve() {
  local candidate=${OF_ROOT_ARG:-${OF_ROOT:-}}
  [[ -n "$candidate" && -d "$candidate" ]] || fail 'set OF_ROOT or pass --of-root PATH'
  OF_ROOT=$(cd "$candidate" && pwd -P)
  case "$(uname -s):$(uname -m)" in
    Linux:x86_64) PLATFORM=linux64; PG="$OF_ROOT/projectGenerator/resources/app/app/projectGenerator" ;;
    Darwin:arm64) PLATFORM=osx; PG="$OF_ROOT/projectGenerator/projectGenerator.app/Contents/Resources/app/app/projectGenerator" ;;
    *) fail "unsupported host $(uname -s) $(uname -m); use the PowerShell wrapper on Windows" ;;
  esac
  [[ -x "$PG" ]] || fail "packaged Project Generator is missing: $PG"
}

doctor() {
  resolve
  local required constants version pg_version
  for required in addons libs/openFrameworks libs/openFrameworksCompiled scripts scripts/templates "scripts/templates/$PLATFORM"; do
    [[ -d "$OF_ROOT/$required" ]] || fail "openFrameworks directory is missing: $required"
  done
  for required in libs/openFrameworks/utils/ofConstants.h libs/openFrameworks/ofMain.h libs/openFrameworks/app/ofAppRunner.h "scripts/templates/$PLATFORM/config.make"; do
    [[ -f "$OF_ROOT/$required" ]] || fail "openFrameworks file is missing: $required"
  done
  constants="$OF_ROOT/libs/openFrameworks/utils/ofConstants.h"
  version=$(awk '$1=="#define"&&$2~/^OF_VERSION_(MAJOR|MINOR|PATCH)$/{v[$2]=$3} END{print v["OF_VERSION_MAJOR"] "." v["OF_VERSION_MINOR"] "." v["OF_VERSION_PATCH"]}' "$constants")
  [[ "$version" == 0.12.1 ]] || fail "expected openFrameworks 0.12.1, observed $version"
  pg_version=$({ "$PG" --version; } 2>&1) || fail "Project Generator could not run: $pg_version"
  grep -Eq '(^|[^0-9])0\.103\.0([^0-9]|$)' <<<"$pg_version" || fail "expected Project Generator 0.103.0, observed $pg_version"
  [[ -d "$PROJECT_PATH/src" && -f "$PROJECT_PATH/addons.make" && ! -s "$PROJECT_PATH/addons.make" ]] || fail 'canonical project inputs are missing or addons.make is not empty'
  inventory
  printf 'section-13: host=%s project=%s OF=%s PG=0.103.0\n' "$PLATFORM" "$PROJECT" "$version"
}

clean_generated() {
  [[ "$PROJECT_PATH" == "$ROOT/exercises/$EXERCISE/"* && ! -L "$PROJECT_PATH" && ! -L "$PROJECT_PATH/bin" ]] || fail 'unsafe project path'
  if [[ -d "$PROJECT_PATH/bin" ]]; then find "$PROJECT_PATH/bin" -mindepth 1 -maxdepth 1 ! -name data -exec rm -rf -- {} +; fi
  rm -rf -- "$PROJECT_PATH/Makefile" "$PROJECT_PATH/config.make" "$PROJECT_PATH/.vscode" "$PROJECT_PATH/$PROJECT.code-workspace" \
    "$PROJECT_PATH/$PROJECT.xcodeproj" "$PROJECT_PATH/Project.xcconfig" "$PROJECT_PATH/of.entitlements" \
    "$PROJECT_PATH/openFrameworks-Info.plist" "$PROJECT_PATH/obj" "$PROJECT_PATH/$PROJECT.app" "$PROJECT_PATH/${PROJECT}Debug.app"
}

generate() {
  doctor
  local before after sources='../shared'
  before=$(mktemp); after=$(mktemp); trap 'rm -f "$before" "$after"' RETURN
  git -C "$ROOT" diff --binary --no-ext-diff -- >"$before"
  clean_generated
  [[ "$PLATFORM" == osx ]] && sources='src/design,../shared'
  (cd "$ROOT" && PG_OF_PATH="$OF_ROOT" "$PG" -o"$OF_ROOT" -p"$PLATFORM" -s"$sources" "exercises/$EXERCISE/$PROJECT")
  case "$PLATFORM" in
    linux64)
      [[ -s "$PROJECT_PATH/Makefile" && -s "$PROJECT_PATH/config.make" ]] || fail 'generator omitted Makefile or config.make'
      grep -Eq '^PROJECT_EXTERNAL_SOURCE_PATHS = ../shared$' "$PROJECT_PATH/config.make" || fail 'generated project omitted shared model path'
      ;;
    osx) [[ -s "$PROJECT_PATH/$PROJECT.xcodeproj/project.pbxproj" ]] || fail 'generator omitted Xcode project' ;;
  esac
  inventory
  git -C "$ROOT" diff --binary --no-ext-diff -- >"$after"
  cmp -s "$before" "$after" || fail 'generation changed tracked files'
  printf 'section-13: generated %s for %s\n' "$PROJECT" "$PLATFORM"
}

build() {
  doctor
  case "$PLATFORM" in
    linux64)
      [[ -s "$PROJECT_PATH/Makefile" ]] || fail 'generate before build'
      jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf 2)
      (cd "$PROJECT_PATH" && make -j"$jobs" "$CONFIGURATION")
      product="$PROJECT_PATH/bin/$PROJECT"; [[ "$CONFIGURATION" == Debug ]] && product="${product}_debug"
      ;;
    osx)
      [[ -d "$PROJECT_PATH/$PROJECT.xcodeproj" ]] || fail 'generate before build'
      xcodebuild -project "$PROJECT_PATH/$PROJECT.xcodeproj" -scheme "$PROJECT $CONFIGURATION" -configuration "$CONFIGURATION" -sdk macosx
      product="$PROJECT_PATH/bin/$PROJECT.app"; [[ "$CONFIGURATION" == Debug ]] && product="$PROJECT_PATH/bin/${PROJECT}Debug.app"
      ;;
  esac
  [[ -e "$product" ]] || fail "expected product is missing: $product"
  printf 'section-13: built %s\n' "$product"
}

case "$COMMAND" in doctor) doctor ;; generate) generate ;; build) build ;; *) usage; exit 2 ;; esac
