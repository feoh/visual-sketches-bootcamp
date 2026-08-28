# Portable course authoring

This directory defines the course's source format without committing to a Hugo
theme. Copy a directory from `templates/`, keep the filenames and relative
layout, and replace its example values. `examples/instructional/` is a complete
fixture used by the smoke checks. Published instructional sources live under
`sections/`; the checker builds them with the templates and validates links
into `exercises/`.

## Portable Markdown contract

- Instructional and synthesis pages are Hugo leaf bundles named `index.md`.
- Use conservative YAML front matter and standard Markdown only. Do not use
  Hugo shortcodes, Obsidian wikilinks, embeds, or plugin syntax.
- Write every local destination relative to the source file using POSIX `/`
  separators: `media/still.svg`, `../../../docs/testing-strategy.md`, never a
  repository-root `/path`, backslash, or `file:` URL.
- Keep one inline Markdown link or image per source line. Reference-style
  images are deliberately unsupported. This makes the dependency-free checker
  intentionally predictable. Fragments and external `https:`, `http:`, and
  `mailto:` destinations are allowed.
- Give local files lowercase, hyphenated names. Case matters on Linux.
- Keep lesson media in the leaf bundle's `media/` directory. Exercise links may
  point source-file-relatively into `exercises/`.
- Use fenced code blocks with a language name. Use ordinary Markdown links so
  both Hugo and Obsidian can follow them.

## Front matter

Every leaf bundle declares `title`, `slug`, `weight`, `draft`, `course_kind`,
`objectives`, `prerequisites`, `source_records`, and `asset_records`.
`course_kind` is `instructional` or `synthesis`. Record fields contain the relative
POSIX paths `sources.yaml` and `assets.yaml`; keep the records beside
`index.md`.

## Citations and assets

`sources.yaml` records each source's stable local `id`, title, creator, URL,
access date (`YYYY-MM-DD`), and a note explaining what claim or technique it
supports. Prefer primary and creator-owned sources. Every record URL must be a
normal prose link, and every external prose link that is not an asset must have
one source record.

`assets.yaml` records every bundled or externally linked visual/audio asset.
Each record includes `id`, `kind`, `creator`, `source`, `license`, `alt`, and
`caption`, plus exactly one of `path` (a bundle-relative file under `media/`)
or `url` (an external HTTP(S) image). Every file under `media/` has exactly one
record, including transcripts and companion stills. Every external Markdown
image has a matching URL record. `source: original` is valid, but `creator` and
`license` are still required. Do not add an asset whose redistribution terms
are unknown.

Static images require meaningful alt text (or `alt: decorative` only when they
convey no information) and a visible caption. Do not begin alt text with
“image of”; state the information a learner needs.

Animation and video records additionally require companion asset IDs:

- `poster`: the ID of a still that communicates the important state;
- `transcript`: the ID of a text record describing sequence, timing, and
  meaningful changes; and
- `reduced_motion`: the ID of a still or non-animated alternative.

Link the poster and transcript next to the animation. Captions and transcripts
must explain information that color alone would otherwise carry. A companion
made by another creator or under another license gets its own asset record.
Video with speech also needs captions in the delivered media; the text
transcript remains required.

## Voice and math starting line

Write to a smart beginner who wants to make something. Basic algebra is the math
starting point: substituting values, reading a graph, and solving small
equations. Do not assume trigonometry or familiarity with names
such as sine, cosine, radians, phase, or `atan2`; introduce each one before the
learner needs it. Explain the picture and the useful intuition first, then show
the formula and code. Define unavoidable jargon in the sentence where it
appears.

Keep the voice conversational and encouraging. Prefer “let's try this” to
“the learner shall implement this.” A small joke is welcome when it clarifies
rather than distracts. Technical instructions still need to be precise, but
they should sound like help from a friendly guide.

## Lesson → Practice → Exercise structure

Every instructional section has exactly three learner-facing phases, in this
order:

1. **Lesson** explains one coherent idea with a visual, ordinary language, a
   worked numerical example, and only the C++ needed for that idea.
2. **Practice** guides the reader through small changes, predictions, builds,
   and repairs. Practice may use compiler or runtime feedback, but it does not
   run the section's unit-test suite.
3. **Exercise** poses one problem against a focused starter with explicit
   change points. The section's unit tests run here and check observable attributes that
   indicate correctness. Visual quality and accessibility remain a manual
   review because tests should not grade taste.

Use H2 headings named exactly `Lesson`, `Practice`, and `Exercise`. Put supporting
subsections beneath them as H3 headings. Keep the exercise brief authoritative
for its files, commands, constraints, and test contract; the lesson page should
summarize and link rather than invent a second exercise.

## Definition of done: instructional section

An instructional section is **written and ready to try** when it has:

- 3–6 observable objectives, prerequisites, and an accessible visual preview;
- the Lesson → Practice → Exercise structure;
- one math idea shown visually, numerically, and symbolically;
- no more than one substantial new C++ mechanism;
- two or three small guided practice steps and one deliberate repair task;
- a focused starter with explicit change points, deterministic tests, fixtures where needed, and an explained
  instructional reference solution;
- exact commands for the checked platforms and friendly guidance for adapting
  them elsewhere;
- known-case, boundary, and property tests plus a practical visual and access
  self-check;
- complete source and asset records; and
- a short troubleshooting note that points the reader toward the smallest
  useful example, test, or number to inspect when something goes sideways.

A section is ready when a reader can follow it, make a sketch, and recover from
ordinary mistakes without filling out a research form. Automated checks verify
the code and links; the reader's own visual choices remain their business.

## Ready-to-use synthesis project

A synthesis project is ready when it offers a clear aim, gentle scope limits,
repeatable example inputs, accessibility and license guidance, lightweight
optional note templates, and several visibly different unfinished examples. It
has no official finished solution. Its self-checks help the reader understand
their own intent and reusable techniques rather than imitate one reference
image.

## Checks

From the repository root:

```sh
scripts/check-authoring.sh
scripts/check-authoring.sh --require-hugo
```

On Windows PowerShell:

```powershell
./scripts/check-authoring.ps1
./scripts/check-authoring.ps1 -RequireHugo
```

Both commands validate front matter, portable links, local targets, media
companions, and required citation/attribution metadata. If Hugo is installed,
they also build the complete example in an isolated temporary site with a tiny
generated layout. `--require-hugo`/`-RequireHugo` fails rather than skipping
that build when Hugo is unavailable. No generated site files enter the
repository.

The POSIX negative suite creates an escaping symlink and forces the conservative
no-`realpath` fallback. The Windows suite does not create symlinks because
hosted-runner link privileges vary; the PowerShell validator still resolves and
contains links that already exist.
