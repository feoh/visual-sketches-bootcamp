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
`--cleanDestinationDir` removes stale output. Only the `pages` workflow below
publishes the result; no script in this repository deploys anything.

## GitHub Pages deployment

The site is published at <https://feoh.github.io/visual-sketches-bootcamp/>,
which is also the `baseURL` in `site/hugo.toml`, so local and deployed builds
emit identical links. The path segment must stay equal to the repository name;
the authoring checker asserts it.

`.github/workflows/pages.yml` builds and deploys:

- It runs `scripts/build-site.sh`, so a deployment cannot skip the authoring
  contract, the Hugo fixtures, or the fatal-warning publication build.
- The build job holds only `contents: read`. Only the deploy job receives
  `pages: write` and `id-token: write`, and it runs no repository code.
- Deployment is guarded on `refs/heads/main`, so a `workflow_dispatch` from a
  branch builds but does not publish.
- Repository Pages source is `build_type: workflow`; no `gh-pages` branch
  exists and nothing commits generated output.

### Rollback and disablement

- Republish a known-good commit: re-run that commit's `pages` workflow run, or
  dispatch `pages` from `main` after reverting.
- Stop publishing without losing history: disable the `pages` workflow in the
  Actions tab. The last deployment stays live.
- Take the site down: `gh api -X DELETE repos/feoh/visual-sketches-bootcamp/pages`.
  This removes the public site; re-enable with a `POST` of
  `{"build_type":"workflow"}` and a workflow run.

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
- [ ] Learner outcomes are labeled `not yet observed` until an actual self-pilot
      or shared-learner run is entered in `docs/pilot-protocol-and-evidence.md`.

## Licensing in generated pages

Original curriculum prose and media use CC BY 4.0; source code and supporting
software use MIT. `LICENSE` defines the exact file scopes and exclusions.
Third-party works keep their own terms and are not relicensed. A publisher must
preserve nearby asset records, attribution files, and notices when distributing
generated pages or downloadable project files.
