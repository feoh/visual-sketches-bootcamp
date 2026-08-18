---
title: Three cumulative sketch studies
slug: 16-three-cumulative-sketch-studies
weight: 180
draft: false
course_kind: synthesis
objectives:
  - Complete three bounded studies that each combine two named technique families
  - Specify and test one deterministic model contract for each study
  - Compare divergent visual grammars without treating an exemplar as a target
  - Document each study with an accessible capture and a 100-word mechanism explanation
prerequisites:
  - Completion of sections 06 through 12 or equivalent work with seeded models, transforms, gestures, particles, forces, and flow
  - Ability to separate deterministic state and geometry from openFrameworks input and rendering adapters
source_records: sources.yaml
asset_records: assets.yaml
---

# Three cumulative sketch studies

## Brief

Make **three small, separate sketches**, not one three-mode application. Each
study combines the named families below, fits one focused weeknight, and leaves
composition, palette, mark vocabulary, and interaction choices to you.

| Study | Required families | Weeknight constraint | Required evidence |
|---|---|---|---|
| A | gesture + temporal memory | 75 minutes; one pointer; at most 96 retained samples; no particles | one model test, one capture, exactly 100 words |
| B | repetition + oscillation or transforms | 75 minutes; one repeated unit; 8–64 instances; at most two animated parameters | one model test, one capture, exactly 100 words |
| C | particles + forces or flow | 90 minutes; at most 256 particles; one force or one sampled field; fixed simulation step | one model test, one capture, exactly 100 words |

Time starts after a project builds and its empty test target runs. At the limit,
stop adding features, preserve a working reset, and document what is incomplete.
Do not spend the study window polishing deployment.

For all three, keep a pure model function or small model object outside
`ofApp`. Record every seed, replay input, viewport, fixed step, and tolerance
needed to reproduce the automated result. Test state or geometry, never a
cross-platform screenshot.

Use the [model-test contract template](templates/model-test-contract.md) before
coding and the [capture and 100-word template](templates/capture-and-explanation.md)
afterward. The fixtures are evaluator inputs and expected values, not starter
implementations.

## Shared acceptance contract

A study is complete when:

- its two required families affect the same visual rule rather than appearing as unrelated layers;
- reset restores the documented initial model and replaying the fixture gives the same result;
- the evaluator can run without a window, clock, pointer, audio device, or network;
- one known case, one boundary, and one invariant are named, with at least one encoded in the supplied fixture;
- the capture has descriptive alt text and does not rely on color alone;
- the explanation contains exactly 100 words, excluding its heading and caption; and
- borrowed code, visual precedents, type, images, audio, and addons are either credited and licensed or explicitly recorded as absent.

The supplied section 16 TSV fixtures are original course evaluator data by
Visual Sketches Bootcamp contributors, dedicated under CC0-1.0. Their
[provenance and failure contract](fixtures/README.md) controls redistribution.

The Web Content Accessibility Guidelines' guidance for
[non-text content](https://www.w3.org/TR/WCAG22/#non-text-content) is the minimum,
not the entire review: explain the visual rule, state, and change that the
capture is evidence for.

## Study A — Gesture remembered as structure

### Constraint and choices

Accept one pointer stream, retain no more than 96 samples, and make age or
history position change geometry. Temporal memory must do more than leave an
alpha trail. It might reorder, offset, connect, fold, stamp, or erode sampled
points. You choose the sampling rule and visual grammar.

### Model-test contract

Implement a pure acceptance-and-memory operation. Given previous accepted
points, a candidate point, a minimum distance, and a capacity, it returns a new
bounded sequence. Distance exactly equal to the threshold is accepted; a
nearer point is rejected without changing state; overflow evicts the oldest
point. Run the sequential cases in
[`gesture-memory.tsv`](fixtures/gesture-memory.tsv). Compare coordinates with a
documented absolute tolerance no larger than `0.0001`.

Required checks:

1. known case: `(3, 4)` is accepted after `(0, 0)` at distance 5;
2. boundary: equality at distance 5 is accepted;
3. invariant: count never exceeds capacity and rejection is transactional.

### Capture and explanation

Capture one gesture after enough input to reveal memory order. Alt text must
name the newest and oldest visible structures, the non-color cue for age, and
the interaction state. In exactly 100 words, connect the sampling boundary,
retention policy, temporal mapping, one composition decision, the fixture
result, and one limitation.

### Divergent partial exemplars

![Two unfinished gesture-memory studies: one turns a path into spaced black contour ribs, while another places shrinking square stamps into four age columns.](media/gesture-memory-partials.svg "Gesture memory can become contour structure or a reordered archive.")

*Partial, non-canonical directions: contour ribs preserve adjacency, while age columns deliberately break the original path.*

- **Contour ribs:** resample by distance, estimate a tangent, and draw short normal segments whose spacing indicates age. It remains incomplete: no palette or interaction beyond reset.
- **Age archive:** assign accepted points to four age bands and stamp only relative turn direction. It remains incomplete: the final composition and motion policy are undecided.

These grammars differ from a worked calligraphic polyline: one turns the gesture
into a measured cross-section; the other discards literal path continuity.

## Study B — Repetition under a local rule

### Constraint and choices

Use one repeated unit 8–64 times. Combine repetition with **either** oscillation
or local transforms. Animate no more than two model parameters. Each instance
must be derivable from its index and explicit model inputs; avoid hand-placing
exceptions to repair the composition.

### Model-test contract

Choose the contract that matches your second family.

**Transform route:** implement `transformPoint(local, translation,
angleDegrees)` independently of the renderer. Rotation occurs around local
origin before translation. Run the known angles in
[`repetition-transform.tsv`](fixtures/repetition-transform.tsv) with absolute
tolerance `0.0001`. Check that `(10, 0)` at zero degrees preserves its offset,
that 360 degrees agrees with zero, and that rotation preserves distance from
the translated origin.

**Oscillation route:** implement `oscillationOffset(phaseRadians, amplitude)` as
`amplitude * sin(phaseRadians)`. Run
[`repetition-oscillation.tsv`](fixtures/repetition-oscillation.tsv) with
absolute tolerance `0.0001`. Check zero phase, the positive and negative
quarter-cycle extrema, one full-cycle return, and the invariant that absolute
offset never exceeds absolute amplitude.

Do not submit the transform fixture as evidence for a sketch whose chosen
second family is oscillation, or vice versa.

### Capture and explanation

Capture a frame where index and local rule are both inferable. Alt text must
state the repeated unit, count or density, ordering cue, and whether time is
paused. In exactly 100 words, connect index to placement, explain transform
order or phase, report the fixture tolerance, identify the two animated
parameters or say only one is used, and name one rejected embellishment.

### Divergent partial exemplars

![Two unfinished repetition studies: offset open brackets form a stepped asymmetric ladder, while nested triangular frames alternate orientation around an empty center.](media/repetition-partials.svg "Repeated units can organize as an asymmetric ladder or a nested alternating frame.")

*Partial, non-canonical directions: a discontinuous bracket ladder and nested alternating frames avoid the radial sine-field grammar of worked examples.*

- **Bracket ladder:** translate one open bracket along a piecewise staircase; use index parity for reflection and no sine wave. The gap, not the mark, carries rhythm.
- **Nested frames:** scale and rotate one triangular outline around an empty center; pause time and let transform order create interference. It has no settled interaction or palette.

At least one submitted study must be this substantially different from worked
examples: not a radial grid or field of circles whose size follows sine. State
the worked grammar you avoided and the structural replacement you chose.

## Study C — A bounded population in one influence

### Constraint and choices

Use at most 256 particles and choose **one** influence family: accumulated
forces or a sampled flow field. Every particle shares one explicit fixed step.
No trails longer than eight points, no collision system, and no second force
added only for visual complexity. Spawn from a recorded event or seed.

### Model-test contract

Implement one semi-implicit Euler step: first `velocity += influence * dt`, then
`position += velocity * dt`. Replay
[`particle-step.tsv`](fixtures/particle-step.tsv) sequentially from zero state
using `dt = 0.5`. Compare with absolute tolerance `0.0001`. Production code may
substep at a smaller fixed step; if so, add a fixture generated from that stated
step rather than editing expected values silently.

Required checks:

1. known case: constant horizontal influence changes velocity before position;
2. boundary: zero influence preserves velocity;
3. invariant: accepted finite input produces finite state and population never exceeds its cap.

### Capture and explanation

Capture a seeded or replayed checkpoint, not a hand-selected unrecorded moment.
Alt text must name population shape, direction cue, boundary behavior, and a
non-color encoding of motion. In exactly 100 words, connect influence to update
order, fixed step to replay, seed or spawn event to composition, the population
cap to frame budget, the fixture result, and one manual observation.

### Divergent partial exemplars

![Two unfinished particle studies: sparse rectangular carriers bend through a blocky vector corridor, while dense dots settle into three spring-like horizontal queues.](media/particle-partials.svg "A flow corridor and restrained queues use different population, influence, and boundary grammars.")

*Partial, non-canonical directions: sparse oriented carriers traverse a quantized corridor; dense dots collect into damped queues.*

- **Quantized corridor:** a coarse, recorded field turns sparse rectangles in 45-degree increments; boundary exit respawns from a fixture event. No translucent trail is used.
- **Damped queues:** particles seek one of three horizontal anchors and encode speed through spacing, not glow. The partial stops before adding input or a final color system.

## Evaluation and handoff

For each study, submit source, build command, test command and output, fixture
copy, capture, 100-word explanation, controls, seed/replay values, and credits.
Use this 10-point review independently for each study:

| Dimension | 0 | 1 | 2 |
|---|---|---|---|
| Family synthesis | mechanisms are absent or unrelated | both appear but one is decorative | one coherent rule depends on both |
| Deterministic model | no isolated contract | contract runs but misses a required edge | fixture plus known, boundary, and invariant checks pass |
| Bounded behavior | unbounded or frame-dependent | cap or time policy is incomplete | stated cap, reset, and time policy hold |
| Visual authorship | copies a worked grammar | some choices change | structure and mapping materially differ |
| Evidence | missing/inaccessible | capture or prose is incomplete | accessible capture and exact 100-word explanation connect intent to evidence |

A similar still triggers a mechanism conversation; it is not an automatic
failure or proof of originality. Compare geometry, density, mapping,
interaction, motion, and temporal behavior. Record which choices are yours.

## Completion checklist

- [ ] Three separate sketches use all three named family pairs.
- [ ] Each fits its 75/75/90-minute scope or reports where work stopped.
- [ ] Each has one deterministic, headless model-test contract and fixture result.
- [ ] Each has one accessible capture and exactly 100 explanatory words.
- [ ] At least one visual grammar substantially differs from worked examples.
- [ ] Reset, caps, explicit time, seeds/replay inputs, controls, and limitations are recorded.
- [ ] All borrowed material has source, creator, and license; absent borrowing is stated.
- [ ] No exemplar was completed into or presented as a canonical answer.
