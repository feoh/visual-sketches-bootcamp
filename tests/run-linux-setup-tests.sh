#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/linux-setup-tests.XXXXXX")
trap 'rm -rf "$WORK"' EXIT
FAKE_OF="$WORK/of_v0.12.1_linux64_gcc6_release"
MOCK_BIN="$WORK/bin"
LOG="$WORK/commands.log"
mkdir -p \
  "$MOCK_BIN" \
  "$FAKE_OF/addons" \
  "$FAKE_OF/scripts/linux/ubuntu" \
  "$FAKE_OF/projectGenerator/resources/app/app" \
  "$FAKE_OF/apps/projectGenerator/commandLine/src/utils" \
  "$FAKE_OF/apps/projectGenerator/commandLine/bin" \
  "$FAKE_OF/libs/openFrameworks/utils" \
  "$FAKE_OF/libs/openFrameworks/app" \
  "$FAKE_OF/libs/openFrameworksCompiled/project"

cat >"$FAKE_OF/libs/openFrameworks/utils/ofConstants.h" <<'EOF'
#define OF_VERSION_MAJOR 0
#define OF_VERSION_MINOR 12
#define OF_VERSION_PATCH 1
EOF
cat >"$FAKE_OF/apps/projectGenerator/commandLine/src/utils/LibraryBinary.cpp" <<'EOF'
#include "LibraryBinary.h"
#include "ofFileUtils.h"

const int probe = 1;
EOF
cat >"$FAKE_OF/libs/openFrameworks/app/ofAppGLFWWindow.cpp" <<'EOF'
void ofAppGLFWWindow::setup() {
        if (!glfwInit()) {
                return;
        }
}
EOF
: >"$FAKE_OF/apps/projectGenerator/commandLine/Makefile"
: >"$FAKE_OF/libs/openFrameworksCompiled/project/Makefile"
cat >"$FAKE_OF/projectGenerator/resources/app/app/projectGenerator" <<'EOF'
#!/usr/bin/env sh
echo 'packaged projectGenerator 0.103.0'
EOF
chmod +x "$FAKE_OF/projectGenerator/resources/app/app/projectGenerator"
cat >"$FAKE_OF/scripts/linux/ubuntu/install_dependencies.sh" <<'EOF'
#!/usr/bin/env sh
exit 0
EOF
chmod +x "$FAKE_OF/scripts/linux/ubuntu/install_dependencies.sh"

cat >"$MOCK_BIN/hugo" <<'EOF'
#!/usr/bin/env sh
exit 0
EOF
cat >"$MOCK_BIN/sudo" <<'EOF'
#!/usr/bin/env sh
printf 'sudo' >>"$COURSE_TEST_LOG"
printf ' %s' "$@" >>"$COURSE_TEST_LOG"
printf '\n' >>"$COURSE_TEST_LOG"
"$@"
EOF
cat >"$MOCK_BIN/pacman" <<'EOF'
#!/usr/bin/env sh
printf 'pacman' >>"$COURSE_TEST_LOG"
printf ' %s' "$@" >>"$COURSE_TEST_LOG"
printf '\n' >>"$COURSE_TEST_LOG"
if [ "${1-}" = -T ]; then
  if [ "${2-}" = freeimage ] && [ ! -f "$COURSE_TEST_FREEIMAGE_INSTALLED" ]; then exit 1; fi
  exit 0
fi
exit 0
EOF
cat >"$MOCK_BIN/paru" <<'EOF'
#!/usr/bin/env sh
printf 'paru' >>"$COURSE_TEST_LOG"
printf ' %s' "$@" >>"$COURSE_TEST_LOG"
printf '\n' >>"$COURSE_TEST_LOG"
touch "$COURSE_TEST_FREEIMAGE_INSTALLED"
exit 0
EOF
cat >"$MOCK_BIN/make" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
printf 'make' >>"$COURSE_TEST_LOG"
printf ' %s' "$@" >>"$COURSE_TEST_LOG"
printf '\n' >>"$COURSE_TEST_LOG"
directory=""
while (($#)); do
  if [[ $1 == -C ]]; then directory=$2; shift 2; else shift; fi
done
if [[ $directory == */apps/projectGenerator/commandLine ]]; then
  mkdir -p "$directory/bin"
  cat >"$directory/bin/projectGenerator" <<'PG'
#!/usr/bin/env sh
echo 'native projectGenerator 0.103.0'
PG
  chmod +x "$directory/bin/projectGenerator"
fi
EOF
chmod +x "$MOCK_BIN"/*

cat >"$WORK/cachyos-release" <<'EOF'
ID=cachyos
ID_LIKE=arch
EOF
cat >"$WORK/ubuntu-release" <<'EOF'
ID=ubuntu
ID_LIKE=debian
EOF
cat >"$WORK/debian-release" <<'EOF'
ID=debian
ID_LIKE=debian
EOF
cat >"$WORK/unknown-release" <<'EOF'
ID=unknown
EOF

export PATH="$MOCK_BIN:$PATH"
export COURSE_TEST_LOG="$LOG"
export COURSE_TEST_FREEIMAGE_INSTALLED="$WORK/freeimage-installed"
export COURSE_OS_RELEASE_FILE="$WORK/cachyos-release"
export COURSE_BUILD_JOBS=3

"$ROOT/scripts/setup-linux.sh" install --of-root "$FAKE_OF" --yes

grep -Fq 'pacman -S --needed --noconfirm' "$LOG"
grep -Fq ' glfw ' "$LOG"
grep -Fq ' xorg-xwayland' "$LOG"
grep -Fq 'paru -S --needed --noconfirm freeimage' "$LOG"
if grep -Fq ' opencv' "$LOG"; then echo 'CachyOS plan unexpectedly installed unsupported OpenCV for this curriculum' >&2; exit 1; fi
if grep -Eq 'apt-get|ubuntu/install_dependencies' "$LOG"; then echo 'CachyOS plan unexpectedly selected an apt/Ubuntu installer' >&2; exit 1; fi
grep -Fq "make -C $FAKE_OF/libs/openFrameworksCompiled/project -j3 Debug" "$LOG"
grep -Fq "make -C $FAKE_OF/libs/openFrameworksCompiled/project -j3 Release" "$LOG"
grep -Fq "make -C $FAKE_OF/apps/projectGenerator/commandLine -j3 Release" "$LOG"
[[ $(grep -Fxc '#include <algorithm>' "$FAKE_OF/apps/projectGenerator/commandLine/src/utils/LibraryBinary.cpp") == 1 ]]
[[ $(grep -Fc 'COURSE_CACHYOS_GLFW_X11' "$FAKE_OF/libs/openFrameworks/app/ofAppGLFWWindow.cpp") == 1 ]]
grep -Fq '#if defined(TARGET_LINUX) && defined(GLFW_PLATFORM) && defined(GLFW_PLATFORM_X11)' "$FAKE_OF/libs/openFrameworks/app/ofAppGLFWWindow.cpp"
grep -Fq 'glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);' "$FAKE_OF/libs/openFrameworks/app/ofAppGLFWWindow.cpp"
[[ $("$FAKE_OF/projectGenerator/resources/app/app/projectGenerator" --version) == 'native projectGenerator 0.103.0' ]]

# Preparation is safe to rerun and does not duplicate either source patch.
"$ROOT/scripts/setup-linux.sh" prepare --of-root "$FAKE_OF"
[[ $(grep -Fxc '#include <algorithm>' "$FAKE_OF/apps/projectGenerator/commandLine/src/utils/LibraryBinary.cpp") == 1 ]]
[[ $(grep -Fc 'COURSE_CACHYOS_GLFW_X11' "$FAKE_OF/libs/openFrameworks/app/ofAppGLFWWindow.cpp") == 1 ]]

# Ubuntu selects the upstream installer path and does not apply Arch package logic.
export COURSE_OS_RELEASE_FILE="$WORK/ubuntu-release"
: >"$LOG"
"$ROOT/scripts/setup-linux.sh" install --of-root "$FAKE_OF" --yes
grep -Fq "$FAKE_OF/scripts/linux/ubuntu/install_dependencies.sh -y" "$LOG"
if grep -Fq 'pacman -S' "$LOG"; then echo 'Ubuntu plan unexpectedly selected pacman' >&2; exit 1; fi

# Debian must not be mistaken for Ubuntu merely because both are Debian-family.
export COURSE_OS_RELEASE_FILE="$WORK/debian-release"
if "$ROOT/scripts/setup-linux.sh" doctor --of-root "$FAKE_OF" >"$WORK/debian.log" 2>&1; then
  echo 'Debian unexpectedly selected the Ubuntu installer contract' >&2
  exit 1
fi
grep -Fq 'unsupported Linux distribution ID=debian ID_LIKE=debian' "$WORK/debian.log"

# Other unsupported distributions fail with a useful diagnostic.
export COURSE_OS_RELEASE_FILE="$WORK/unknown-release"
if "$ROOT/scripts/setup-linux.sh" doctor --of-root "$FAKE_OF" >"$WORK/unknown.log" 2>&1; then
  echo 'unsupported distribution unexpectedly passed' >&2
  exit 1
fi
grep -Fq 'unsupported Linux distribution' "$WORK/unknown.log"

printf '%s\n' 'linux setup tests: CachyOS package/AUR plan, idempotent guarded OF patches, native PG replacement, Ubuntu selection, and Debian/unsupported rejection passed'
