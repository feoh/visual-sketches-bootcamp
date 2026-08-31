# Route definitions and optional practice notes

**Protocol version:** 1.3

**Last updated:** 2026-08-30
**Route definitions:** [`curriculum-map.md#pick-a-pace-or-make-up-your-own`](curriculum-map.md#pick-a-pace-or-make-up-your-own)
**Checked route contract:** [`pilot/routes.tsv`](pilot/routes.tsv)
**Optional notes:** [`pilot/README.md`](pilot/README.md)

This file documents the machine-checked order of the published lessons. The
route names are only suggested ways to spread out the work. The checked file
lists 21 ordered bundles, including the first-C++ test interlude.

## Current status

The lessons, examples, tests, and optional routes are available to use. No
extra records are needed. Automated checks cover links, builds, and repeatable
behavior; appearance and live-device behavior are best inspected while you make
your own sketches.

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

The prompts in [`pilot/checkpoints.md`](pilot/checkpoints.md) are optional remix
ideas:

1. after Unit 0, reconstruct setup and the first test;
2. after Unit 2, combine seeded choice, local transforms, and gesture input in a
   new small sketch; and
3. near the end, revisit one section 19 idea with saved input.

Try them when they sound fun. Skip them when another experiment has your
attention. The route file remains the source of truth for published order.

## Route contract

A named route is only a convenient selection of lesson bundles. `complete-20`
includes every bundle; `core-12` and `accelerated-8-plus-2` treat sections
13–17 as optional. Move between routes or change the pace whenever that helps.
