#!/usr/bin/env bash
set -euo pipefail

COMMAND=""
OF_ROOT_ARG=""
DISTRO=auto
ASSUME_YES=0
DRY_RUN=0
OS_RELEASE_FILE=${COURSE_OS_RELEASE_FILE:-/etc/os-release}

fail() { printf 'setup-linux: ERROR: %s\n' "$*" >&2; exit 1; }
info() { printf 'setup-linux: %s\n' "$*"; }
usage() {
  cat <<'USAGE'
Usage: scripts/setup-linux.sh <doctor|prepare|install> [options]
  --of-root PATH                    openFrameworks 0.12.1 root (else OF_ROOT)
  --distro auto|cachyos|arch|ubuntu override /etc/os-release detection
  --yes                             pass noninteractive confirmation to package tools
  --dry-run                         print package/preparation commands without running them

Commands:
  doctor   Check the distro, required commands/packages, patched framework, and Project Generator.
  prepare  Patch and rebuild the verified OF 0.12.1 Linux archive for CachyOS/Arch compatibility.
  install  Install distro dependencies, then prepare and run doctor.
USAGE
}

[[ $(uname -s) == Linux ]] || fail 'this wrapper supports Linux only'
[[ $# -gt 0 ]] || { usage; exit 2; }
COMMAND=$1
shift
while [[ $# -gt 0 ]]; do
  case "$1" in
    --of-root) [[ $# -ge 2 ]] || fail '--of-root requires a path'; OF_ROOT_ARG=$2; shift 2 ;;
    --distro) [[ $# -ge 2 ]] || fail '--distro requires a value'; DISTRO=$2; shift 2 ;;
    --yes) ASSUME_YES=1; shift ;;
    --dry-run) DRY_RUN=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *) fail "unknown argument: $1" ;;
  esac
done
[[ "$COMMAND" == doctor || "$COMMAND" == prepare || "$COMMAND" == install ]] || { usage >&2; fail "unknown command: $COMMAND"; }
[[ "$DISTRO" == auto || "$DISTRO" == cachyos || "$DISTRO" == arch || "$DISTRO" == ubuntu ]] || fail 'distro must be auto, cachyos, arch, or ubuntu'

resolve_distro() {
  local detected_id="" detected_like=""
  if [[ "$DISTRO" == auto ]]; then
    [[ -r "$OS_RELEASE_FILE" ]] || fail "cannot read OS release metadata: $OS_RELEASE_FILE"
    # /etc/os-release is a trusted, distribution-owned shell assignment file.
    # shellcheck disable=SC1090
    . "$OS_RELEASE_FILE"
    detected_id=${ID:-}
    detected_like=${ID_LIKE:-}
    case "$detected_id" in
      cachyos) DISTRO=cachyos ;;
      arch) DISTRO=arch ;;
      ubuntu) DISTRO=ubuntu ;;
      *)
        case " $detected_like " in
          *' cachyos '*) DISTRO=cachyos ;;
          *' arch '*) DISTRO=arch ;;
          *' ubuntu '*) DISTRO=ubuntu ;;
          *) fail "unsupported Linux distribution ID=${detected_id:-unknown} ID_LIKE=${detected_like:-unknown}; use --distro only for a verified compatible derivative" ;;
        esac
        ;;
    esac
  fi
  case "$DISTRO" in
    cachyos|arch) DISTRO_FAMILY=arch ;;
    ubuntu) DISTRO_FAMILY=debian ;;
  esac
  info "distribution=$DISTRO family=$DISTRO_FAMILY"
}

resolve_of_root() {
  local candidate=${OF_ROOT_ARG:-${OF_ROOT:-}}
  [[ -n "$candidate" ]] || fail 'set OF_ROOT or pass --of-root PATH'
  [[ -d "$candidate" ]] || fail "openFrameworks root does not exist: $candidate"
  OF_ROOT=$(cd "$candidate" && pwd -P)
  local required
  for required in addons libs scripts projectGenerator apps/projectGenerator/commandLine; do
    [[ -d "$OF_ROOT/$required" ]] || fail "OF_ROOT is missing $required/: $OF_ROOT"
  done
  local constants="$OF_ROOT/libs/openFrameworks/utils/ofConstants.h" version
  [[ -f "$constants" ]] || fail "OF_ROOT is missing $constants"
  version=$(awk '$1=="#define"&&$2~/^OF_VERSION_(MAJOR|MINOR|PATCH)$/{v[$2]=$3} END{print v["OF_VERSION_MAJOR"] "." v["OF_VERSION_MINOR"] "." v["OF_VERSION_PATCH"]}' "$constants")
  [[ "$version" == 0.12.1 ]] || fail "expected openFrameworks 0.12.1, observed ${version:-unknown}"
}

print_command() {
  printf 'setup-linux: would run:'
  printf ' %q' "$@"
  printf '\n'
}

run_command() {
  if (( DRY_RUN )); then print_command "$@"; else "$@"; fi
}

run_as_root() {
  if (( EUID == 0 )); then
    run_command "$@"
  else
    command -v sudo >/dev/null || fail 'sudo is required to install system packages'
    run_command sudo "$@"
  fi
}

arch_packages() {
  local glfw_package=glfw-x11
  [[ "$DISTRO" == cachyos ]] && glfw_package=glfw
  printf '%s\n' \
    base-devel bash brotli curl assimp fontconfig freeglut freetype2 gcc git glew \
    "$glfw_package" gstreamer gst-libav gst-plugins-bad gst-plugins-base \
    gst-plugins-good libxcursor libxi libxinerama libxrandr openal poco \
    pkgconf pugixml rtaudio tar uriparser xorg-xwayland
}

install_arch_dependencies() {
  command -v pacman >/dev/null || fail 'pacman is required for CachyOS/Arch installation'
  mapfile -t packages < <(arch_packages)
  local pacman_args=(-S --needed)
  (( ASSUME_YES )) && pacman_args+=(--noconfirm)
  run_as_root pacman "${pacman_args[@]}" "${packages[@]}"

  # FreeImage moved from the Arch repositories to the AUR, but OF 0.12.1 links
  # it directly. CachyOS ships paru; keep the AUR review interactive unless the
  # caller explicitly requested --yes.
  if (( DRY_RUN )) || ! pacman -T freeimage >/dev/null 2>&1; then
    (( EUID != 0 || DRY_RUN )) || fail 'FreeImage is absent; rerun this repository script as your user (without sudo) so paru can build it safely'
    command -v paru >/dev/null || fail 'FreeImage is required but absent; install the AUR package freeimage (CachyOS provides paru)'
    local paru_args=(-S --needed)
    (( ASSUME_YES )) && paru_args+=(--noconfirm)
    run_command paru "${paru_args[@]}" freeimage
  fi
}

install_debian_dependencies() {
  local installer="$OF_ROOT/scripts/linux/ubuntu/install_dependencies.sh"
  [[ -x "$installer" ]] || fail "upstream Ubuntu dependency installer is missing: $installer"
  local args=("$installer")
  (( ASSUME_YES )) && args+=(-y)
  run_as_root "${args[@]}"
}

insert_algorithm_include() {
  local source="$OF_ROOT/apps/projectGenerator/commandLine/src/utils/LibraryBinary.cpp"
  [[ -f "$source" ]] || fail "Project Generator source is missing: $source"
  grep -Fqx '#include <algorithm>' "$source" && return 0
  (( DRY_RUN )) && { info "would add <algorithm> to $source for current GCC/libstdc++"; return 0; }
  [[ -w "$source" ]] || fail "OF_ROOT is not writable by $(id -un): $source"
  local temporary
  temporary=$(mktemp "${source}.XXXXXX")
  awk '
    { print }
    !done && $0 == "#include \"ofFileUtils.h\"" {
      print ""
      print "#include <algorithm>"
      done=1
    }
    END { if (!done) exit 1 }
  ' "$source" >"$temporary" || { rm -f "$temporary"; fail 'could not locate the Project Generator include anchor'; }
  chmod --reference="$source" "$temporary"
  mv -- "$temporary" "$source"
  info 'patched Project Generator source for GCC/libstdc++ compatibility'
}

insert_glfw_x11_hint() {
  local source="$OF_ROOT/libs/openFrameworks/app/ofAppGLFWWindow.cpp"
  local marker='COURSE_CACHYOS_GLFW_X11'
  [[ -f "$source" ]] || fail "openFrameworks GLFW source is missing: $source"
  grep -Fq "$marker" "$source" && return 0
  (( DRY_RUN )) && { info "would force GLFW's X11 backend in $source"; return 0; }
  [[ -w "$source" ]] || fail "OF_ROOT is not writable by $(id -un): $source"
  local temporary
  temporary=$(mktemp "${source}.XXXXXX")
  awk '
    !done && /^[[:space:]]*if \(!glfwInit\(\)\) \{/ {
      print "#if defined(TARGET_LINUX) && defined(GLFW_PLATFORM) && defined(GLFW_PLATFORM_X11)"
      print "        // COURSE_CACHYOS_GLFW_X11: OF 0.12.1 uses X11-native input/icon APIs."
      print "        // GLFW 3.4+ dual-backend builds otherwise prefer Wayland and pass null X11 handles."
      print "        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);"
      print "#endif"
      done=1
    }
    { print }
    END { if (!done) exit 1 }
  ' "$source" >"$temporary" || { rm -f "$temporary"; fail 'could not locate the GLFW initialization anchor'; }
  chmod --reference="$source" "$temporary"
  mv -- "$temporary" "$source"
  info 'patched openFrameworks 0.12.1 to use GLFW X11 under dual-backend GLFW'
}

build_jobs() {
  local jobs=${COURSE_BUILD_JOBS:-}
  if [[ -z "$jobs" ]]; then
    jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf 2)
    (( jobs > 8 )) && jobs=8
  fi
  [[ "$jobs" =~ ^[1-9][0-9]*$ ]] || fail "COURSE_BUILD_JOBS must be a positive integer, observed: $jobs"
  printf '%s\n' "$jobs"
}

prepare_arch_framework() {
  insert_algorithm_include
  insert_glfw_x11_hint
  local jobs
  jobs=$(build_jobs)
  local compiled="$OF_ROOT/libs/openFrameworksCompiled/project"
  local pg_source="$OF_ROOT/apps/projectGenerator/commandLine"
  local pg_built="$pg_source/bin/projectGenerator"
  local pg_target="$OF_ROOT/projectGenerator/resources/app/app/projectGenerator"
  [[ -f "$compiled/Makefile" && -f "$pg_source/Makefile" ]] || fail 'openFrameworks build metadata is incomplete'

  info "building patched openFrameworks Debug and Release libraries with $jobs jobs"
  run_command make -C "$compiled" -j"$jobs" Debug
  run_command make -C "$compiled" -j"$jobs" Release
  info 'building a native Project Generator for the host libraries'
  run_command make -C "$pg_source" -j"$jobs" Release
  if (( DRY_RUN )); then
    print_command install -m 0755 "$pg_built" "$pg_target"
  else
    [[ -x "$pg_built" ]] || fail "native Project Generator build omitted $pg_built"
    install -m 0755 "$pg_built" "$pg_target"
  fi
}

check_commands() {
  local missing=() command
  for command in awk bash c++ cksum cmp curl diff find git grep make sed tar; do
    command -v "$command" >/dev/null 2>&1 || missing+=("$command")
  done
  ((${#missing[@]} == 0)) || fail "missing required commands: ${missing[*]}"
}

check_arch_packages() {
  command -v pacman >/dev/null || fail 'pacman is required on CachyOS/Arch'
  mapfile -t packages < <(arch_packages)
  local missing=() package
  for package in "${packages[@]}" freeimage; do
    # base-devel is a group, not a dependency name accepted by pacman -T.
    [[ "$package" == base-devel ]] && continue
    pacman -T "$package" >/dev/null 2>&1 || missing+=("$package")
  done
  ((${#missing[@]} == 0)) || fail "missing CachyOS/Arch packages: ${missing[*]}; run install"
}

doctor() {
  check_commands
  [[ "$DISTRO_FAMILY" == arch ]] && check_arch_packages
  local pg="$OF_ROOT/projectGenerator/resources/app/app/projectGenerator" output
  [[ -x "$pg" ]] || fail "Project Generator is missing or not executable: $pg"
  output=$({ "$pg" --version; } 2>&1) || fail "Project Generator could not run against host libraries: $output"
  grep -Eq '(^|[^0-9])0\.103\.0([^0-9]|$)' <<<"$output" || fail "expected Project Generator 0.103.0, observed: $output"
  if [[ "$DISTRO_FAMILY" == arch ]]; then
    grep -Fq '#include <algorithm>' "$OF_ROOT/apps/projectGenerator/commandLine/src/utils/LibraryBinary.cpp" || fail 'Project Generator GCC compatibility patch is absent; run prepare'
    grep -Fq 'COURSE_CACHYOS_GLFW_X11' "$OF_ROOT/libs/openFrameworks/app/ofAppGLFWWindow.cpp" || fail 'GLFW dual-backend compatibility patch is absent; run prepare'
  fi
  info 'doctor passed: openFrameworks=0.12.1 Project Generator=0.103.0'
  if [[ "$DISTRO_FAMILY" == arch ]] && pkg-config --exists opencv5 2>/dev/null; then
    info 'OpenCV 5 detected; this curriculum does not enable ofxOpenCv, whose upstream 0.12.1 metadata only handles OpenCV 3/4'
  fi
}

resolve_distro
resolve_of_root
case "$COMMAND" in
  doctor) doctor ;;
  prepare)
    [[ "$DISTRO_FAMILY" == arch ]] && prepare_arch_framework
    if (( DRY_RUN )); then info 'dry run complete; no framework files were changed'; else doctor; fi
    ;;
  install)
    if [[ "$DISTRO_FAMILY" == arch ]]; then install_arch_dependencies; else install_debian_dependencies; fi
    [[ "$DISTRO_FAMILY" == arch ]] && prepare_arch_framework
    if (( DRY_RUN )); then info 'dry run complete; no packages or framework files were changed'; else doctor; fi
    ;;
esac
