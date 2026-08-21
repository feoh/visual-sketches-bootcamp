---
title: Time as a drawable axis
slug: 13-time-as-a-drawable-axis
weight: 150
draft: false
course_kind: instructional
objectives:
  - Implement and inspect a limited ring buffer whose age zero is always the newest sample
  - Map normalized spatial position to a repeatable retained history age
  - Supply frame and time explicitly so model replay does not depend on a window clock
  - Check allocation arithmetic, byte budget, finite values, order, and counter overflow before mutation
  - Design an accessible spatial-temporal collage while checking its calculations without comparing GPU pixels
prerequisites:
  - Completion of section 11 or equivalent vectors, interpolation, finite guards, repeatable tests, and openFrameworks event adapters
source_records: sources.yaml
asset_records: assets.yaml
---

# Time as a drawable axis

## See what you're making

![Twenty vertical time slices cross a navy field; newest amber circles begin high at the left, progressively older mint circles descend and fade to the right, and horizontal ticks plus NOW and THEN labels communicate age without color.](media/time-axis-preview.svg "Horizontal position chooses a saved moment.")

*A limited history turns horizontal position into ages from the newest through the oldest saved sample.*

The labels, repeated slice boundaries, circle outlines, and interior ticks keep age
order legible without relying on amber, mint, or opacity alone.

## Borrow the idea, not the artwork

Use the course's [credited precedent notes](../../../docs/source-notes.md#visual-vocabulary) to study time as stored material rather than copy an
echo or slit-scan image. Name creators and collaborators, isolate the history-selection
principle, and change the source, spatial lookup, decay, geometry, palette, and
interaction.

## Take a guess

A history with capacity three receives frames 10, 11, 12, then 13. Which frames remain
from oldest to newest? If horizontal position 0 means newest and 1 means oldest, which
frame does position 0.5 select? Before reading on, sketch the storage slots and the
moving next-write index.

## Let's unpack it

### Before the history vocabulary

A **ring buffer** is a fixed row of storage slots used in a loop. When writing reaches
the last slot, it wraps to the first and replaces the oldest value. It keeps a useful
recent history without growing forever.

This sketch lets horizontal position choose an age from that history. A point near the
left selects the newest saved frame; a point farther right selects an older one. The
result turns time into something you can place across the screen.

The code uses whole-number age slots because a saved frame is either in one slot or
another. A small worked example below shows exactly how a 0-to-1 position becomes an
index.

### A ring makes limited time

A [`std::vector<Sample>`](https://en.cppreference.com/w/cpp/container/vector.html) is allocated once. `next` names the slot that the next capture
overwrites and `count` never exceeds capacity. The section's one substantial new
C++ mechanism is modulo indexing over that fixed storage:

```cpp
storage[next] = sample;
next = (next + 1) % storage.size();
count = std::min(count + 1, storage.size());
```

Age zero means newest. With capacity 3, push frames 10, 11, 12, then 13. Frame 13 wraps
to the first storage slot and replaces frame 10. The retained ages are:

```text
age 0 -> frame 13 (newest)
age 1 -> frame 12
age 2 -> frame 11 (oldest)
```

The helper finds the right storage slot with:

```text
slot = (next slot + capacity - 1 - age) modulo capacity
```

**Modulo** means wrap around after reaching the end. Drawing code can ask for an age
without needing to know which physical slot currently holds it.

The model rejects zero capacity, more than 65,536 samples, multiplication overflow, or
more than 8 MiB before allocating. It computes `capacity * sizeof(Sample)` on the actual compiler
instead of pretending every ABI gives the struct the same byte size. Frame numbers must
strictly increase; time must not go backward; time, position, phase, decay, and viewport
values must be finite. A rejected push or resize leaves the prior history unchanged.

### Space selects a discrete age

With five saved samples, positions `0`, `0.24`, `0.5`,
`0.76`, and `1` select ages `0`, `1`,
`2`, `3`, and `4`. The code calculates that with:

```text
age = floor(position × (history length - 1) + 0.5)
```

Here `floor` drops the decimal part after adding `0.5`, which rounds to
the nearest whole age slot. Visually:

```text
NOW                                              THEN
x=0        .24        .50        .76        1.0
age 0  ---- 1  ------- 2  ------- 3  ------- 4
newest                                         oldest
```

The half-up rule makes every decision explicit and testable. It is a sampler, not
interpolation between two moments. If you choose interpolation in a remix, test both
neighboring ages and the blend amount separately.

### Decay describes a stable echo

Age can also control intensity. The reference uses [`std::exp`](https://en.cppreference.com/w/cpp/numeric/math/exp.html):

```text
opacity(age) = exp(-decay_rate * age / max(1, n - 1))
```

With decay rate 2, newest opacity is 1, halfway opacity is about 0.368, and the oldest
is about 0.135. The curve is positive and monotone. It attenuates an echo; it does not
copy bright output back into itself. A feedback remix with gain at or above 1 can
preserve or amplify energy, so gain, clearing, and clamping need their own tests and
manual review.

### Replay owns frame and time

`makeMotionSample(frame, time_seconds, width, height, output)` receives both clock values. It never calls an openFrameworks clock.
Replaying the same tuples produces the same sample sequence under the tested C++17
implementation. The OF adapter may gather wall-clock deltas, but it validates them and
passes explicit values into the pure model. This boundary makes order, wraparound,
resize, selection, replay, and overflow testable without a graphics context.

Resize keeps the newest suffix when shrinking and preserves chronological order when
growing. Reset clears count and next-write position while retaining a valid capacity.
Neither operation invents timestamps.

### Test saved history with numbers; check appearance yourself

The delivered adapters draw directly and do not require `ofFbo`. An optional
[`ofFbo`](https://openframeworks.cc/documentation/gl/ofFbo/) experiment belongs behind the renderer boundary: allocation, texture
orientation, alpha blending, resize reallocation, GPU limits, and ping-pong read/write
separation require a native manual check. Pure tests must not instantiate a GL context
or pixel-gate results across drivers. Never sample from the same target currently being
drawn unless a documented platform-safe technique proves that behavior.

## Make it run: inspect three complete experiments

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

Run the same pure suite with Clang, then find the assertions that push frames 1 through
5 into capacity 3 and shrink frames 1 through 6 to capacity 3:

```sh
CXX=clang++ tests/run-section-13-tests.sh
```

The first retains 3, 4, 5 after wrap. The second also retains 4, 5, 6 because resize
keeps the newest suffix rather than physical slots.

### 3. Run the time windows

Set `OF_ROOT` to openFrameworks 0.12.1. The course supplies checked commands for Linux
x86-64, macOS arm64, and Windows Visual Studio 2022 x64 Developer PowerShell. On another
system, the sketch may work, but you may need to adapt the build steps.

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
reduced-motion still. Resize resets history against the new viewport. After building,
open the app and inspect the actual window.

## Break it on purpose

In `exercises/13-time-as-a-drawable-axis/shared/temporal_history.cpp`, temporarily remove `- 1` from the `atAge` slot
expression. Predict whether age zero reads the next-write slot before and after wrap.
Run the pure suite, restore the subtraction, and rerun. If this was your only edit:

```sh
git restore -- exercises/13-time-as-a-drawable-axis/shared/temporal_history.cpp
```

That command discards every uncommitted change in the named file. Before moving on, make
sure you can explain why a not-yet-full buffer exposes an unwritten slot.

## Your turn

Open the [spatial-temporal collage brief](../../../exercises/13-time-as-a-drawable-axis/README.md). Change limited parameters in `starter/src/design/temporal_design.cpp` and own the
geometry grammar in `starter/src/ofApp.cpp`. Make spatial position choose age, but produce an
image unlike both the starter's filled time windows and the solution's dark
line-and-diamond loom.

## Check your work

```sh
CXX=g++ tests/run-section-13-tests.sh
CXX=clang++ tests/run-section-13-tests.sh
scripts/check-authoring.sh
```

Use the PowerShell suite on Windows. With a valid `OF_ROOT`, generate and build
starter and solution in Debug and Release. Manually launch keyboard routes,
pause/reset/reduced motion, history fill and wrap, tiny and resized windows, contrast,
age cues, and originality. If you add FBOs, inspect allocation failure, resize,
alpha/clear behavior, texture orientation, and ping-pong separation on the graphics
setup you use. Automated tests do not inspect those pixels.

## Optional notes for future you

Explain the difference between a physical storage slot and a sample's age. Show how one
left-to-right position chooses an age, then name one non-color age cue and one visual
decision you made. Save a capture with alt text.

## Make it yours

Use y instead of x for age, arrange ages around a clock, select age from distance to a
pointer-controlled seam, interpolate between adjacent samples, retain multiple limited
histories, or add two explicitly separated FBOs for feedback. Preserve finite guards,
monotone frame/time order, checked memory arithmetic, repeatable selection, reset/resize
behavior, keyboard access, reduced motion, and the pure/GPU validation boundary.

## Quick visual check

- Left-to-right age remains understandable from labels, boundaries, shape, or ticks
  without color alone.
- P pauses, R replays from frame/time zero, and M provides a still reduced-motion
  alternative.
- Resize and a tiny window fail safely; history fill and wrap do not flash or expose
  unwritten samples.
- Text and marks retain suitable contrast; there is no audio-only instruction.
- Spatial grammar, density, temporal range, geometry, composition, and palette differ
  from both examples.
- Capture alt text names spatial age direction, newest/oldest states, geometry, fading,
  controls, and palette roles.
- Any FBO path is manually checked for allocation, orientation, clear/alpha, resize, GPU
  limits, and read/write separation.
- Reused references, code, and assets are credited.

## If you get stuck

If the echo is out of order, label each history slot with its age and inspect one
write/read cycle. Check capacity versus length, then check which end is newest. A ring
buffer is a queue wearing a circle costume; draw the slots on paper before debugging the
shader-sized version.
