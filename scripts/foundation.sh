#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
EXPECTED_TESTS=12
OF_ROOT_ARG=""
PROJECT="all"
CONFIGURATION="Release"

fail() { printf 'foundation: ERROR: %s\n' "$*" >&2; exit 1; }
info() { printf 'foundation: %s\n' "$*"; }
usage() {
  cat <<'USAGE'
Usage: scripts/foundation.sh <doctor|generate|build|test> [options]
  --of-root PATH                 openFrameworks 0.12.1 root (else OF_ROOT)
  --project all|windowed|unit    default: all (test requires unit)
  --configuration Debug|Release default: Release
USAGE
}

[[ $# -gt 0 ]] || { usage; exit 2; }
COMMAND=$1; shift
while [[ $# -gt 0 ]]; do
  case "$1" in
    --of-root) [[ $# -ge 2 ]] || fail "--of-root requires a path"; OF_ROOT_ARG=$2; shift 2 ;;
    --project) [[ $# -ge 2 ]] || fail "--project requires a value"; PROJECT=$2; shift 2 ;;
    --configuration) [[ $# -ge 2 ]] || fail "--configuration requires a value"; CONFIGURATION=$2; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) fail "unknown argument: $1" ;;
  esac
done
[[ "$PROJECT" == all || "$PROJECT" == windowed || "$PROJECT" == unit ]] || fail "project must be all, windowed, or unit"
[[ "$CONFIGURATION" == Debug || "$CONFIGURATION" == Release ]] || fail "configuration must be Debug or Release"

resolve_environment() {
  local candidate=${OF_ROOT_ARG:-${OF_ROOT:-}}
  [[ -n "$candidate" ]] || fail "set OF_ROOT or pass --of-root PATH"
  [[ -d "$candidate" ]] || fail "openFrameworks root does not exist: $candidate"
  OF_ROOT=$(cd "$candidate" && pwd -P)
  for directory in addons libs scripts; do [[ -d "$OF_ROOT/$directory" ]] || fail "OF_ROOT is missing $directory/: $OF_ROOT"; done

  case "$(uname -s):$(uname -m)" in
    Linux:x86_64) HOST_PLATFORM=linux64; PG="$OF_ROOT/projectGenerator/resources/app/app/projectGenerator" ;;
    Darwin:arm64) HOST_PLATFORM=osx; PG="$OF_ROOT/projectGenerator/projectGenerator.app/Contents/Resources/app/app/projectGenerator" ;;
    Linux:*) fail "Linux foundation lane requires x86_64; observed $(uname -m)" ;;
    Darwin:*) fail "macOS foundation lane requires arm64; observed $(uname -m)" ;;
    *) fail "unsupported host $(uname -s); use scripts/foundation.ps1 on Windows" ;;
  esac
  [[ -x "$PG" ]] || fail "packaged Project Generator is missing or not executable: $PG"
}

version_macro() {
  local name=$1 file="$OF_ROOT/libs/openFrameworks/utils/ofConstants.h"
  awk -v macro="OF_VERSION_$name" '$1=="#define" && $2==macro {print $3}' "$file"
}

validate_addons_file() {
  local name=$1 file="$REPO_ROOT/foundation/$1/addons.make" raw line seen
  seen=$(mktemp)
  while IFS= read -r raw || [[ -n "$raw" ]]; do
    raw=${raw%$'\r'}
    line=$(printf '%s' "$raw" | sed -E 's/^[[:space:]]+//; s/[[:space:]]+$//')
    [[ -n "$line" ]] || continue
    [[ "$raw" == "$line" ]] || { rm -f "$seen"; fail "$file contains surrounding whitespace: $raw"; }
    if [[ "$line" == /* || "$line" == \\* || "$line" == //* || "$line" =~ ^\~[/\\] || "$line" =~ ^[[:alpha:]]: ]]; then
      rm -f "$seen"; fail "$file contains an absolute or drive-qualified path: $line"
    fi
    if grep -Fqx -- "$line" "$seen"; then
      rm -f "$seen"; fail "$file contains duplicate addon: $line"
    fi
    printf '%s\n' "$line" >>"$seen"
  done <"$file"
  if [[ "$name" == unit ]]; then
    if [[ $(wc -l <"$seen" | tr -d ' ') != 1 ]] || ! grep -Fqx ofxUnitTests "$seen"; then
      rm -f "$seen"; fail "$file must contain only ofxUnitTests exactly once"
    fi
  else
    [[ ! -s "$seen" ]] || { rm -f "$seen"; fail "$file must be empty for the addon-free windowed probe"; }
  fi
  rm -f "$seen"
}

validate_source_inventory() {
  local expected actual root file
  expected=$(mktemp); actual=$(mktemp)
  cat >"$expected" <<'INVENTORY'
foundation/unit/src/main.cpp
foundation/unit/src/ofApp.cpp
foundation/unit/src/ofApp.h
foundation/windowed/src/main.cpp
foundation/windowed/src/ofApp.cpp
foundation/windowed/src/ofApp.h
shared/core/course_probe.cpp
shared/core/course_probe.h
shared/test-support/deterministic_fixture.h
shared/test-support/expect_near.h
INVENTORY
  for root in foundation/unit/src foundation/windowed/src shared/core shared/test-support; do
    while IFS= read -r file; do printf '%s\n' "${file#"$REPO_ROOT"/}" >>"$actual"; done < <(
      find "$REPO_ROOT/$root" -type f \( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.h' -o -name '*.hh' -o -name '*.hpp' \)
    )
  done
  LC_ALL=C sort -o "$expected" "$expected"; LC_ALL=C sort -o "$actual" "$actual"
  if ! cmp -s "$expected" "$actual"; then
    diff -u "$expected" "$actual" >&2 || true
    rm -f "$expected" "$actual"; fail "course source inventory has missing or stale files"
  fi
  rm -f "$expected" "$actual"
}

doctor() {
  resolve_environment
  command -v git >/dev/null || fail "git is required to verify generation does not change tracked inputs"
  local major minor patch pg_version
  major=$(version_macro MAJOR); minor=$(version_macro MINOR); patch=$(version_macro PATCH)
  [[ "$major.$minor.$patch" == 0.12.1 ]] || fail "expected openFrameworks 0.12.1, observed ${major:-?}.${minor:-?}.${patch:-?} at $OF_ROOT"
  pg_version=$({ "$PG" --version; } 2>&1) || fail "Project Generator could not run: $pg_version"
  grep -Eq '(^|[^0-9])0\.103\.0([^0-9]|$)' <<<"$pg_version" || fail "expected Project Generator 0.103.0, observed: $pg_version"
  for project in windowed unit; do
    [[ -d "$REPO_ROOT/foundation/$project/src" ]] || fail "missing foundation/$project/src"
    [[ -f "$REPO_ROOT/foundation/$project/addons.make" ]] || fail "missing foundation/$project/addons.make"
    validate_addons_file "$project"
  done
  [[ -d "$REPO_ROOT/shared/core" && -d "$REPO_ROOT/shared/test-support" ]] || fail "shared source directories are missing or have incorrect case"
  validate_source_inventory
  [[ -d "$OF_ROOT/addons/ofxUnitTests/src" ]] || fail "pinned OF_ROOT is missing the shipped ofxUnitTests addon"
  info "host=$HOST_PLATFORM arch=$(uname -m)"
  info "OF_ROOT=$OF_ROOT openFrameworks=0.12.1"
  info "Project Generator=$PG version=0.103.0"
}

selected_projects() {
  if [[ "$PROJECT" == all ]]; then printf '%s\n' windowed unit; else printf '%s\n' "$PROJECT"; fi
}

clean_project() {
  local name=$1 path="$REPO_ROOT/foundation/$1"
  [[ "$path" == "$REPO_ROOT"/foundation/* && ! -L "$path" && ! -L "$path/bin" ]] || fail "unsafe project path: $path"
  if [[ -d "$path/bin" ]]; then
    find "$path/bin" -mindepth 1 -maxdepth 1 ! -name data -exec rm -rf -- {} +
  fi
  rm -rf -- "$path/Makefile" "$path/config.make" "$path/.vscode" "$path/$name.code-workspace" \
    "$path/$name.xcodeproj" "$path/Project.xcconfig" "$path/of.entitlements" "$path/openFrameworks-Info.plist" \
    "$path/$name.sln" "$path/$name.vcxproj" "$path/$name.vcxproj.filters" "$path/$name.vcxproj.user" \
    "$path/icon.rc" "$path/obj" "$path/bin/${name:?}" "$path/bin/${name:?}_debug" \
    "$path/bin/$name.app" "$path/bin/${name}Debug.app"
}

xcode_object_for_path() {
  local project_file=$1 relative=$2
  awk -v expected="$relative" '
    /^[[:space:]]*"[^"]+"[[:space:]]*:[[:space:]]*\{/ {
      object=$0
      sub(/^[[:space:]]*"/, "", object)
      sub(/".*/, "", object)
    }
    /^[[:space:]]*"path"[[:space:]]*:/ {
      value=$0
      sub(/^[^:]*:[[:space:]]*"/, "", value)
      sub(/",?[[:space:]]*$/, "", value)
      if (value == expected) print object
    }
  ' "$project_file"
}

assert_xcode_path_once() {
  local project_file=$1 relative=$2 references count
  references=$(xcode_object_for_path "$project_file" "$relative")
  if [[ -z "$references" ]]; then count=0; else count=$(printf '%s\n' "$references" | wc -l | tr -d ' '); fi
  [[ "$count" == 1 ]] || fail "Xcode metadata expected one path '$relative', observed $count"
}

assert_xcode_compiled_once() {
  local project_file=$1 relative=$2 file_reference build_references build_reference count
  file_reference=$(xcode_object_for_path "$project_file" "$relative")
  build_references=$(awk -v reference="$file_reference" '
    /^[[:space:]]*"[^"]+"[[:space:]]*:[[:space:]]*\{/ {
      object=$0
      sub(/^[[:space:]]*"/, "", object)
      sub(/".*/, "", object)
    }
    $0 ~ "\"fileRef\"[[:space:]]*:[[:space:]]*\"" reference "\"" { print object }
  ' "$project_file")
  if [[ -z "$build_references" ]]; then count=0; else count=$(printf '%s\n' "$build_references" | wc -l | tr -d ' '); fi
  [[ "$count" == 1 ]] || fail "Xcode metadata expected one build reference for '$relative', observed $count"
  build_reference=$build_references
  count=$(grep -Fc "\"$build_reference\"" "$project_file" || true)
  [[ "$count" == 2 ]] || fail "Xcode metadata did not include '$relative' exactly once in its sources phase"
}

validate_generated() {
  local name=$1 path="$REPO_ROOT/foundation/$1"
  case "$HOST_PLATFORM" in
    linux64)
      [[ -s "$path/Makefile" && -s "$path/config.make" ]] || fail "$name generation omitted Makefile or config.make"
      if [[ "$name" == unit ]]; then
        grep -Eq '^PROJECT_EXTERNAL_SOURCE_PATHS = ../../shared$' "$path/config.make" || fail "unit config.make omitted the single shared source root"
      else
        grep -Eq '^PROJECT_EXTERNAL_SOURCE_PATHS = ../../shared/core$' "$path/config.make" || fail "windowed config.make omitted shared/core"
      fi
      ;;
    osx)
      local project_file="$path/$name.xcodeproj/project.pbxproj" source
      [[ -s "$project_file" && -s "$path/Project.xcconfig" ]] || fail "$name generation omitted Xcode metadata"
      if [[ "$name" == windowed ]]; then
        for source in src/main.cpp src/ofApp.cpp src/ofApp.h ../../shared/core/course_probe.cpp ../../shared/core/course_probe.h; do
          assert_xcode_path_once "$project_file" "$source"
        done
        for source in src/main.cpp src/ofApp.cpp ../../shared/core/course_probe.cpp; do
          assert_xcode_compiled_once "$project_file" "$source"
        done
      else
        for source in src/main.cpp src/ofApp.cpp src/ofApp.h ../../shared/core/course_probe.cpp ../../shared/core/course_probe.h ../../shared/test-support/expect_near.h ../../shared/test-support/deterministic_fixture.h; do
          assert_xcode_path_once "$project_file" "$source"
        done
        for source in src/main.cpp src/ofApp.cpp ../../shared/core/course_probe.cpp; do
          assert_xcode_compiled_once "$project_file" "$source"
        done
      fi
      ;;
  esac
}

canonical_input_snapshot() {
  find "$REPO_ROOT/foundation/windowed/src" "$REPO_ROOT/foundation/windowed/bin/data" \
    "$REPO_ROOT/foundation/unit/src" "$REPO_ROOT/foundation/unit/bin/data" \
    "$REPO_ROOT/shared" -type f -print | LC_ALL=C sort | while IFS= read -r file; do cksum "$file"; done
  for file in "$REPO_ROOT"/foundation/{windowed,unit}/{addons.make,README.md}; do cksum "$file"; done
}

generate() {
  doctor
  mkdir -p "$REPO_ROOT/.harness/logs"
  local before after before_inputs after_inputs
  before=$(mktemp); after=$(mktemp); before_inputs=$(mktemp); after_inputs=$(mktemp)
  trap 'rm -f "$before" "$after" "$before_inputs" "$after_inputs"' RETURN
  git -C "$REPO_ROOT" diff --binary --no-ext-diff HEAD -- >"$before"
  canonical_input_snapshot >"$before_inputs"
  while IFS= read -r name; do
    clean_project "$name"
    local sources='../../shared/core'
    # OF 0.12.1 GNU Make cannot transform object paths for two external roots;
    # one shared root keeps both unit-only directories PG-owned and buildable.
    [[ "$name" == unit ]] && sources='../../shared'
    local log="$REPO_ROOT/.harness/logs/generate-$HOST_PLATFORM-$name.log"
    # A prior container/CI run can leave a root-owned file in a user-writable
    # ignored log directory. Replace the disposable file rather than failing to
    # truncate it in place.
    rm -f -- "$log" || fail "cannot replace generated log: $log"
    info "generating foundation/$name"
    set +e
    (cd "$REPO_ROOT" && PG_OF_PATH="$OF_ROOT" "$PG" -o"$OF_ROOT" -p"$HOST_PLATFORM" -s"$sources" "foundation/$name") >"$log" 2>&1
    local status=$?
    set -e
    [[ $status -eq 0 ]] || { cat "$log" >&2; fail "Project Generator exited $status for $name (log: $log)"; }
    if grep -Eiq '\[[[:space:]]*error[[:space:]]*\]|"errorMessage"[[:space:]]*:[[:space:]]*"[^"]+' "$log"; then
      cat "$log" >&2; fail "Project Generator reported an error for $name (log: $log)"
    fi
    validate_generated "$name"
    validate_source_inventory
  done < <(selected_projects)
  git -C "$REPO_ROOT" diff --binary --no-ext-diff HEAD -- >"$after"
  canonical_input_snapshot >"$after_inputs"
  if ! cmp -s "$before" "$after"; then
    diff -u "$before" "$after" >&2 || true
    fail "generation changed tracked files"
  fi
  if ! cmp -s "$before_inputs" "$after_inputs"; then
    diff -u "$before_inputs" "$after_inputs" >&2 || true
    fail "generation changed canonical project inputs"
  fi
  if git -C "$REPO_ROOT" grep -F "$OF_ROOT" -- foundation shared scripts tests .github >/dev/null; then fail "tracked implementation files embed the selected OF_ROOT"; fi
  info "generation passed for $PROJECT"
}

product_path() {
  local name=$1
  case "$HOST_PLATFORM:$CONFIGURATION" in
    linux64:Debug) printf '%s/foundation/%s/bin/%s_debug' "$REPO_ROOT" "$name" "$name" ;;
    linux64:Release) printf '%s/foundation/%s/bin/%s' "$REPO_ROOT" "$name" "$name" ;;
    osx:Debug) printf '%s/foundation/%s/bin/%sDebug.app' "$REPO_ROOT" "$name" "$name" ;;
    osx:Release) printf '%s/foundation/%s/bin/%s.app' "$REPO_ROOT" "$name" "$name" ;;
  esac
}

build() {
  doctor
  local jobs=2
  command -v getconf >/dev/null && jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf 2)
  while IFS= read -r name; do
    local path="$REPO_ROOT/foundation/$name"
    case "$HOST_PLATFORM" in
      linux64)
        command -v make >/dev/null || fail "make is required"
        [[ -s "$path/Makefile" ]] || fail "generate foundation/$name before build"
        (cd "$path" && make -j"$jobs" "$CONFIGURATION")
        ;;
      osx)
        command -v xcodebuild >/dev/null || fail "xcodebuild is required"
        [[ -d "$path/$name.xcodeproj" ]] || fail "generate foundation/$name before build"
        local scheme="$name $CONFIGURATION"
        [[ -f "$path/$name.xcodeproj/xcshareddata/xcschemes/$scheme.xcscheme" ]] || fail "generated Xcode scheme is missing: $scheme"
        xcodebuild -project "$path/$name.xcodeproj" -scheme "$scheme" -configuration "$CONFIGURATION" -sdk macosx
        ;;
    esac
    local product; product=$(product_path "$name")
    [[ -e "$product" ]] || fail "expected $CONFIGURATION product is missing: $product"
    info "built $product"
  done < <(selected_projects)
}

test_unit() {
  [[ "$PROJECT" == unit || "$PROJECT" == all ]] || fail "test only supports --project unit"
  doctor
  local product executable log="$REPO_ROOT/.harness/logs/test-$HOST_PLATFORM-$CONFIGURATION.log"
  mkdir -p "$(dirname "$log")"
  rm -f -- "$log" || fail "cannot replace unit log: $log"
  product=$(product_path unit)
  case "$HOST_PLATFORM" in
    linux64) executable=$product ;;
    osx) executable="$product/Contents/MacOS/$([[ "$CONFIGURATION" == Debug ]] && printf unitDebug || printf unit)" ;;
  esac
  [[ -x "$executable" ]] || fail "build the unit $CONFIGURATION product before test: $executable"
  set +e
  (cd "$REPO_ROOT/foundation/unit/bin" && "$executable") >"$log" 2>&1
  local status=$?
  set -e
  cat "$log"
  [[ $status -eq 0 ]] || fail "unit executable returned $status"
  local summaries summary_lines
  summary_lines=$(grep -Eo '[0-9]+/[0-9]+ tests passed' "$log" || true)
  if [[ -z "$summary_lines" ]]; then summaries=0; else summaries=$(printf '%s\n' "$summary_lines" | wc -l | tr -d ' '); fi
  [[ "$summaries" == 1 ]] || fail "expected one unit summary, observed $summaries"
  grep -Fq "$EXPECTED_TESTS/$EXPECTED_TESTS tests passed" "$log" || fail "expected exactly $EXPECTED_TESTS discovered passing tests"
  ! grep -Eq '[0-9]+/[0-9]+ tests failed|\[[[:space:]]*error[[:space:]]*\]' "$log" || fail "unit log contains a failure diagnostic"
  info "unit runtime passed with exactly $EXPECTED_TESTS tests"
}

case "$COMMAND" in
  doctor) doctor ;;
  generate) generate ;;
  build) build ;;
  test) test_unit ;;
  *) usage; fail "unknown command: $COMMAND" ;;
esac
