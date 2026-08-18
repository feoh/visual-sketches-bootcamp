#!/bin/sh
set -eu

if [ "$#" -ne 0 ]; then
  printf '%s\n' 'usage: scripts/build-site.sh' >&2
  exit 2
fi

script_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
root=$(CDPATH='' cd -- "$script_dir/.." && pwd -P)
destination="$root/.hugo-public"

"$root/scripts/check-authoring.sh" --require-hugo
hugo --source "$root" \
  --config site/hugo.toml \
  --destination "$destination" \
  --cleanDestinationDir \
  --panicOnWarning \
  --printPathWarnings
printf 'publication build: %s\n' "$destination"
