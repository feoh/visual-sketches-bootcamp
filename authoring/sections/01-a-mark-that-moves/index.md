---
title: A mark that moves
slug: 01-a-mark-that-moves
weight: 20
draft: false
course_kind: instructional
objectives:
  - Store position, rate, input, and pause state in variables with suitable numeric types
  - Explain how setup, update, draw, keyboard, and pointer callbacks divide responsibility
  - Convert a rate in pixels per second into fixed-step displacement
  - Compare variable-step motion with a fixed-step accumulator and clamp pause spikes
  - Build an accessible pointer-responsive traveler with wrap, fallback, pause, reset, and reduced motion
prerequisites:
  - Completion of section 00 or an equivalent generated openFrameworks 0.12.1 app
  - Ability to declare a function and read a compiler diagnostic
source_records: sources.yaml
asset_records: assets.yaml
---

# A mark that moves

## See what we're making

One still cannot prove motion, so this preview uses position and ruler ticks to show the
same traveler after equal simulation steps.

![Three equal panels place a yellow traveler at panel-local ruler positions 70, 100, and 130 after zero, 30, and 60 fixed steps at 120 pixels per second; a cyan trail points left.](media/traveler-time-preview.svg "Each 30-step interval advances 30 pixels on identical panel-local rulers.")

*At 120 pixels per second, each 30-step interval advances the mark by 30 pixels; position and ticks communicate the change without relying on color.*

The preview is static and has no audio. In the app, a line opposite velocity is a
non-color direction cue, pause adds a ring, and reduced motion changes speed.

## Take a guess

Assume `x` starts at `100.0f`, speed is `120.0f` pixels per
second, and one update receives `1.0f / 60.0f` seconds:

```cpp
x = x + speed * dt;
```

Predict the new `x`. Then predict what the same line does if `dt`
accidentally contains `16.67` **milliseconds** rather than `0.01667`
seconds. Keep both answers; the repair task makes this unit mismatch visible.

## Let's unpack it

### Before the motion vocabulary

Animation is a series of still pictures. The mark moves because the program remembers
its position, changes that number a little, and draws the next frame.

Suppose the mark starts at x = 100 and moves 120 pixels each second. If one update
represents 1/60 of a second, it moves 2 pixels during that update because
`120 × 1/60 = 2`. Its new x position is 102.

The saved position is called **state**. Changing it is an **update**. Using the same
small slice of time for every update is a **fixed time step**. Those names will recur,
but the underlying idea is only “remember, add a little, draw.”

### Variables and numeric types

A variable names a value that can be read and changed. The [fundamental-types reference](https://en.cppreference.com/w/cpp/language/types.html) is the
language authority; this section needs only a small working set:

```cpp
float x = 100.0f;       // fractional pixels
float speed = 120.0f;   // pixels per second
int pointer_x = 320;    // whole pixel supplied by a callback
bool paused = false;    // one of two states
```

A declaration combines a type, name, and initial value. `float` is appropriate
for fractional position, rate, and elapsed seconds. `int` matches the whole
pixel coordinates delivered by openFrameworks input callbacks. `bool` records a
yes/no mode. The one main C++ idea here is mutable state: variables survive from one
callback to the next as members of `ofApp` or fields of the renderer-independent
`traveler::State`.

### Callback roles

The [openFrameworks events documentation](https://openframeworks.cc/documentation/events/ofEvents.html) defines when callbacks are dispatched.

- `setup()` runs once and creates the initial model.
- `update()` changes model state once per frame; it never draws.
- `draw()` reads state and emits graphics; it never advances time.
- `mouseMoved()` and `mouseDragged()` store a pointer target.
- `keyPressed()` and `keyReleased()` store fallback direction or trigger pause, reset,
  and reduced-motion commands.

Input callbacks do not move the traveler immediately. They update variables; the next
fixed simulation step reads one complete input snapshot.

The course calls this thin layer between openFrameworks and the plain C++ calculations
an **adapter**. It receives framework events, passes ordinary values into the model, and
draws the model's answer. Keeping it thin lets recorded input use the same calculations
without opening a window.

### Turn speed and time into movement

```text
start x                         next x
  100 px ├────────── 2 px ───────► 102 px
          speed = 120 px/s, time = 1/60 s
```

Numerically:

```text
120 pixels/second × 1/60 second = 2 pixels
```

The reusable version of that same calculation is:

```text
next position = current position + velocity × elapsed seconds
```

In code and math notes, you may see the shorter names `p` for position,
`v` for velocity, and `dt` (or `Δt`) for elapsed seconds.
The names change; the `100 + 120 × 1/60 = 102` calculation does not. Seconds cancel seconds,
leaving pixels. The adapter uses [`ofGetLastFrameTime()`](https://openframeworks.cc/documentation/utils/ofUtils.html#show_ofGetLastFrameTime), which reports seconds. Do not multiply
it by 1000. At 120 pixels/second, `16.67` used as seconds would request roughly
2000 pixels in one frame—the Predict unit bug.

### Why this exercise does not use a different time step every frame

A direct variable-step update is compact:

```cpp
position += velocity * frame_seconds;
```

But steering direction depends on current position. One 1/30-second update can sample
that direction fewer times than four 1/120-second updates, so the paths can differ even
when elapsed time is equal. Floating-point rounding and collision/boundary decisions can
also move to different frames.

The course model retains `advanceVariable(...)` only for comparison. Its actual rule follows the
fixed-step accumulator technique described in [Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/):

```text
frame = clamp(valid_seconds, 0, 0.1)
accumulator += frame
while accumulator >= 1/120:
    simulate exactly 1/120 second
    accumulator -= 1/120
```

`NaN` or infinite, zero, and negative frame durations produce no motion. Every
valid positive frame clamps to `0.1` second before accumulation, so returning
after a long pause performs at most twelve fixed steps rather than seconds of frantic
catch-up. Pause discards the accumulator. A remainder smaller than one fixed step waits
for the next frame. This is a simulation rule, not a promise that the window renders at
120 frames per second.

### Pointer, keyboard, wrap, and reduced motion

At each step the pointer vector is `target - position`; the model normalizes it and
multiplies by your chosen speed. Before a pointer event, a rightward keyboard direction
is the fallback. Pressing an arrow key disables pointer steering and supplies a
direction that works without a pointing device.

The center may leave by one radius so the mark disappears completely. It then wraps,
preserving overshoot, into the interval `[-radius, extent + radius)`.
This avoids sticking at an edge. Space toggles pause, `R` resets deterministic
motion state, and `M` uses one quarter speed. Reduced motion is a persistent
preference, so reset does not silently turn it off.

## Make it run

Check the sketch in layers: test the arithmetic, generate the project, compile it,
then open the window and look at the result.

### Example 1: one variable-step calculation

```cpp
float x = 100.0f;
const float speed = 120.0f;
const float dt = 1.0f / 60.0f;
x = x + speed * dt;  // 102.0f, within floating-point tolerance
```

This answers Predict for seconds. It demonstrates the equation, not the final
frame-time policy.

### Example 2: fixed-step values you can print and test

Read `advanceFrame(...)` in
[`traveler_model.cpp`](../../../exercises/01-a-mark-that-moves/shared/traveler_model.cpp). The public test supplies exactly one
`1/120`-second frame at 120 pixels/second and observes a one-pixel change. Then
it compares one 1/30-second frame with four 1/120-second frames. Run it without
openFrameworks:

```sh
tests/run-section-01-tests.sh
```

### Example 3: generated starter app

```sh
scripts/section-01.sh generate --project starter
scripts/section-01.sh build --project starter --configuration Release
```

On Windows Developer PowerShell:

```powershell
.\scripts\section-01.ps1 generate -Project starter
.\scripts\section-01.ps1 build -Project starter -Configuration Release
```

The wrappers keep Project Generator in charge of generated files, verify which source
files belong to the project, and reuse section 00's cleanup safeguards. These commands
are checked on the course's named Linux, macOS, and Windows setups. A build on one system
cannot tell you what happens on another, and no compiler can judge the picture.

## Break it on purpose

In `exercises/01-a-mark-that-moves/starter/src/ofApp.cpp`, temporarily change:

```cpp
ofGetLastFrameTime()
```

to:

```cpp
ofGetLastFrameTime() * 1000.0f
```

This compiles: types cannot identify a mistaken unit. Predict the symptom, run the app
briefly, and observe that the 0.1-second clamp prevents an unlimited jump but motion
still advances at the clamp on almost every frame. Repair by removing `* 1000.0f`;
source content changed, so do not regenerate. If this was the only edit you meant to
discard, restore exactly that file with:

```sh
git restore -- exercises/01-a-mark-that-moves/starter/src/ofApp.cpp
```

This command discards every uncommitted change in that file. Before moving on, make sure
you can name the wrong unit, visible behavior, limiting rule, and repair.

## Your turn

Open the [traveler brief](../../../exercises/01-a-mark-that-moves/README.md). Choose start, speed, radius, and colors in the starter design
file, then make a silhouette of your own or geometric trail. The starter is a circle with one
trail line; the explained solution is a diamond with two parallel trail lines. Choose
geometry distinct from both while preserving the repeatable public model and controls:
pointer response, arrow fallback, wrap, pause, reset, and reduced motion. There is no
target screenshot.

## Check your work

Run the pure C++17 test before and after each model/design change:

```sh
tests/run-section-01-tests.sh
```

Or in Windows Developer PowerShell:

```powershell
.\tests\run-section-01-tests.ps1
```

Known cases test one fixed `dt` and reset. The frame-partition property
compares equal elapsed time; boundary tests cover wrap with overshoot and a long run.
Pause-spike tests cover the 0.1-second clamp, pause accumulator clearing, and
`NaN` or infinite time. The starter tests compile the starter design and
checks finite/ranged choices, distinct mark/background RGB values, initial state,
quarter-speed reduced motion, and reduced-motion persistence through reset. Sufficient
visual contrast remains a manual review item. The explicit fixture inventory names all
four streams. No test reads the clock, pointer, window, renderer, or pixels.

Then generate and build both configurations. Open the app and inspect the window; a
successful build cannot tell you whether the motion and composition look right.

## Optional notes for future you

Explain what the accumulator keeps and how one 1/30-second frame becomes four
1/120-second steps. Name one visual choice you made and one accessibility control.
Include the unit bug you repaired and save a still capture with alt text.

## Make it yours

Keep the fixed-step rule and controls, but change one spatial behavior: steer away from
the pointer, constrain velocity to horizontal/vertical axes, or map pointer distance to
speed within the documented range. Predict how the frame-partition test should behave
before editing. The remix must change mapping or geometry, not only palette.

## Quick visual check

- Pointer and arrow fallback each produce an immediately legible response.
- Space pauses without a resume jump; reset repeats the start; reduced motion is visibly
  slower and remains enabled after reset.
- Wrap is clean on all edges and after narrow/wide resize.
- A line, silhouette, or placement communicates motion without color alone.
- Contrast is suitable and no flashing or rapid full-field changes occur.
- The result differs from starter, preview, and solution in silhouette, trail, mapping,
  or spatial behavior—not merely palette.
- The capture alt text names mark position, direction cue, and relationship.
- Reused code and assets remain credited and licensed.

## If you get stuck

If the mark jumps, freezes, or vanishes, pause before rewriting everything. Print or
inspect position, speed, and `dt` one at a time. Check the units first:
seconds and milliseconds are not cousins who can safely share a wardrobe. Then rerun the
smallest model test and make one change.
