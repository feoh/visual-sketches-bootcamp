# Technical, mathematical, accessibility, originality, and license review

This is a release gate and evidence ledger, not a claim that every manual check
has happened. Automated checks may prefill only automated rows. Graphical,
device, assistive-technology, and learner behavior remain `not observed` until
a named reviewer performs them.

## Review record rules

Each finding receives an ID, commit, section/file, evidence class (`automated`,
`manual graphical/device`, or `human learner`), severity, owner, and evidence.
Use statuses `open`, `local-pass-awaiting-commit`,
`fixed-awaiting-verification`, `verified`, `accepted-risk`, or `not observed`.
`local-pass-awaiting-commit` records reproducible working-tree evidence that is
not yet an immutable release result. `accepted-risk` requires rationale and
release owner.
Block release for any open critical/high finding, an unknown required-asset
license, or a false platform/accessibility/learner claim.

## 1. Automated intake

- [ ] Record commit and run `scripts/check-authoring.sh --require-hugo` (or the
      PowerShell equivalent); attach full output and Hugo version.
- [ ] Run every `tests/run-section-NN-tests.*` and the first-C++ test variants;
      attach command, platform/toolchain, exit code, and concise result.
- [ ] Link claimed native generation/build/test CI statuses by OS and commit.
- [ ] Run `scripts/build-site.*`; confirm expected section count, navigation,
      stylesheet, media, and rewritten repository links in generated HTML.
- [ ] Confirm working tree/staged state so evidence corresponds to the commit.
- [ ] Inventory every `authoring/**/assets.yaml` record and any runtime asset
      outside authoring; reject a missing creator, source, license, alt, caption,
      or required motion companion.

Automated passing output does not complete the manual checklists below.

## 2. Technical review (per section)

- [ ] Published commands match tracked paths and separate generation, build,
      test, and launch; destructive actions are containment checked.
- [ ] Public signatures, fixtures, starter, tests, solution explanation, and
      lesson wording agree.
- [ ] Known case, boundaries, invalid/non-finite inputs, invariants, limits, and
      deterministic replay policy are tested where applicable.
- [ ] Time units, coordinate spaces, random seed scope, numeric tolerances,
      overflow/index bounds, and fallback state are explicit.
- [ ] Pure model logic does not depend on wall time, global randomness, live
      devices, window creation, or rendering when a deterministic seam is
      claimed.
- [ ] openFrameworks adapter builds only on platforms evidenced for the exact
      commit; unsupported combinations are labeled.
- [ ] A reviewer launches starter and solution, resizes, pauses/resets, exercises
      controls, removes/denies optional devices/assets, and records observed
      rendering separately from test output.
- [ ] Errors are visible/actionable and no exercise requires a hidden global
      dependency, system font, network call, or untracked generated file.

## 3. Mathematical review (per section)

- [ ] Every symbol is defined with units/domain before use; vector/scalar and
      degrees/radians distinctions are explicit.
- [ ] Visual, numeric, and symbolic representations agree on one known case.
- [ ] Recompute all displayed examples independently and compare with tests.
- [ ] Check endpoints, inclusive/exclusive intervals, zero-length vectors,
      normalization, wrap/clamp/map ordering, coordinate origin/orientation,
      interpolation convention, and division-by-zero policy where relevant.
- [ ] State discrete/continuous assumptions, fixed/variable timestep behavior,
      floating-point tolerance, and reproducibility limits honestly.
- [ ] Confirm diagrams do not use spacing, scale, color, or arrow direction that
      contradicts the formula or numeric example.
- [ ] Confirm test properties encode the stated claim rather than one accidental
      implementation, and counterexamples fail the test.
- [ ] Record reviewer calculation or script output; “looks correct” is not
      mathematical evidence.

## 4. Accessibility review

### Document and site

- [ ] Heading order and landmark names are meaningful; no lesson relies on
      visual position alone for structure.
- [ ] All images have task-relevant alt text and adjacent visible captions;
      decorative images are explicitly marked. Animation/video has a poster,
      transcript, and reduced-motion alternative.
- [ ] Link text identifies purpose out of context; code, tables, and long URLs
      reflow or scroll without hiding adjacent content.
- [ ] Keyboard-only traversal has visible focus, working skip link, logical
      order, and no trap at 100% and 200% zoom.
- [ ] At 320 CSS pixels, content reflows without two-dimensional page scrolling
      except data/code regions; text remains usable at 200%.
- [ ] Text/focus/UI contrast and non-color cues are measured, not eyeballed;
      record tool, colors, ratios, and applicable threshold.
- [ ] Screen-reader check records browser, reader/version, headings, landmarks,
      lists, images, tables, code, and previous/next navigation.
- [ ] Forced-colors/high-contrast, dark mode, print, and prefers-reduced-motion
      receive dated manual checks on claimed platforms.

### Sketch interaction

- [ ] Pointer or device input has a documented keyboard/non-device fallback;
      controls are reachable and described.
- [ ] Motion can pause/reduce, does not flash unsafely, and fallback still
      communicates state. Record frame/device observation; pure tests cannot
      prove this.
- [ ] Meaning is available without color alone; labels, shapes, position, line
      style, or text provide the redundant cue.
- [ ] Missing audio/image/device permission produces a visible, usable fallback
      without exposing private input or requiring participation.
- [ ] Captions/transcripts describe timing and meaningful visual/audio changes.

## 5. Originality review

Originality is mechanism-level documentation, not screenshot similarity or a
score for taste.

- [ ] List precedents with creator, title, source URL, access date, and the
      transferable principle studied.
- [ ] Identify copied/adapted code, data, media, phrase, font, algorithm, and
      tutorial structure; link the license review IDs.
- [ ] State learner/author intent and at least three deliberate changes among
      geometry, density, palette, input, mapping, interaction, motion, and time.
- [ ] Compare source and result at code/process/mechanism level. A divergent
      palette alone is insufficient; a similar still alone is not proof of
      copying.
- [ ] Check that reference solutions are explained examples, not prescribed
      target screenshots, and synthesis work has no canonical finished answer.
- [ ] Retain only process material the learner wants saved: sketches, parameter
      notes, commits, tests, captures, and attribution decisions.
- [ ] Escalate confusing similarity, unattributed borrowing, or incompatible
      terms to a human release owner; do not auto-adjudicate authorship.

## 6. License and third-party asset policy

Repository licensing is dual-scope: CC BY 4.0 for original curriculum prose and
media, and MIT for source code/tests/scripts/supporting software. `LICENSE` is
the authoritative scope notice; `LICENSES/` contains the license texts.

- [ ] Every original file falls unambiguously under one listed scope or carries
      its own header/notice.
- [ ] Do not label upstream documentation, linked examples, learner submissions,
      fonts, libraries, openFrameworks, or third-party assets as repository-owned
      or relicense them under CC BY/MIT.
- [ ] For each bundled third-party item, record creator, canonical source,
      exact license/version, download/access date, modifications, required
      attribution, and redistribution/source obligations before merge.
- [ ] Verify terms from the license text or authoritative rights-holder page;
      “found online,” search snippets, and absent notices are not permission.
- [ ] Reject unknown terms, no-derivatives assets that must be adapted,
      noncommercial assets for unrestricted distribution, and dependencies whose
      obligations cannot be met by the delivery format.
- [ ] Preserve copyright, attribution, license, modification, source-offer, and
      notice requirements in repository and generated/downloaded output.
- [ ] Check asset bytes against the record and confirm every copy (starter,
      solution, fixture, site output) retains applicable terms.
- [ ] Treat public-domain/CC0 claims as records requiring source and provenance.
- [ ] Record code dependency/addon licenses separately from media licenses and
      verify binary/source redistribution obligations.
- [ ] Before accepting learner work for publication, obtain explicit publishing
      permission and asset/license attestations; course participation alone is
      not permission.

## Review evidence ledger

Add one row per run or finding. Do not erase superseded evidence.

| ID | Date | Commit | Area | Section/file | Evidence class | Severity | Command/device/method | Status | Evidence/result | Owner / reviewer | Follow-up |
|---|---|---|---|---|---|---|---|---|---|---|---|
| BASELINE-AUTO | 2026-08-18 | repository baseline | intake | repository | automated | high | not run by this document | not observed | Populate from a reproducible validation run. | unassigned | Run automated intake. |
| BASELINE-GRAPHICS | 2026-08-18 | repository baseline | technical/accessibility | complete curriculum | manual graphical/device | high | no observation recorded | not observed | No cross-browser, assistive-technology, sketch rendering, or device result is claimed. | unassigned | Assign platform reviewers. |
| BASELINE-LEARNER | 2026-08-18 | repository baseline | pilot | complete curriculum | human learner | high | no pilot recorded | not observed | See pilot protocol; no learner result is claimed. | unassigned | Start an informal self-pilot or shared-learner run with Unit 0. |
| DELIVERY-AUTO-001 | 2026-08-18 | working tree based on `5f314af` | intake | sections 00–17 and optional site | automated | high | required Hugo authoring check, publication build, focused GCC/Clang suites, ShellCheck, YAML parse | local-pass-awaiting-commit | Authoring/publication emitted all 19 non-draft bundles; focused 07/09/11/14/15 suites passed with GCC and Clang. This is not commit-addressed release evidence. | implementation review | Record the delivered commit and obtain three-platform CI. |
| DELIVERY-TECH-014 | 2026-08-18 | working tree based on `5f314af` | technical/math | section 14 | automated | medium | mutation tests and adapter source contract | fixed-awaiting-verification | Explicit overlarge status, bounded design validation, transactional model tests, and visible adapter errors pass pure C++ checks. | implementation review | Compile/launch both native adapters on supported platforms. |
| DELIVERY-SEED-009 | 2026-08-18 | working tree based on `5f314af` | technical/math | section 09 | automated | medium | zero/former-sentinel regression under GCC and Clang | local-pass-awaiting-commit | Full-state generator preserves zero without the former sentinel alias; deterministic replay remains green. | implementation review | Retain regression in platform CI. |
| DELIVERY-A11Y-015 | 2026-08-18 | working tree based on `5f314af` | accessibility | section 15 | automated | high | adapter source contract | fixed-awaiting-verification | Both adapters contain a bounded tiny-window route with source and N stop/fallback guidance. | implementation review | Manually inspect contrast, clipping, keyboard behavior, and live stream stop. |
| DELIVERY-LICENSE-001 | 2026-08-18 | working tree based on `5f314af` | license/originality | repository and sections 08/09/11/13/14/15 | automated | high | file/record audit | fixed-awaiting-verification | Dual-scope license, CC0 fixture provenance, BSL notice/text, and lesson-level precedent-transfer guidance are present. | implementation review | Release owner verifies notice/legal scope and collaborator credits before publication. |
| DELIVERY-AUTO-002 | 2026-08-19 | working tree based on `5afa25b` | intake | sections 00–17, pilot protocol, and optional site | automated | high | required Hugo check; 21 negative authoring contracts; publication build; every section 00–15 and first-C++ starter/solution under GCC 16 and Clang 22; ShellCheck | local-pass-awaiting-commit | All commands passed locally; Hugo emitted 19 lessons and the checked sibling synthesis link. PowerShell and native OF builds were not run locally. | delivery implementation review | Commit the revision, obtain Windows contract and three-platform native CI, then record immutable run URLs. |
| DELIVERY-CI-001 | 2026-08-19 | `5afa25b7905f4e7001dbea4ea0286944398f959e` | intake | authoring workflow | automated | high | [GitHub Actions run 32200320574](https://github.com/feoh/visual-sketches-bootcamp/actions/runs/32200320574), including one failed-jobs rerun | fixed-awaiting-verification | POSIX and Windows contracts plus 47 of 48 native section jobs passed. Section 03 Linux failed only in the dependency installer after its 10-minute timeout interrupted `dpkg`; generation/build never ran. The working tree lengthens the bounded attempt and repairs `dpkg` before retry. | delivery implementation review | Verify the workflow repair on an immutable commit; do not call current delivery-head CI fully green. |
| DELIVERY-CACHYOS-001 | 2026-08-18 | working tree based on `64da096` | platform/tooling | all POSIX scripts and native projects | automated | high | `setup-linux.sh` contract/doctor; ShellCheck of 42 POSIX scripts; foundation doctor/generate/Debug/Release/unit; 32 section projects generated and built Debug/Release; all deterministic tests with GCC 16 and Clang 22; Hugo/21 negative authoring/publication checks | local-pass-awaiting-commit | A repository-owned CachyOS/Arch path now avoids apt, omits unused OpenCV 5 integration, installs the pacman/paru dependency plan, rebuilds PG for host libraries, and forces the X11 GLFW backend required by OF 0.12.1. All native wrapper builds and deterministic contracts passed on the dated CachyOS snapshot. Root-owned disposable logs and authoring fixtures copying ignored native outputs were also made recoverable. | delivery implementation review | Commit and obtain Ubuntu/macOS/Windows CI; retain CachyOS as a dated rolling-snapshot claim distinct from Ubuntu evidence. |
| DELIVERY-CACHYOS-MANUAL-001 | 2026-08-18 | working tree based on `64da096` | platform/tooling | foundation and section 00 | manual graphical/device | high | two five-second runtime probes with rendering and resize observation | local-pass-awaiting-commit | The author confirmed foundation and section-00 rendering and resize after the patch. This was a technical smoke check, not a learner working through the curriculum. | delivery implementation review | Keep this separate from later self-pilot or shared-learner notes. |
| DELIVERY-CACHYOS-REVIEW-001 | 2026-08-18 | working tree based on `64da096` | platform/tooling | CachyOS/Arch setup and script hardening diff | automated independent review | high | fresh-context reviewer inspected the full diff, requested conservative distro detection and stronger AUR/PG/GLFW assertions, then re-reviewed the repairs | reviewed-awaiting-commit | Debian is explicitly rejected rather than treated as Ubuntu; missing FreeImage exercises `paru`; native PG replacement and exact guarded GLFW hint are asserted. The follow-up review reported no remaining blocker, high, or medium findings. | independent implementation reviewer | Preserve contract tests in CI and obtain immutable multi-platform results after commit. |
| DELIVERY-PAGES-001 | 2026-08-19 | `92d40aa0755a84185b72346ec120d8646dc4df3c` | intake | optional Hugo publication | automated | high | [GitHub Actions run 32212993599](https://github.com/feoh/visual-sketches-bootcamp/actions/runs/32212993599); HTTP smoke against <https://feoh.github.io/visual-sketches-bootcamp/> | fixed-awaiting-verification | The `pages` workflow ran `scripts/build-site.sh` (full authoring contract plus fatal-warning Hugo build) and deployed from `main`. Home, course index, stylesheet, representative bundle media, and all 19 non-draft lessons return 200; repository-file links, sibling navigation, skip link, and `main`/`nav` landmarks are correct at the deployed base URL. | delivery implementation review | This is transport and link evidence only. It does not satisfy the manual keyboard, screen-reader, 200%/320px reflow, dark-mode, or print rows of the publication readiness gate. |

## Release sign-off

- [ ] All expected sections have technical and math reviewers distinct from the
      author where practical.
- [ ] Accessibility includes both document/site and sketch interaction evidence.
- [ ] Originality and license rows identify every precedent and bundled asset.
- [ ] Critical/high findings are `verified` or explicitly `accepted-risk` by the
      release owner; unknown license findings cannot be accepted as risk.
- [ ] The release summary separates automated, manual graphical/device, and
      human-learner evidence and lists every `not observed` category.
- [ ] Sign-off records release owner, date, commit, base URL/artifact checksum,
      and residual risks.
