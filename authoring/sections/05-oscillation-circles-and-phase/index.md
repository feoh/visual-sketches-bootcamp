---
title: Oscillation, circles, and phase
slug: 05-oscillation-circles-and-phase
weight: 70
draft: false
course_kind: instructional
objectives:
  - Use a circle as a visual calculator for moving a point around a center
  - Explain sine, cosine, radians, amplitude, frequency, phase, and period in plain language
  - Convert between center/radius/angle and ordinary x/y coordinates with a documented zero policy
  - Build a repeated row/column field with deterministic phase offsets and fixed time
  - Test conversions, quarter-turns, periodicity, bounds, and learner choices without pixels
prerequisites:
  - Completion of section 04 or equivalent C++ structs, vectors, pure functions, and tolerance literacy
  - Basic algebra: substitute values into a formula and solve a simple equation
source_records: sources.yaml
asset_records: assets.yaml
---

# Oscillation, circles, and phase

Circles are about to become a very handy motion machine. If the words *sine*,
*cosine*, and *radians* currently look like alphabet soup, excellent: this is
the lesson where we make them useful. You will not need to memorize a trig
table or prove anything about triangles. We will use a picture, a few friendly
number patterns, and formulas that tell the computer where to put a mark.

## See what we're making

![A four-column, three-row field uses outlined orbit paths, crosshair centers, and filled travelers at progressively shifted phases, with a labeled circular inset showing cosine on x, sine on y, and positive-down screen coordinates.](media/phase-field-preview.svg "Nested phase field and circular coordinates.")

*Nested rows and columns repeat one circular rule while phase offsets create a traveling rhythm.*

The preview is static and has no audio. Outlined paths, crosshair centers, filled
travelers, labels, and position—not color alone—explain the field.

## Take a guess

No trig knowledge is required for this guess. Draw a circle with a point at the
right-hand side. If the point moves one quarter-turn at a time, where do you
expect it to be after one, two, and three quarter-turns? Does it return to the
same spot after a full turn? Keep the sketch nearby—we will use it as our
cheat sheet instead of asking your memory to do all the heavy lifting.

## Let's unpack it

### Start with one full turn

A full circle is 360 degrees, or one turn. Computers can also measure that
same turn with a unit called **radians**. The name sounds grander than the
idea: `2*pi` radians is one full turn. We will use `tau` as a friendly name for
that amount so “one turn” is easy to spot in code.

Do not scatter rounded `3.14` values through a program. Give the relationships
names:

```cpp
constexpr float pi = 3.14159265358979323846f;
constexpr float tau = 2.0f * pi;
constexpr float fixed_dt = 1.0f / 60.0f;
```

`constexpr` says these values are available during compilation and cannot be
changed accidentally. `tau` makes “one full turn” visible in formulas.
`fixed_dt` makes each update an inspectable sixtieth of a second.

### Degrees and radians are two labels for the same turn

Degrees are the labels we use in everyday conversation: a right angle is 90
degrees and a full turn is 360. Radians are the labels the C++ math functions
expect. You can switch labels with ordinary algebra:

```text
radians = degrees * pi / 180
degrees = radians * 180 / pi
```

So 90 degrees is `pi/2` radians, 180 degrees is `pi`, and 360 degrees is
`tau`. You do not have to choose a favorite unit; just convert at the door and
keep the rest of the calculation consistent. The course model keeps
`degreesToRadians()` and `radiansToDegrees()` explicit so tests can check both
directions. Non-finite input returns zero by policy rather than passing NaN
into geometry.

### Sine and cosine are two tiny number machines

Here is the useful, non-mysterious version. Give **cosine** an angle and it
returns the point's horizontal share of a unit circle. Give **sine** the same
angle and it returns the vertical share. At the four easy quarter-turns, their
answers are close to this table:

| Angle | `cos(angle)` | `sin(angle)` | Where the point is |
|---|---:|---:|---|
| `0` | `1` | `0` | right |
| `pi/2` (90°) | `0` | `1` | down in an openFrameworks window |
| `pi` (180°) | `-1` | `0` | left |
| `3*pi/2` (270°) | `0` | `-1` | up in an openFrameworks window |

The formulas for a circle of radius `r` are simply:

```text
x offset = r * cos(angle)
y offset = r * sin(angle)
```

Then add those offsets to the center. That is the whole trick: cosine handles
left/right, sine handles up/down, and the radius scales both. The functions
[`std::sin`](https://en.cppreference.com/w/cpp/numeric/math/sin.html) and
`std::cos` fill in the values between those four easy cases. Their answers
repeat after `2*pi`, so one full turn lands back where it started.

Near a cardinal zero, a computer may report a tiny value such as `-4e-8`
instead of exactly `0`. That is normal floating-point fuzz, not a haunted
circle. Tests compare approximately.

### Two useful ways to describe a point

Most of the time we use **Cartesian coordinates**: `(x, y)` tells us the
horizontal and vertical position directly. For circles, **polar coordinates**
are often friendlier: `(radius, angle)` says “go this far from the center in
this direction.”

```cpp
struct Polar { float radius; float angle_radians; };
Vec2 offset = polarToCartesian({radius, angle});
Vec2 point{center.x + offset.x, center.y + offset.y};
```

Radius is the reach; angle is the direction. `polarToCartesian()` does the
cosine/sine bookkeeping for us. To go the other way, the computer measures the
reach with `hypot(x, y)` and asks `atan2(y, x)` for the angle:

```text
radius = hypot(x, y)
angle = atan2(y, x)
```

You can think of [`std::atan2`](https://en.cppreference.com/w/cpp/numeric/math/atan2.html)
as a direction finder that looks at both x and y, so it knows all four
quadrants. We do not need to derive it. We only need to know that it is safer
than typing `atan(y/x)`, especially when x is zero.

The zero vector `(0,0)` has a reach of zero but no meaningful direction. Our
explicit policy is `cartesianToPolar({0,0}) == {0,0}`. That is a useful edge
case to name, not a failure to hide.

### Make a circle breathe and travel

Once the circle helpers work, a repeating motion is just a formula. Here is the
shape we will use:

```text
value = amplitude * sin(tau * frequency * time + phase)
```

The vocabulary is friendlier than it first appears:

- **Amplitude**: how far the mark moves from its center.
- **Frequency**: how many cycles happen each second.
- **Period**: how long one cycle takes, so `period = 1 / frequency`.
- **Phase**: where in the cycle a mark starts. It is a starting offset, not a new kind of force.

For amplitude `7` and frequency `2` cycles per second, solve the small equation
`period = 1 / 2`: one cycle takes `0.5` seconds. After a quarter period,
`0.125` seconds, the mark reaches its maximum displacement. Add one full turn
to the phase or one full period to time and the value comes back around.

A field becomes interesting when neighboring marks use different starting
phases. They all follow the same rule, but they are not marching in lockstep.
That is how one boring repeated instruction turns into a wave. The model rejects
negative amplitude/frequency in its general helper; exercise designs use a
small positive frequency so every field has a clear period.

### Nested loops make a field

One loop walks rows; an inner loop walks columns:

```cpp
for (int row = 0; row < rows; ++row) {
    for (int column = 0; column < columns; ++column) {
        int index = row * columns + column;
        // make exactly one mark
    }
}
```

A 3-by-4 field makes 12 marks with indices 0 through 11. The mark at row 1,
column 2 has index `1*4+2 = 6`. Row and column are not interchangeable:
columns control horizontal spacing, while rows control vertical spacing.
`flatIndex()` returns `-1` for invalid coordinates instead of indexing memory.

Each field mark receives a phase:

```text
phase = tau * frequency * time
      + row * row_phase_step
      + column * column_phase_step
```

Then `polarToCartesian({amplitude, phase})` supplies a circular displacement
around that mark's base. The same rule repeats; row/column offsets make a wave
seem to travel through the field. No randomness or frame-clock lookup exists
inside the model.

### Deterministic time at the renderer boundary

The openFrameworks adapter advances `time_seconds` by `fixed_dt` in `update()`
and passes it to pure `makeScene()`. Space or `P` pauses; `R` resets through
[`keyPressed`](https://openframeworks.cc/documentation/application/ofBaseApp/#!show_keyPressed).
Those controls offer a keyboard-operable way to inspect exact phases.

This fixed update is intentionally introductory. If rendering stalls, visual
time slows instead of silently taking a giant step. The standard-library C++17
model accepts an explicit time, so identical inputs replay identically on
Linux, macOS, and Windows. Native compilation does not prove that a graphical
runtime was launched.

### Keep strokes inside tiny viewports

Visible bounds include mark radius and half the 3-pixel stroke, plus a 2-pixel
outer margin. A base point also reserves the full amplitude so its traveler can
move in any direction:

```text
base inset = amplitude + mark radius + 1.5 + 2
mark inset = mark radius + 1.5 + 2
```

Base positions are evenly interpolated between opposite base insets. Even in a
valid `64 x 64` viewport, every traveler's stroke remains inside. A viewport
smaller than `64` in either dimension, invalid design, or non-finite time
returns `Scene{valid=false}`. Validation checks each base and its full orbit or
capped crosshair extent as well as each traveler center and mark extent. Tests
also calculate the circle, crosshair, and diamond extrema independently under
minimum and maximum learner parameters; they do not compare pixels.

### Learner-owned design, shared safety

`Design` owns rows, columns, amplitude, frequency, row/column phase steps, mark
radius, and palette. These choices change count, geometry, rhythm, and color.
The shared model owns conversion rules, indexing, phase math, bounds, and
invalid-input policy. Public tests compile the starter's
`makePhaseFieldDesign()`, so an out-of-range learner choice produces an
actionable failure.

The starter uses orbit outlines, crosshairs capped at the mark radius, and
filled circular travelers. The explained solution uses connected row threads
and alternating filled/outlined diamonds drawn from four axis vertices exactly
one mark radius from the center. A successful learner result should make a
third spatial or shape relationship rather than recolor either one.

### Approximate tests, independent oracles

The fixture contains hand-calculated values for a 3-by-4 field: viewport,
time, row, column, flat index, base, phase, and center. Its reader requires
exactly twelve fields per data row. Tests report actual, expected, difference,
absolute tolerance `0.002`, and relative tolerance `0.000001`.

Properties also check conversion, four cardinal points, four `atan2` quadrants,
zero policy, wave parameters, count/order, phase and time periodicity,
determinism, parameter variation, non-finite policy, and tiny/stroke bounds.
There is no screenshot target or pixel gate.

## Try the numbers

Use the table and ordinary algebra. It is completely fine to write the circle
on paper first.

1. Convert 90 degrees: `90*pi/180 = pi/2` radians.
2. Convert `pi` radians: `pi*180/pi = 180` degrees.
3. Radius 10 at `pi/2`: the table says `(cos,sin) ≈ (0,1)`, so the offset is approximately `(0,10)`.
4. A point `(-3,4)` is 5 units from the center because `sqrt(3² + 4²) = 5`; `atan2` places it in quadrant two.
5. For amplitude 6 and frequency 0.5 cycles per second, solve `period = 1 / 0.5`: the period is `2` seconds.
6. In 5 rows by 7 columns, count is `35`; row 3, column 2 has index `23`.

Sketch the circle and grid before running code. The picture explains direction;
the numbers check the picture; the tests catch the tiny cases we would rather
not debug by staring at a moving window.

## Break it on purpose

In the exact tracked file
`exercises/05-oscillation-circles-and-phase/shared/phase_field_model.cpp`,
temporarily change:

```cpp
polar.radius * std::sin(polar.angle_radians)
```

to:

```cpp
polar.radius * std::cos(polar.angle_radians)
```

Run `tests/run-section-05-tests.sh`. Predict which cardinal, quadrant
round-trip, fixture-center, and periodic properties fail. Read the component
diagnostics, restore sine for y, and rerun. If this was your only intended edit:

```sh
git restore -- exercises/05-oscillation-circles-and-phase/shared/phase_field_model.cpp
```

That command discards every uncommitted change in the named file. Record the
failure, the circle-coordinate explanation, and the repaired result.

## Your turn

Open the [phase-field brief](../../../exercises/05-oscillation-circles-and-phase/README.md).
Edit exactly
`exercises/05-oscillation-circles-and-phase/starter/src/design/phase_field_design.cpp`
first. Choose valid grid, amplitude, frequency, row/column phase, radius, and
palette values. Then edit `starter/src/ofApp.cpp` to create learner-owned marks
and relationships while retaining pause/reset controls and pure model calls.

Do not copy the starter or solution and only change colors. Consider arcs,
short directional strokes, paired marks, alternating scales, or negative-space
bands. Predict count, first two phases, period, and maximum extent before
building.

## Check your work

On Linux or macOS, use both available compilers:

```sh
CXX=g++ tests/run-section-05-tests.sh
CXX=clang++ tests/run-section-05-tests.sh
```

On Windows Developer PowerShell:

```powershell
.\tests\run-section-05-tests.ps1
```

Generate and compile starter and solution in Debug and Release before a
release claim. Launch manually at `64 x 64`, narrow, square, and wide sizes;
pause, resume, reset, and watch a complete period. Pure Linux/Windows runners
and native Linux/macOS/Windows CI prove their named numerical/compilation
contracts only. They do not prove graphical runtime, accessibility, contrast,
or originality.

## Tell the story

In 80–120 words, explain how the circle table turns an angle and radius into a
position. Mention one new word—perhaps amplitude, frequency, period, phase, or
polar coordinates—using your own example. Include one periodicity check, one
boundary choice, and one visual decision you made. Add alt text for a capture.
If your explanation sounds like a textbook, rewrite it as if you were telling a
friend why the marks move.

## Make it yours

Keep the model contract but change a relationship: map row to amplitude, use
alternating frequency signs in your renderer, connect equal-phase neighbors,
or draw only marks near a cardinal direction. Predict cardinal, periodic,
count, and boundary consequences before editing.

## Quick visual check

- Pause/resume/reset work by keyboard, and no pattern flashes.
- Mark roles and phase relationships remain understandable without color alone.
- Ink/background and accent/background contrast are suitable.
- Geometry remains legible at narrow, square, wide, and `64 x 64` sizes.
- Geometry or spatial behavior differs from starter and solution, not only palette.
- Capture alt text names the grid, phase direction, shape encoding, and viewport.
- Reused code and assets remain credited and license-compatible.

## If you get stuck

Keep the circle table visible. Check one quarter-turn before investigating a
whole animated field: is cosine controlling x, is sine controlling y, and are
you using radians consistently? If the mark is almost—but not exactly—at zero,
remember that floating-point numbers enjoy being technically correct in tiny,
annoying ways. Use the approximate comparison helper.
