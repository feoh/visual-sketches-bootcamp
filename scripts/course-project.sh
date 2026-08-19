#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
[[ $# -gt 0 ]] || { echo "course-project: missing section" >&2; exit 2; }
SECTION=$1; shift
case "$SECTION" in
  00) EXERCISE=00-visual-signature; DESIGN=signature_design; SHARED=signature_geometry ;;
  01) EXERCISE=01-a-mark-that-moves; DESIGN=traveler_design; SHARED=traveler_model ;;
  02) EXERCISE=02-python-to-cpp-survival-kit; DESIGN=family_design; SHARED=mark_family ;;
  03) EXERCISE=03-map-clamp-and-lerp; DESIGN=poster_design; SHARED=poster_layout ;;
  04) EXERCISE=04-vectors-direction-and-distance; DESIGN=constellation_design; SHARED=constellation_model ;;
  05) EXERCISE=05-oscillation-circles-and-phase; DESIGN=phase_field_design; SHARED=phase_field_model ;;
  06) EXERCISE=06-controlled-chance; DESIGN=edition_design; SHARED=edition_model ;;
  07) EXERCISE=07-local-coordinate-systems; DESIGN=sculpture_design; SHARED=sculpture_model ;;
  08) EXERCISE=08-gesture-as-geometry; DESIGN=gesture_design; SHARED=gesture_model ;;
  09) EXERCISE=09-particles-with-memory; DESIGN=particle_design; SHARED=particle_model ;;
  10) EXERCISE=10-forces-steering-and-springs; DESIGN=force_design; SHARED=force_model ;;
  11) EXERCISE=11-noise-and-flow-fields; DESIGN=flow_field_design; SHARED=flow_field_model ;;
  12) EXERCISE=12-color-blending-and-trails; DESIGN=trail_design; SHARED=color_trail_model ;;
  13) EXERCISE=13-time-as-a-drawable-axis; DESIGN=temporal_design; SHARED=temporal_history ;;
  14) EXERCISE=14-images-and-type-as-geometry; DESIGN=image_geometry_design; SHARED=image_geometry_model ;;
  15) EXERCISE=15-embodied-audio-input; DESIGN=audio_instrument_design; SHARED=audio_input_model ;;
  *) echo "course-project: unsupported section $SECTION" >&2; exit 2 ;;
esac
LABEL="section-$SECTION"
OF_ROOT_ARG=""
PROJECT=starter
CONFIGURATION=Release
fail() { printf '%s: ERROR: %s\n' "$LABEL" "$*" >&2; exit 1; }
usage() { printf 'usage: scripts/section-%s.sh <doctor|generate|build> [--of-root PATH] [--project starter|solution] [--configuration Debug|Release]\n' "$SECTION"; }
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

validate_source_inventory() {
  local expected actual source_root="$ROOT/exercises/$EXERCISE"
  expected=$(mktemp); actual=$(mktemp)
  cat >"$expected" <<EOF
exercises/$EXERCISE/$PROJECT/src/main.cpp
exercises/$EXERCISE/$PROJECT/src/ofApp.cpp
exercises/$EXERCISE/$PROJECT/src/ofApp.h
exercises/$EXERCISE/$PROJECT/src/design/$DESIGN.cpp
exercises/$EXERCISE/$PROJECT/src/design/$DESIGN.h
exercises/$EXERCISE/shared/$SHARED.cpp
exercises/$EXERCISE/shared/$SHARED.h
EOF
  find "$PROJECT_PATH/src" "$source_root/shared" -type f \
    \( -iname '*.c' -o -iname '*.cc' -o -iname '*.cpp' -o -iname '*.cxx' -o \
       -iname '*.m' -o -iname '*.mm' -o -iname '*.h' -o -iname '*.hh' -o \
       -iname '*.hpp' -o -iname '*.hxx' \) -print |
    while IFS= read -r file; do printf '%s\n' "${file#"$ROOT"/}"; done >"$actual"
  LC_ALL=C sort -o "$expected" "$expected"; LC_ALL=C sort -o "$actual" "$actual"
  if ! cmp -s "$expected" "$actual"; then
    diff -u "$expected" "$actual" >&2 || true
    rm -f "$expected" "$actual"; fail 'course source inventory has missing or stale files'
  fi
  rm -f "$expected" "$actual"
}

resolve() {
  local candidate=${OF_ROOT_ARG:-${OF_ROOT:-}}
  [[ -n "$candidate" && -d "$candidate" ]] || fail 'set OF_ROOT or pass --of-root PATH'
  OF_ROOT=$(cd "$candidate" && pwd -P)
  case "$(uname -s):$(uname -m)" in
    Linux:x86_64) PLATFORM=linux64; PG="$OF_ROOT/projectGenerator/resources/app/app/projectGenerator" ;;
    Darwin:arm64) PLATFORM=osx; PG="$OF_ROOT/projectGenerator/projectGenerator.app/Contents/Resources/app/app/projectGenerator" ;;
    *) fail "unsupported host $(uname -s) $(uname -m); use the matching PowerShell wrapper on Windows" ;;
  esac
  [[ -x "$PG" ]] || fail "packaged Project Generator is missing: $PG"
}

validate_of_tree() {
  local template="scripts/templates/$PLATFORM" required
  for required in \
    addons \
    libs/openFrameworks \
    libs/openFrameworksCompiled \
    scripts \
    scripts/templates \
    "$template"; do
    [[ -d "$OF_ROOT/$required" ]] || fail "openFrameworks directory is missing: $required"
  done
  for required in \
    libs/openFrameworks/utils/ofConstants.h \
    libs/openFrameworks/ofMain.h \
    libs/openFrameworks/app/ofAppRunner.h \
    "$template/config.make"; do
    [[ -f "$OF_ROOT/$required" ]] || fail "openFrameworks file is missing: $required"
  done
  [[ -x "$PG" ]] || fail "packaged Project Generator is missing: $PG"
}

doctor() {
  resolve
  validate_of_tree
  local constants="$OF_ROOT/libs/openFrameworks/utils/ofConstants.h" version pg_version
  version=$(awk '$1=="#define"&&$2~/^OF_VERSION_(MAJOR|MINOR|PATCH)$/{v[$2]=$3} END{print v["OF_VERSION_MAJOR"] "." v["OF_VERSION_MINOR"] "." v["OF_VERSION_PATCH"]}' "$constants")
  [[ "$version" == 0.12.1 ]] || fail "expected openFrameworks 0.12.1, observed $version"
  pg_version=$({ "$PG" --version; } 2>&1) || fail "Project Generator could not run: $pg_version"
  grep -Eq '(^|[^0-9])0\.103\.0([^0-9]|$)' <<<"$pg_version" || fail "expected Project Generator 0.103.0, observed $pg_version"
  [[ -d "$PROJECT_PATH/src" && -f "$PROJECT_PATH/addons.make" ]] || fail "missing canonical project inputs for $PROJECT"
  [[ ! -s "$PROJECT_PATH/addons.make" ]] || fail "$LABEL projects must not enable addons"
  [[ -f "$ROOT/exercises/$EXERCISE/shared/$SHARED.cpp" ]] || fail "shared model source is missing"
  validate_source_inventory
  printf '%s: host=%s project=%s OF=%s PG=0.103.0\n' "$LABEL" "$PLATFORM" "$PROJECT" "$version"
}

clean_generated() {
  local resolved_project
  resolved_project=$(cd "$PROJECT_PATH" && pwd -P) || fail 'canonical project directory is missing'
  [[ "$resolved_project" == "$PROJECT_PATH" &&
     "$PROJECT_PATH" == "$ROOT/exercises/$EXERCISE/"* &&
     ! -L "$PROJECT_PATH" && ! -L "$PROJECT_PATH/bin" ]] || fail 'unsafe project path'
  if [[ -d "$PROJECT_PATH/bin" ]]; then find "$PROJECT_PATH/bin" -mindepth 1 -maxdepth 1 ! -name data -exec rm -rf -- {} +; fi
  rm -rf -- "$PROJECT_PATH/Makefile" "$PROJECT_PATH/config.make" "$PROJECT_PATH/.vscode" "$PROJECT_PATH/$PROJECT.code-workspace" \
    "$PROJECT_PATH/$PROJECT.xcodeproj" "$PROJECT_PATH/Project.xcconfig" "$PROJECT_PATH/of.entitlements" \
    "$PROJECT_PATH/openFrameworks-Info.plist" "$PROJECT_PATH/obj" "$PROJECT_PATH/$PROJECT.app" "$PROJECT_PATH/${PROJECT}Debug.app"
}

xcode_object_for_path() {
  local project_file=$1 relative=$2
  awk -v expected="$relative" '
    /^[[:space:]]*"[^"]+"[[:space:]]*:[[:space:]]*\{/ {
      object=$0; sub(/^[[:space:]]*"/, "", object); sub(/".*/, "", object)
    }
    /^[[:space:]]*"path"[[:space:]]*:/ {
      value=$0; sub(/^[^:]*:[[:space:]]*"/, "", value); sub(/",?[[:space:]]*$/, "", value)
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
      object=$0; sub(/^[[:space:]]*"/, "", object); sub(/".*/, "", object)
    }
    $0 ~ "\"fileRef\"[[:space:]]*:[[:space:]]*\"" reference "\"" { print object }
  ' "$project_file")
  if [[ -z "$build_references" ]]; then count=0; else count=$(printf '%s\n' "$build_references" | wc -l | tr -d ' '); fi
  [[ "$count" == 1 ]] || fail "Xcode metadata expected one build reference for '$relative', observed $count"
  build_reference=$build_references
  count=$(grep -Fc "\"$build_reference\"" "$project_file" || true)
  [[ "$count" == 2 ]] || fail "Xcode metadata did not compile '$relative' exactly once"
}

validate_xcode_membership() {
  local project_file=$1 source expected actual
  expected=$(mktemp); actual=$(mktemp)
  cat >"$expected" <<EOF
../shared/$SHARED.cpp
../shared/$SHARED.h
src/design/$DESIGN.cpp
src/design/$DESIGN.h
src/main.cpp
src/ofApp.cpp
src/ofApp.h
EOF
  awk '
    /^[[:space:]]*"path"[[:space:]]*:/ {
      value=$0; sub(/^[^:]*:[[:space:]]*"/, "", value); sub(/",?[[:space:]]*$/, "", value)
      lower=tolower(value)
      if (lower ~ /^(src|\.\.\/shared)\/.*\.(c|cc|cpp|cxx|m|mm|h|hh|hpp|hxx)$/) print value
    }
  ' "$project_file" >"$actual"
  LC_ALL=C sort -o "$expected" "$expected"; LC_ALL=C sort -o "$actual" "$actual"
  if ! cmp -s "$expected" "$actual"; then
    diff -u "$expected" "$actual" >&2 || true
    rm -f "$expected" "$actual"; fail 'Xcode metadata has missing, duplicate, or stale course sources'
  fi
  while IFS= read -r source; do assert_xcode_path_once "$project_file" "$source"; done <"$expected"
  for source in src/main.cpp src/ofApp.cpp src/design/$DESIGN.cpp ../shared/$SHARED.cpp; do
    assert_xcode_compiled_once "$project_file" "$source"
  done
  rm -f "$expected" "$actual"
}

ensure_microphone_privacy_description() {
  [[ "$SECTION" == 15 && "$PLATFORM" == osx ]] || return 0
  local plist="$PROJECT_PATH/openFrameworks-Info.plist"
  local buddy=/usr/libexec/PlistBuddy
  local purpose='This sketch uses microphone amplitude only after the learner presses L; it stores no audio.'
  [[ -f "$plist" && -x "$buddy" ]] || fail 'macOS microphone privacy metadata tools are missing'
  if "$buddy" -c 'Print :NSMicrophoneUsageDescription' "$plist" >/dev/null 2>&1; then
    "$buddy" -c "Set :NSMicrophoneUsageDescription '$purpose'" "$plist"
  else
    "$buddy" -c "Add :NSMicrophoneUsageDescription string '$purpose'" "$plist"
  fi
  "$buddy" -c 'Print :NSMicrophoneUsageDescription' "$plist" | grep -Fq 'stores no audio' ||
    fail 'macOS microphone privacy description was not written'
}

validate_generated() {
  local external_root project_file
  case "$PLATFORM" in
    linux64)
      [[ -s "$PROJECT_PATH/Makefile" && -s "$PROJECT_PATH/config.make" ]] || fail 'generator omitted Makefile or config.make'
      [[ $(grep -Ec '^PROJECT_EXTERNAL_SOURCE_PATHS = ../shared$' "$PROJECT_PATH/config.make") == 1 ]] || fail 'config.make must contain exactly one ../shared source root'
      external_root=$(cd "$PROJECT_PATH/../shared" && pwd -P)
      [[ "$external_root" == "$ROOT/exercises/$EXERCISE/shared" ]] || fail 'generated external source root escaped the exercise shared directory'
      ;;
    osx)
      project_file="$PROJECT_PATH/$PROJECT.xcodeproj/project.pbxproj"
      [[ -s "$project_file" && -s "$PROJECT_PATH/Project.xcconfig" ]] || fail 'generator omitted Xcode metadata'
      validate_xcode_membership "$project_file"
      ;;
  esac
  validate_source_inventory
}

canonical_snapshot() {
  find "$PROJECT_PATH/src" "$PROJECT_PATH/bin/data" "$ROOT/exercises/$EXERCISE/shared" -type f -print | LC_ALL=C sort | while IFS= read -r file; do cksum "$file"; done
  cksum "$PROJECT_PATH/addons.make"
}

generate() {
  doctor
  local before after before_inputs after_inputs log="$ROOT/.harness/logs/generate-$PLATFORM-$LABEL-$PROJECT.log"
  before=$(mktemp); after=$(mktemp); before_inputs=$(mktemp); after_inputs=$(mktemp)
  trap 'rm -f "$before" "$after" "$before_inputs" "$after_inputs"' RETURN
  git -C "$ROOT" diff --binary --no-ext-diff HEAD -- >"$before"
  canonical_snapshot >"$before_inputs"
  clean_generated; mkdir -p "$(dirname "$log")"
  # A prior container/CI run can leave a root-owned file in a user-writable
  # ignored log directory. Replace the disposable file rather than failing to
  # truncate it in place.
  rm -f -- "$log" || fail "cannot replace generated log: $log"
  local sources='../shared'
  [[ "$PLATFORM" == osx ]] && sources='src/design,../shared'
  set +e
  (cd "$ROOT" && PG_OF_PATH="$OF_ROOT" "$PG" -o"$OF_ROOT" -p"$PLATFORM" -s"$sources" "exercises/$EXERCISE/$PROJECT") >"$log" 2>&1
  status=$?; set -e
  [[ $status -eq 0 ]] || { cat "$log" >&2; fail "Project Generator exited $status"; }
  ! grep -Eiq '\[[[:space:]]*error[[:space:]]*\]' "$log" || { cat "$log" >&2; fail 'Project Generator reported an error'; }
  ensure_microphone_privacy_description
  validate_generated
  git -C "$ROOT" diff --binary --no-ext-diff HEAD -- >"$after"
  canonical_snapshot >"$after_inputs"
  cmp -s "$before" "$after" || fail 'generation changed tracked files'
  cmp -s "$before_inputs" "$after_inputs" || fail 'generation changed canonical project inputs'
  printf '%s: generated %s for %s\n' "$LABEL" "$PROJECT" "$PLATFORM"
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
  printf '%s: built %s\n' "$LABEL" "$product"
}

case "$COMMAND" in doctor) doctor ;; generate) generate ;; build) build ;; *) usage; exit 2 ;; esac
