#!/usr/bin/env bash
set -euo pipefail

fail() { printf 'setup-of: ERROR: %s\n' "$*" >&2; exit 1; }
platform=""
destination="${PWD}/.openframeworks"
while [[ $# -gt 0 ]]; do
  case "$1" in
    --platform) [[ $# -ge 2 ]] || fail "--platform requires linux64 or osx"; platform=$2; shift 2 ;;
    --destination) [[ $# -ge 2 ]] || fail "--destination requires a path"; destination=$2; shift 2 ;;
    *) fail "unknown argument: $1" ;;
  esac
done
[[ "$platform" == linux64 || "$platform" == osx ]] || fail "--platform must be linux64 or osx"
case "$platform" in
  linux64)
    asset=of_v0.12.1_linux64_gcc6_release.tar.gz
    root=of_v0.12.1_linux64_gcc6_release
    sha=d6c1dcab777665b2aa63e5e3d9122cc116f096b3421db3493f795a621b399c63 ;;
  osx)
    asset=of_v0.12.1_osx_release.tar.gz
    root=of_v0.12.1_osx_release
    sha=e4a2dd6e75805320aac905f9f82f7b85e67a8b799137d47f35d40f7c4c195c9b ;;
esac
mkdir -p "$destination"
destination=$(cd "$destination" && pwd -P)
archive="$destination/$asset"
url="https://github.com/openframeworks/openFrameworks/releases/download/0.12.1/$asset"
if [[ ! -f "$archive" ]]; then
  command -v curl >/dev/null || fail "curl is required"
  curl --fail --location --retry 3 --output "$archive" "$url"
fi
if command -v sha256sum >/dev/null; then
  printf '%s  %s\n' "$sha" "$archive" | sha256sum --check --status || fail "openFrameworks archive checksum mismatch: $archive"
elif command -v shasum >/dev/null; then
  observed=$(shasum -a 256 "$archive" | awk '{print $1}')
  [[ "$observed" == "$sha" ]] || fail "openFrameworks archive checksum mismatch: $archive"
else
  fail "sha256sum or shasum is required"
fi

# Never trust a previously extracted tree. Extract the verified archive into a
# fresh sibling, validate its identity, then replace the selected root.
staging=$(mktemp -d "$destination/.of-extract.XXXXXX")
backup="$destination/.${root}.backup.$$"
cleanup() { rm -rf -- "$staging"; }
trap cleanup EXIT
tar -xzf "$archive" -C "$staging"
staged_root="$staging/$root"
for required in addons libs scripts; do
  [[ -d "$staged_root/$required" ]] || fail "verified archive omitted $root/$required"
done
constants="$staged_root/libs/openFrameworks/utils/ofConstants.h"
[[ -f "$constants" ]] || fail "verified archive omitted openFrameworks version constants"
version=$(awk '$1=="#define" && $2~/^OF_VERSION_(MAJOR|MINOR|PATCH)$/ {value[$2]=$3} END {print value["OF_VERSION_MAJOR"] "." value["OF_VERSION_MINOR"] "." value["OF_VERSION_PATCH"]}' "$constants")
[[ "$version" == 0.12.1 ]] || fail "verified archive contains openFrameworks $version, expected 0.12.1"

installed="$destination/$root"
[[ ! -e "$backup" && ! -L "$backup" ]] || fail "refusing unexpected backup path: $backup"
if [[ -e "$installed" || -L "$installed" ]]; then
  mv -- "$installed" "$backup"
fi
if ! mv -- "$staged_root" "$installed"; then
  if [[ -e "$backup" || -L "$backup" ]]; then
    mv -- "$backup" "$installed" || fail "install failed and the prior tree remains at $backup"
  fi
  fail "could not install freshly verified openFrameworks tree"
fi
rm -rf -- "$backup"
printf 'OF_ROOT=%s\n' "$installed"
