# Complete-curriculum pilot protocol and evidence ledger

**Protocol version:** 1.1

**Last updated:** 2026-08-19
**Route definitions:** [`curriculum-map.md#provisional-course-routes`](curriculum-map.md#provisional-course-routes)

A session sheet records this protocol version, an exact curriculum commit, and
one route ID: `complete-18`, `core-12`, `accelerated-8-plus-2`, or a fully listed
custom route. Route names are planning labels, not completion or pacing claims.

## Evidence status at repository baseline

**Human-learner evidence: not yet observed.** No completion, pacing, learning,
accessibility, setup-success, or satisfaction result is claimed here. Existing
CI and deterministic tests are automated engineering evidence; author review of
a graphical sketch or device fallback is manual technical evidence. Neither is
a learner pilot.

## Evidence classes

Use exactly one class on every ledger entry:

1. **Automated** — reproducible command output: authoring validation, Hugo build,
   deterministic C++ tests, native generation/build/test status, or link check.
2. **Manual graphical/device** — dated observation of rendering, interaction,
   audio/input hardware, keyboard fallback, resize, reduced motion, or assistive
   technology on a named platform/device.
3. **Human learner** — observation or report from a consented participant
   attempting the curriculum. Do not infer this class from CI, author
   walkthroughs, or issue comments.

Allowed evidence result values are `pass`, `fail`, `blocked`, and `not
observed`. Never replace a missing observation with `pass`.

## Outcome and intervention codebook

Use one eventual outcome and one first-attempt outcome for each lesson:

| Code | Meaning |
|---|---|
| `IC` | Completed independently with no facilitator intervention after the task began. |
| `NP` | Completed after a neutral prompt that restated the goal or asked what evidence was available without naming a procedure. |
| `PA` | Completed after procedural assistance such as locating a command, file, or toolchain setting. |
| `SA` | Completed after solution-revealing assistance that exposed an implementation or answer. |
| `PT` | Partial: produced usable evidence for some but not all required completion components. |
| `BI` | Blocked by instruction, ambiguity, missing prerequisite, or curriculum defect. |
| `BE` | Blocked by environment, platform, device, or accessibility barrier. |
| `SK` | Skipped by route design or facilitator decision. |
| `WD` | Participant paused or withdrew; do not infer a reason. |

The first attempt ends at the earliest of a facilitator intervention, the
learner requesting procedural/solution help, or the learner declaring that they
cannot continue. Before assistance, record `IC` only for completion; otherwise
record `PT`, `BI`, or `BE` according to observed progress/cause. For the eventual
code, incomplete/blocking states (`WD`, `BI`, `BE`, then `PT`) take precedence;
a completed lesson uses the greatest assistance received in the order `SA` >
`PA` > `NP` > `IC`. `SK` is valid only for a section omitted by the recorded
route before work begins. Do not replace a cause with a guess: use `PT` plus a
note when `BI` versus `BE` is genuinely unknown.

Timestamp every `NP`, `PA`, or `SA` intervention and preserve both the
first-attempt and eventual code; an eventual assisted completion does not erase
the first attempt. Confusion severity uses `C1` (noticed but self-recovered),
`C2` (cost time or required a neutral prompt), `C3` (required
procedural/solution help), or `C4` (blocked progress, access, safety, or data).

Lesson completion is not one Boolean. Mark each component separately as
`pass`, `fail`, `blocked`, or `not observed`: deterministic test, native build,
graphical/manual review, reflection/explanation, and saved evidence artifact.
The eventual outcome code summarizes those fields. A route is complete only
when every route-required section has an eventual completion code (`IC`, `NP`,
`PA`, or `SA`) and all five completion components pass. `complete-18` requires
00–17, `core-12` and `accelerated-8-plus-2` require 00–12 and 16–17, and a
custom route requires its exact predeclared list. Assisted route completion is
reported as assisted and is never counted as independent completion.

## Pilot entry gate

The facilitator completes these before inviting a participant:

- [ ] Record protocol version, curriculum commit, recruitment method, intended
      audience, prior-experience questions, and inclusion/exclusion rules.
- [ ] Define privacy/consent text, storage location, retention/deletion date,
      access list, compensation, and withdrawal procedure.
- [ ] Do not collect names, recordings, source projects, or device identifiers
      unless the consent and retention plan requires them.
- [ ] Provide a non-recorded route and explain that withdrawal has no penalty.
- [ ] Run `scripts/check-authoring.sh --require-hugo` and attach its log.
- [ ] Record the supported platform matrix; label every other platform
      unsupported rather than treating a participant as its tester by surprise.
- [ ] Author-walk every required lesson command on each platform being offered.
- [ ] Verify keyboard alternatives, pause/reduced-motion controls, transcripts,
      contrast/reflow, and device-missing fallbacks manually.
- [ ] Decide stop conditions: unsafe flashing, data loss, inaccessible required
      interaction, setup dead end, or distress ends the session and records a
      blocker.
- [ ] Prepare neutral prompts. Assistance must be timestamped and coded; do not
      silently turn an assisted completion into an independent completion.

## Session protocol

Use anonymous IDs such as `P01`; keep the identity key outside this repository.

1. Record date, protocol version, route ID and exact required/optional section
   list, commit, delivery mode, OS/version, hardware/input devices,
   openFrameworks/Hugo/toolchain versions, accessibility settings, and stated
   prior experience.
2. Ask the participant to narrate only if they consent. The facilitator records
   lesson start/end, breaks, retries, assistance, first confusion, and active
   minutes separately for reading, setup/build repair, exercise, reflection,
   and manual review.
3. For every lesson, run its published deterministic test command. Separately
   inspect the graphical/manual checklist; a passing model test is not a visual
   pass.
4. Ask the participant to explain the visual, numeric, and symbolic form of the
   lesson’s math idea. Record the response or `not observed`; do not grade by
   resemblance to lesson wording.
5. Record keyboard-only and reduced-motion/device-fallback attempts where
   applicable. Never force use of audio, camera, or other live input.
6. Save only consented artifacts. Record checksums or repository-relative paths,
   not private external URLs, in the evidence ledger.
7. End with the same short questions: what was expected, what surprised you,
   where did you get stuck, what would you change, and which mechanism could you
   reuse in an original sketch?
8. Review stop conditions, explain artifact deletion/withdrawal again, and
   record whether follow-up contact was separately consented.

## Lesson observation sheet (copy once per lesson and participant)

| Field | Required entry |
|---|---|
| Participant / lesson / commit | Anonymous ID, exact section slug, Git commit |
| Environment | OS, compiler, openFrameworks, relevant device and settings |
| Timing | Start, end, breaks; active minutes split into reading, setup/build repair, exercise, reflection, and manual review |
| Outcome | First-attempt and eventual code from the fixed codebook; completion definition used |
| Automated evidence | Command, exit code, concise output, artifact path |
| Manual graphical/device evidence | Observer, viewport/device, checklist result, artifact path or `not observed` |
| Human-learner evidence | Observable action or participant quote; never an interpretation alone |
| Assistance | Timestamp, neutral prompt or intervention, resulting action |
| Confusion | First confusing instruction/term, later friction, `C1`–`C4` severity |
| Accessibility | Keyboard, reduced motion, non-color cue, transcript/fallback result |
| Math check | Visual, numeric, symbolic explanation result or `not observed` |
| Originality | Intent and deliberate changes; do not score aesthetic preference |
| Completion components | Deterministic test, native build, graphical/manual review, reflection, artifact: each `pass` / `fail` / `blocked` / `not observed` |
| Follow-up | Issue/change candidate, owner, due date, verification needed |

## Participant course ledger (copy once per participant)

| Field | Required entry |
|---|---|
| Participant / protocol / route | Anonymous ID, protocol version, route ID, exact required/optional sections |
| Enrollment and consent | Date, inclusion decision, consent scope, artifact/recording choices, deletion date |
| Session plan | Planned dates, maximum active duration, break policy, facilitated/independent mode |
| Session history | Date, sections attempted, reading/setup-build/exercise/reflection/manual-review/break minutes, assistance count, next resume point |
| Missed or changed session | `not applicable`, rescheduled, route change, pause, or `WD`; never invent a reason |
| Course completion | Required sections attempted/completed by code, missing components, exact definition |
| Retention | Artifacts retained/deleted, date, authority, and separately consented follow-up status |

Setup reuse and repeated setup are recorded separately. A participant who
withdraws remains in denominators only where the consent plan permits and is
reported as `WD`, never as a failure or silent omission.

## Pilot evidence ledger

Add rows; do not overwrite failures or `not observed` rows. Link only artifacts
whose storage and consent rules permit repository access.

| ID | Date | Commit | Scope | Evidence class | Platform/device | Result | Evidence/artifact | Reviewer | Follow-up |
|---|---|---|---|---|---|---|---|---|---|
| BASELINE-001 | 2026-08-18 | `5afa25b7905f4e7001dbea4ea0286944398f959e` | complete curriculum pilot | Human learner | not applicable | not observed | No pilot session or learner outcome has been recorded. This row remains true after protocol-only revisions. | repository maintainers | Run entry gate before recruitment. |

## Synthesis and reporting gate

Do not aggregate until each session sheet is complete and intervention coding is
consistent. Report denominators and missing data, not only percentages.

- [ ] Separate independent, assisted, partial, blocked, and skipped outcomes.
- [ ] Report median and range for active time and setup time; retain per-section
      values so a single total does not hide a bottleneck.
- [ ] Group friction by instruction, environment, accessibility, concept, and
      defect; preserve dissenting observations.
- [ ] Separate automated failures, manual graphical/device findings, and learner
      findings in every summary.
- [ ] Require at least two observations before writing a recurring pattern, and
      still show the count. A single serious accessibility blocker remains a
      blocker, not an ignorable anecdote.
- [ ] Map each revision to evidence IDs and rerun the relevant automated/manual
      gate. Do not claim the revision helped learners until observed later.
- [ ] Publish only de-identified, consent-compatible results and limitations.
- [ ] Leave unsupported platforms and unobserved learner outcomes explicit.
