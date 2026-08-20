# Route definitions and optional practice notes

**Protocol version:** 1.2

**Last updated:** 2026-08-19
**Route definitions:** [`curriculum-map.md#pick-a-pace-or-make-up-your-own`](curriculum-map.md#pick-a-pace-or-make-up-your-own)
**Checked route contract:** [`pilot/routes.tsv`](pilot/routes.tsv)
**Optional notes:** [`pilot/README.md`](pilot/README.md)

This file documents the machine-checked order of the published lessons. It is
project bookkeeping, not an assignment. The route names describe suggested
weeks, not required pacing, and the checked manifest lists 19 ordered bundles,
including the first-C++ test interlude.

## Current status

The lessons, examples, tests, and optional routes are available to use. No
reader report, timing study, or submission is required. Automated checks cover
links, builds, and deterministic behavior; graphical appearance and live-device
behavior are best inspected while you make your own sketches.

## What the optional notes are for

Use [`pilot/progress-log.md`](pilot/progress-log.md) if a personal checklist is
helpful. Use [`pilot/lesson-notes.md`](pilot/lesson-notes.md) to save a command,
a bug, a visual idea, or a useful explanation. Keep those notes private if they
contain personal details, captures, code, or recordings.

A reader can use simple labels such as `done`, `partial`, `blocked`, `skipped`,
or `paused`, but no label is necessary. The important result is a sketch you
understand well enough to change.

## Before starting a route

- [ ] Pick a route, or ignore the routes and follow your curiosity.
- [ ] Run `scripts/check-authoring.sh --require-hugo` or the PowerShell
      equivalent if you are changing the curriculum itself.
- [ ] Check disk space, install permissions, supported platforms, and any
      keyboard/reduced-motion or no-device fallback you need.
- [ ] Keep the course in a disposable learner branch if you want easy resets.

## Optional checkpoints

The prompts in [`pilot/checkpoints.md`](pilot/checkpoints.md) are small remix
challenges, not exams:

1. after Unit 0, reconstruct setup and the first test;
2. after Unit 2, combine seeded choice, local transforms, and gesture input in a
   new small sketch; and
3. at the complete path, use the section 17 capstone evidence.

Try them when they sound fun. Skip them when another experiment has your
attention. The route file remains the source of truth for published order.

## Route contract

A named route is only a convenience for selecting required and optional lesson
bundles. `complete-18` includes every bundle; `core-12` and
`accelerated-8-plus-2` treat sections 13–15 as optional. None of these routes is
a grade, certification, or promise about how fast a person should learn.
