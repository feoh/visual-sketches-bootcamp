# Informal curriculum pilot and pacing notes

**Protocol version:** 1.2

**Last updated:** 2026-08-19
**Route definitions:** [`curriculum-map.md#provisional-course-routes`](curriculum-map.md#provisional-course-routes)
**Checked route contract:** [`pilot/routes.tsv`](pilot/routes.tsv)
**Ready-to-copy notes:** [`pilot/README.md`](pilot/README.md)

This is a lightweight way to try the course yourself, or to take notes if you
share it with someone else. It is not a research study, paid course, college
course, certification, or formal assessment.

Route names describe provisional weeks, not lesson counts or proven pacing. Each
named route lists 19 ordered bundles, including the first-C++ test interlude, in
`pilot/routes.tsv`. The manifest marks each bundle required or optional.

## Current status

**Learner pilot: not yet observed.** Automated tests and an author checking that
a sketch renders are useful technical evidence, but they do not show how long a
learner needs or where the instructions are confusing.

A self-pilot counts as learner feedback when you actually work through the
learner tasks without leaning on knowledge of the answer. Label it `self-pilot`
so its limits are clear. If someone else tries the course, label it
`shared-pilot`. Neither supports broad claims from one person's experience.

## What to record

Use [`pilot/progress-log.md`](pilot/progress-log.md) for the overall route and
copy [`pilot/lesson-notes.md`](pilot/lesson-notes.md) for each lesson. Keep the
notes practical:

- active time for reading, setup/build repair, exercise, reflection, and manual
  visual review;
- whether the deterministic test and native build passed;
- whether the sketch looked and behaved as intended;
- what was confusing and what help was needed;
- whether you could explain the idea visually, with numbers, and with symbols;
- a capture or other artifact you produced and looked at; and
- confidence before and after the checkpoint.

An artifact only needs to be produced and observed. Nobody has to give the
repository a copy. If another person tries the course, ask before saving or
sharing their words, captures, code, recordings, or identifying details. Do not
publish their material without a separate clear yes.

## Simple outcome labels

Use one label per lesson:

| Label | Meaning |
|---|---|
| `done` | Finished without another person revealing a procedure or answer. |
| `helped` | Finished after procedural or answer-level help. |
| `partial` | Made useful progress but did not finish every required component. |
| `blocked` | Could not continue because of instructions, setup, platform, device, or accessibility. |
| `skipped` | Optional under the chosen route or deliberately deferred before starting. |
| `paused` | Stopped for now without claiming a reason. |

In notes, distinguish a small nudge from procedural or answer-level help. The
point is to find where scaffolding is needed, not to grade the learner.

A required lesson is complete when its deterministic test, native build,
manual/graphical review, explanation, and produced artifact are all `pass`, and
its outcome is `done` or `helped`. A named route is complete when every row
marked `required` in `pilot/routes.tsv` meets that definition. `complete-18`
requires all 19 bundles. `core-12` and `accelerated-8-plus-2` make sections
13–15 optional and require the other 16 bundles.

## Before starting a route

- [ ] Pick a route, record the exact Git commit, and note the platform.
- [ ] Run `scripts/check-authoring.sh --require-hugo` or the PowerShell
      equivalent so curriculum failures are not blamed on the learner.
- [ ] Check disk space, install permissions, supported platforms, and any
      keyboard/reduced-motion or no-device fallback you need.
- [ ] Keep the course in a disposable learner branch.
- [ ] If sharing, agree on whether any notes or captures will be saved. “Nothing
      saved” is always a valid choice.

## Checkpoints

Run the fixed, short prompts in [`pilot/checkpoints.md`](pilot/checkpoints.md):

1. after Unit 0, reconstruct setup and the first test;
2. after Unit 2, combine seeded choice, local transforms, and gesture input in a
   new small sketch; and
3. at the complete path, use the section 17 capstone evidence.

Record confidence as self-report, not proof of skill. After each checkpoint,
choose `continue`, `revise`, or `pause`. Map course changes to the notes that
motivated them in [`pilot/revision-log.md`](pilot/revision-log.md). Do not claim a
revision improved pacing until it has been tried afterward.

## Pilot notes ledger

Append rows; do not erase a blocked or unfinished attempt.

| ID | Date | Commit | Mode | Route/scope | Platform | Result | Notes | Follow-up |
|---|---|---|---|---|---|---|---|---|
| BASELINE-001 | 2026-08-18 | `5afa25b7905f4e7001dbea4ea0286944398f959e` | not applicable | complete curriculum | not applicable | not observed | No self-pilot or shared learner run has been recorded. | Start with Unit 0 and use the lightweight notes. |

## Pacing summary

When enough lessons have been tried, summarize the actual section times and the
largest setup, instruction, accessibility, and concept bottlenecks. Keep one
person's result as one person's result. Revise pacing from those observations,
not from visual polish or automated test duration.
