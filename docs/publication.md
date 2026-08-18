# Optional Hugo publication

The canonical course is the portable Markdown under `authoring/sections/`.
Hugo is an optional renderer, not an authoring requirement: the site mounts
those leaf bundles directly and does not copy or rewrite lesson prose. The same
relative links remain usable in Obsidian and ordinary Markdown viewers.

## Local build

Install Hugo Extended 0.164.0 or newer, then run from the repository root:

```sh
scripts/build-site.sh
```

On PowerShell:

```powershell
./scripts/build-site.ps1
```

The scripts first run the complete authoring contract with Hugo required. They
then create `.hugo-public/`, replacing only that ignored directory. For a
one-off destination or base URL, run Hugo directly:

```sh
hugo --source . --config site/hugo.toml \
  --destination /tmp/visual-sketches-site \
  --baseURL https://example.invalid/visual-sketches-bootcamp/ \
  --cleanDestinationDir --panicOnWarning --printPathWarnings
```

Do not point `--destination` at a directory containing anything you need:
`--cleanDestinationDir` removes stale output. No command in this repository
pushes, deploys, or publishes the result.

## What the layer does

- Hugo module mounts map `authoring/sections/` to `content/course/`, while
  layouts and CSS remain under `site/`.
- Bundle media is emitted with each lesson. Render hooks turn lesson links to
  repository files into GitHub source links; external citations remain direct.
- The authoring checker rejects missing local Markdown targets, unsafe paths,
  unrecorded media, and incomplete citation/license records before Hugo runs.
- Hugo warnings are fatal in the checked build. The checker confirms every
  non-draft section slug, representative media, navigation, and stylesheet in
  generated output.
- The layouts provide semantic landmarks, a keyboard skip link, visible focus,
  lesson objectives/prerequisites, previous/next navigation, responsive media,
  dark mode, print styles, and reduced-motion handling. No JavaScript or
  external theme is required.

## Publication readiness gate

A successful build is **automated evidence only**. Before public release, the
release owner records all of the following:

- [ ] Exact commit, Hugo version, command, base URL, and build log.
- [ ] Manual keyboard and screen-reader navigation at home, one early lesson,
      one late lesson, and a lesson with the widest table/code block.
- [ ] Manual 200%, 320 CSS-pixel reflow, dark-mode, reduced-motion, and print
      checks in the browsers/platforms being claimed.
- [ ] Manual checks that representative images, captions, transcripts, exercise
      repository links, and external citations work at the deployed base URL.
- [ ] The review ledger in `docs/review-process.md` has no blocking `open` item.
- [ ] License notices are present and every bundled third-party asset has a
      compatible record and retained notice.
- [ ] Any platform claim is supported by that platform’s CI/manual evidence;
      a local Hugo build does not prove native openFrameworks behavior.
- [ ] Human-learner outcomes are labeled `not yet observed` until an actual
      consented pilot is entered in `docs/pilot-protocol-and-evidence.md`.

## Licensing in generated pages

Original curriculum prose and media use CC BY 4.0; source code and supporting
software use MIT. `LICENSE` defines the exact file scopes and exclusions.
Third-party works keep their own terms and are not relicensed. A publisher must
preserve nearby asset records, attribution files, and notices when distributing
generated pages or downloadable project files.
