# Complete-curriculum pilot protocol and evidence ledger

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

Allowed result values are `pass`, `fail`, `blocked`, and `not observed`. Never
replace a missing observation with `pass`.

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

1. Record date, commit, delivery mode, OS/version, hardware/input devices,
   openFrameworks/Hugo/toolchain versions, accessibility settings, and stated
   prior experience.
2. Ask the participant to narrate only if they consent. The facilitator records
   lesson start/end, active work, breaks, setup time, retries, assistance, and
   the first point of confusion.
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
| Timing | Start, end, breaks, active minutes, setup minutes |
| Outcome | completed / partial / blocked / skipped, plus definition used |
| Automated evidence | Command, exit code, concise output, artifact path |
| Manual graphical/device evidence | Observer, viewport/device, checklist result, artifact path or `not observed` |
| Human-learner evidence | Observable action or participant quote; never an interpretation alone |
| Assistance | Timestamp, neutral prompt or intervention, resulting action |
| Confusion | First confusing instruction/term, later friction, severity |
| Accessibility | Keyboard, reduced motion, non-color cue, transcript/fallback result |
| Math check | Visual, numeric, symbolic explanation result or `not observed` |
| Originality | Intent and deliberate changes; do not score aesthetic preference |
| Follow-up | Issue/change candidate, owner, due date, verification needed |

## Pilot evidence ledger

Add rows; do not overwrite failures or `not observed` rows. Link only artifacts
whose storage and consent rules permit repository access.

| ID | Date | Commit | Scope | Evidence class | Platform/device | Result | Evidence/artifact | Reviewer | Follow-up |
|---|---|---|---|---|---|---|---|---|---|
| BASELINE-001 | 2026-08-18 | repository baseline | complete curriculum pilot | Human learner | not applicable | not observed | No pilot session or learner outcome has been recorded. | repository maintainers | Run entry gate before recruitment. |

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
