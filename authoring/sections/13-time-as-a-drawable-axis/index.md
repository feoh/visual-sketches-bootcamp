---
title: Time as a drawable axis
slug: 13-time-as-a-drawable-axis
weight: 150
draft: false
course_kind: instructional
objectives:
  - Implement and inspect a bounded ring buffer whose age zero is always the newest sample
  - Map normalized spatial position to a deterministic retained history age
  - Supply frame and time explicitly so model replay does not depend on a window clock
  - Check allocation arithmetic, byte budget, finite values, order, and counter overflow before mutation
  - Design an accessible spatial-temporal collage while keeping GPU appearance outside portable model evidence
prerequisites:
  - Completion of section 11 or equivalent vectors, interpolation, finite guards, deterministic tests, and openFrameworks event adapters
source_records: sources.yaml
asset_records: assets.yaml
---

# Time as a drawable axis

## Look

![Twenty vertical time slices cross a navy field; newest amber circles begin high at the left, progressively older mint circles descend and fade to the right, and horizontal ticks plus NOW and THEN labels communicate age without color.](media/time-axis-preview.svg "Horizontal position selects a retained moment.")

*A bounded history turns horizontal position into age zero through the oldest retained sample.*

The labels, repeated slice boundaries, circle outlines, and interior ticks keep
age order legible without relying on amber, mint, or opacity alone.

## Precedent and transfer

Use the course's [credited precedent notes](../../../docs/source-notes.md#visual-vocabulary)
to study time as stored material rather than copy an echo or slit-scan image.
Name creators and collaborators, isolate the history-selection principle, and
change the source, spatial lookup, decay, geometry, palette, and interaction.

## Predict

A history with capacity three receives frames 10, 11, 12, then 13. Which frames
remain from oldest to newest? If horizontal position 0 means newest and 1 means
oldest, which frame does position 0.5 select? Before reading on, sketch the
storage slots and the moving next-write index.

## Learn

### A ring makes bounded time

A [`std::vector<Sample>`](https://en.cppreference.com/w/cpp/container/vector.html)
is allocated once. `next` names the slot that the next capture overwrites and
`count` never exceeds capacity. The section's one substantial new C++ mechanism
is modulo indexing over that fixed storage:

```cpp
storage[next] = sample;
next = (next + 1) % storage.size();
count = std::min(count + 1, storage.size());
```

Age zero is newest. The lookup is:

```text
slot(age) = (next + capacity - 1 - age) mod capacity
```

For capacity 3 after frames 10, 11, 12, 13, `next` has wrapped and frame 10 is
overwritten. The retained order is 11, 12, 13. Age one selects frame 12. The
formula hides physical slot order from rendering code, which asks only for age.

The model rejects zero capacity, more than 65,536 samples, multiplication
overflow, or more than 8 MiB before allocating. It computes
`capacity * sizeof(Sample)` on the actual compiler instead of pretending every
ABI gives the struct the same byte size. Frame numbers must strictly increase;
time must not go backward; time, position, phase, decay, and viewport values
must be finite. A rejected push or resize leaves the prior history unchanged.

### Space selects a discrete age

Let `x` be horizontal position normalized to `[0, 1]`, and let `n` be retained
history length. The symbolic mapping is:

```text
age(x, n) = floor(x * (n - 1) + 0.5)
```

Numerically, with five samples, positions `0`, `0.24`, `0.5`, `0.76`, and `1`
select ages `0`, `1`, `2`, `3`, and `4`. Visually:

```text
NOW                                              THEN
x=0        .24        .50        .76        1.0
age 0  ---- 1  ------- 2  ------- 3  ------- 4
newest                                         oldest
```

The half-up rule makes every decision explicit and testable. It is a sampler,
not interpolation between two moments. If you choose interpolation in a remix,
test both neighboring ages and the blend amount separately.

### Decay describes a stable echo

Age can also control intensity. The reference uses
[`std::exp`](https://en.cppreference.com/w/cpp/numeric/math/exp.html):

```text
opacity(age) = exp(-decay_rate * age / max(1, n - 1))
```

With decay rate 2, newest opacity is 1, halfway opacity is about 0.368, and the
oldest is about 0.135. The curve is positive and monotone. It attenuates an echo;
it does not copy bright output back into itself. A feedback remix with gain at
or above 1 can preserve or amplify energy, so gain, clearing, and clamping need
their own tests and manual review.

### Replay owns frame and time

`makeMotionSample(frame, time_seconds, width, height, output)` receives both
clock values. It never calls an openFrameworks clock. Replaying the same tuples
produces the same sample sequence under the tested C++17 implementation. The OF
adapter may gather wall-clock deltas, but it validates them and passes explicit
values into the pure model. This boundary makes order, wraparound, resize,
selection, replay, and overflow testable without a graphics context.

Resize keeps the newest suffix when shrinking and preserves chronological order
when growing. Reset clears count and next-write position while retaining a valid
capacity. Neither operation invents timestamps.

### GPU pixels are evidence of appearance, not model correctness

The delivered adapters draw directly and do not require `ofFbo`. An optional
[`ofFbo`](https://openframeworks.cc/documentation/gl/ofFbo/) experiment belongs
behind the renderer boundary: allocation, texture orientation, alpha blending,
resize reallocation, GPU limits, and ping-pong read/write separation require a
native manual check. Pure tests must not instantiate a GL context or pixel-gate
results across drivers. Never sample from the same target currently being drawn
unless a documented platform-safe technique proves that behavior.

## Build: inspect three complete experiments

### 1. Read the hand-checkable oracle

Linux x86-64 or macOS arm64:

```sh
cat exercises/13-time-as-a-drawable-axis/fixtures/temporal-oracle.txt
CXX=g++ tests/run-section-13-tests.sh
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
Get-Content .\exercises\13-time-as-a-drawable-axis\fixtures\temporal-oracle.txt
.\tests\run-section-13-tests.ps1
```

Confirm origin position `(84, 50)` in a 100-square viewport and the five pinned
spatial selections.

### 2. Trace one wrap and resize

Run the same pure suite with Clang, then find the assertions that push frames 1
through 5 into capacity 3 and shrink frames 1 through 6 to capacity 3:

```sh
CXX=clang++ tests/run-section-13-tests.sh
```

The first retains 3, 4, 5 after wrap. The second also retains 4, 5, 6 because
resize keeps the newest suffix rather than physical slots.

### 3. Run the time windows

Set `OF_ROOT` to openFrameworks 0.12.1. Supported native lanes are Linux x86-64,
macOS arm64, and Windows Visual Studio 2022 x64 Developer PowerShell. Other
architectures are unattested and unsupported by these wrappers.

Linux x86-64:

```sh
scripts/section-13.sh generate --project starter
scripts/section-13.sh build --project starter --configuration Release
exercises/13-time-as-a-drawable-axis/starter/bin/starter
```

macOS arm64:

```sh
scripts/section-13.sh generate --project starter
scripts/section-13.sh build --project starter --configuration Release
open exercises/13-time-as-a-drawable-axis/starter/bin/starter.app
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\scripts\section-13.ps1 generate -Project starter
.\scripts\section-13.ps1 build -Project starter -Configuration Release
& .\exercises\13-time-as-a-drawable-axis\starter\bin\starter.exe
```

P pauses, R resets explicit frame/time history, and M freezes capture for a
reduced-motion still. Resize resets history against the new viewport. A native
build proves compilation only; launch and inspect the actual GPU window.

## Break and repair

In `exercises/13-time-as-a-drawable-axis/shared/temporal_history.cpp`,
temporarily remove `- 1` from the `atAge` slot expression. Predict whether age
zero reads the next-write slot before and after wrap. Run the pure suite, restore
the subtraction, and rerun. If this was your only edit:

```sh
git restore -- exercises/13-time-as-a-drawable-axis/shared/temporal_history.cpp
```

That command discards every uncommitted change in the named file. Record the
first failed order assertion and explain why a not-yet-full buffer exposes an
unwritten slot.

## Exercise

Open the [spatial-temporal collage brief](../../../exercises/13-time-as-a-drawable-axis/README.md).
Change bounded parameters in `starter/src/design/temporal_design.cpp` and own
the geometry grammar in `starter/src/ofApp.cpp`. Make spatial position choose
age, but produce an image unlike both the starter's filled time windows and the
solution's dark line-and-diamond loom.

## Test

```sh
CXX=g++ tests/run-section-13-tests.sh
CXX=clang++ tests/run-section-13-tests.sh
scripts/check-authoring.sh
```

Use the PowerShell suite on Windows. With a valid `OF_ROOT`, generate and build
starter and solution in Debug and Release. Manually launch keyboard routes,
pause/reset/reduced motion, history fill and wrap, tiny and resized windows,
contrast, age cues, and originality. If you add FBOs, inspect allocation failure,
resize, alpha/clear behavior, texture orientation, and ping-pong separation on
each claimed GPU lane. Automated tests do not validate those pixels.

## Reflect

In 160–200 words, distinguish capacity from length, physical slot from age,
frame from time, sampling from interpolation, resize from reset, and decay from
feedback gain. Explain the modulo formula, checked byte budget, explicit replay
inputs, one non-color age cue, one reduced-motion choice, and one learner-owned
visual decision. Include capture alt text.

## Remix

Use y instead of x for age, arrange ages around a clock, select age from distance
to a pointer-controlled seam, interpolate between adjacent samples, retain
multiple bounded histories, or add two explicitly separated FBOs for feedback.
Preserve finite guards, monotone frame/time order, checked memory arithmetic,
deterministic selection, reset/resize behavior, keyboard access, reduced
motion, and the pure/GPU validation boundary.

## Manual review

- Left-to-right age remains understandable from labels, boundaries, shape, or ticks without color alone.
- P pauses, R replays from frame/time zero, and M provides a still reduced-motion alternative.
- Resize and a tiny window fail safely; history fill and wrap do not flash or expose unwritten samples.
- Text and marks retain suitable contrast; there is no audio-only instruction.
- Spatial grammar, density, temporal range, geometry, composition, and palette differ from both examples.
- Capture alt text names spatial age direction, newest/oldest states, geometry, fading, controls, and palette roles.
- Any FBO path is manually checked for allocation, orientation, clear/alpha, resize, GPU limits, and read/write separation.
- Reused references, code, and assets are credited.

## Pilot note

Pilot evidence not yet collected. After a learner completes this section,
record exact platform/tool versions; reading, prediction, repair, exercise, and
reflection time separately; setup friction; pointer/keyboard and reduced-motion
access; whether next-write versus newest, modulo wrap, capacity versus length,
spatial rounding, decay, explicit frame/time, replay, resize/reset, byte budget,
finite/overflow rejection, and pure/GPU boundaries were understood; automated
result; manual accessibility/originality/FBO result; and confusions. Do not infer
completion time, visual success, GPU portability, or accessibility from CI.
