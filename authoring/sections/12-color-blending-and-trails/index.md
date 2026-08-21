---
title: Color, blending, and trails
slug: 12-color-blending-and-trails
weight: 140
draft: false
course_kind: instructional
objectives:
  - Represent straight RGBA colors and two endpoint palettes as checked data in the unit interval
  - Interpolate palette channels and calculate source-over alpha composition as weighted averages
  - Apply limited exponential alpha decay to repeatable oldest-to-newest trail marks
  - Keep pure trail-plan output independent from renderer state and keep openFrameworks blend and style changes scoped
  - Design an accessible two-palette particle or gesture study whose shapes, fading, and overlap are choices you make
prerequisites:
  - Completion of section 09 or section 08, or equivalent limited histories, interpolation, finite guards, and openFrameworks event adapters
  - Ability to read a small struct, std::vector, loops, and normalized values from 0 through 1
source_records: sources.yaml
asset_records: assets.yaml
---

# Color, blending, and trails

## See what you're making

![On a dark navy field, a cyan-to-mint trail marked by crosses and a violet-to-amber trail marked by diamonds loop through each other; older marks are smaller and more transparent, central overlap is pale and bright, and a labeled five-step alpha key shows age without relying on color.](media/trail-preview.svg "Two palettes cross while alpha and size reveal age.")

*Two palettes cross; shape identifies each path, while size and alpha reveal age and overlap.*

The crosses and diamonds identify palettes without color. The labeled size key makes
old-to-new order readable even if transparency or hue differences are hard to see. The
preview is an original static SVG; different graphics hardware may draw the live version
a little differently.

## Take a guess

A half-visible red mark `(1, 0, 0, 0.5)` is drawn over solid blue `(0, 0, 1, 1)`.
Predict the resulting alpha and RGB numbers. The formal name **straight alpha** comes
later. A mark starts with alpha `0.75` and
retains `0.8` each sample: what is its alpha at age 2? If a palette runs from
`(0.1, 0.2, 0.3, 1)` to `(0.9, 0.6, 0.3, 0.5)`, what is its midpoint? Decide which answer describes
model data and which still depends on a renderer.

## Let's unpack it

### Before the color vocabulary

Each color has red, green, blue, and alpha values. Alpha says how strongly the new color
covers what is already there: 0 means invisible, 1 means fully opaque, and 0.5 means
halfway.

A trail keeps older marks and draws them again with less alpha. If each age step keeps
80% of the previous strength, the values go `1.0`, `0.8`,
`0.64`, and so on. That repeated multiplication is called **exponential decay**.
The name is formal; the visual result is simply a smooth fade into the past.

Blending order matters because “red over blue” mixes from a different starting color
than “blue over red.” You will work through actual channel numbers before using the
general formula.

### Color is checked data

`Color` stores straight red, green, blue, and alpha channels in `[0, 1]`.
Straight means RGB has not already been multiplied by alpha. `Palette` stores two
colors. `paletteColor(palette, amount, output)` rejects an amount that contains `NaN`, infinity, or
falls outside the allowed range and otherwise interpolates every channel:

```text
channel(t) = first_channel + (second_channel - first_channel) * t
```

At `t = 0`, the exact first endpoint is returned. At `t = 1`, the exact
second endpoint is returned. At `t = 0.5`, the Predict palette becomes
`(0.5, 0.4, 0.3, 0.75)`. This is both a numerical weighted average and a small palette function.
Tests sample 101 amounts across each palette and check that every channel
remains between its endpoints and within `[0, 1]`.

The plain `Color`, `Palette`, `TrailSample`, and `TrailMark` records are
the section's data boundary. No renderer color, global random source, wall clock, or GPU
state enters the pure C++17 model.

### Alpha composition is a weighted average

Alpha means coverage, not brightness. Start with a concrete case: draw red at half
strength over solid blue. Half of the result comes from red and half from blue, so the
mixed color is purple:

```text
output alpha = 0.5 + 1.0 × (1 - 0.5) = 1.0
output color = red × 0.5 + blue × 0.5 = (0.5, 0, 0.5)
```

The general version uses short names:

- `Cs` is the new source color and `As` is its alpha;
- `Cb` is the existing background color and `Ab` is its alpha; and
- `Co` and `Ao` are the output color and alpha.

```text
Ao = As + Ab × (1 - As)
Co = (Cs × As + Cb × Ab × (1 - As)) / Ao
```

If `Ao` is zero, the helper returns transparent black. The [W3C compositing standard](https://www.w3.org/TR/compositing-1/)
defines this source-over operation and separates it from artistic blend modes. Real
screen pixels also depend on color space, antialiasing, and graphics settings, so the
course tests these channel calculations rather than comparing screenshots.

### Decay turns age into controlled persistence

The newest sample has age zero. If a trail retains fraction `r` of alpha per
sample, an age-`n` mark has:

```text
alpha(n) = initial_alpha * r^n
```

For example, starting at alpha `0.75` and keeping 80% per age step gives
`0.75 × 0.8 × 0.8 = 0.48` at age 2. If the keep fraction stays between 0 and 1, each older mark is
no stronger than the mark before it. The model rejects bad numbers, limits history to
4,096 samples, calculates age from list order, and returns ordinary color and radius
values.

`buildTrailMarks(history, design, output)` first builds a new list on the side. It
replaces `output` only after every sample and mark passes its checks. Bad input therefore
leaves the previous good output unchanged. An unrelated trail, drawing style, blend
mode, or clock value cannot change the calculated marks.

### Composition and blend mode are different decisions

The pure model provides source-over arithmetic as a lesson-sized known case. The starter
adapter requests normal alpha blending; its filled circles and open squares remain two
non-color palette cues. The solution requests additive blending; crosses and diamonds
form a sparse luminous star loom. Additive blending is intentionally a renderer
decision, not smuggled into palette or decay functions.

The openFrameworks adapter follows the [ofGraphics state API](https://openframeworks.cc/documentation/graphics/ofGraphics/):

```cpp
ofPushStyle();
ofEnableBlendMode(OF_BLENDMODE_ADD);
// draw only from repeatable TrailMark values
ofDisableBlendMode();
ofPopStyle();
```

Matrix changes in the solution are also paired with push/pop calls. Help text uses a
separate style scope. A draw call never owns the only copy of trail age, palette
identity, alpha, or radius.

### Two palettes require two roles, not just more hues

Each input sample carries palette index 0 or 1 and interpolation amount. In the starter,
the two repeatable paths adapt the memory idea from section 09; pointer/drag and arrows
move their shared origin. Palette A uses circles and palette B squares. The solution
changes path equations, history length, decay, size, background, both palettes, blend
mode, sampling rhythm, and geometry. It uses crosses for A and diamonds for B. You must
create a third visual grammar rather than recolor either example.

P pauses, R clears and replays the initial phase, and M keeps current marks only.
Pointer and keyboard routes alter the same visible origin. Very small windows suppress
origin movement safely. Nothing flashes, and no meaning is audio-only.

## Make it run: inspect three complete experiments

### 1. Replay the independent arithmetic oracle

Linux x86-64 or macOS arm64:

```sh
cat exercises/12-color-blending-and-trails/fixtures/trail-oracle.txt
CXX=g++ tests/run-section-12-tests.sh
```

The hand-computable rows pin palette start/middle/end, half-red over blue, and two decay
steps. The suite also proves malformed fixture rows fail through cardinality checks
before field indexing. Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
Get-Content .\exercises\12-color-blending-and-trails\fixtures\trail-oracle.txt
.\tests\run-section-12-tests.ps1
```

### 2. Trace one repeatable trail plan

For three samples, model age is `2, 1, 0` from oldest to newest. With radii 2 to
10, marks receive radii `2, 6, 10`. With opaque palette colors, base opacity
`0.75`, and retention `0.8`, their alphas are `0.48, 0.60, 0.75`. Find
those assertions in `color_trail_model_test.cpp`, then compare them with the symbolic decay equation.
Palette color alpha can lower those values further.

### 3. Compare the visibly divergent adapters

Set `OF_ROOT` to openFrameworks 0.12.1. The course supplies checked commands for Linux
x86-64, macOS arm64, and Windows Visual Studio 2022 x64 Developer PowerShell. On another
system, the sketch may work, but you may need to adapt the build steps.

Linux x86-64:

```sh
scripts/section-12.sh generate --project starter
scripts/section-12.sh build --project starter --configuration Release
exercises/12-color-blending-and-trails/starter/bin/starter
```

macOS arm64:

```sh
scripts/section-12.sh generate --project solution
scripts/section-12.sh build --project solution --configuration Release
open exercises/12-color-blending-and-trails/solution/bin/solution.app
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\scripts\section-12.ps1 generate -Project starter
.\scripts\section-12.ps1 build -Project starter -Configuration Release
& .\exercises\12-color-blending-and-trails\starter\bin\starter.exe
```

Launch both projects, not just one. Normal-alpha paired ribbons and the additive star
loom should be visibly divergent. Compilation cannot prove launch, contrast, additive
appearance, or scoped state after a draw.

## Break it on purpose

In `exercises/12-color-blending-and-trails/shared/color_trail_model.cpp`, temporarily change:

```cpp
const std::size_t age = history.size() - 1 - index;
```

to:

```cpp
const std::size_t age = history.size() - index;
```

Predict why even the newest point now decays once. Run `tests/run-section-12-tests.sh` and locate the
first age or alpha failure. Restore the original expression and rerun. If this was your
only edit:

```sh
git restore -- exercises/12-color-blending-and-trails/shared/color_trail_model.cpp
```

That command discards every uncommitted change in the named file. Before moving on, make
sure the off-by-one failure makes sense.

## Your turn

Open the [two-palette trail study brief](../../../exercises/12-color-blending-and-trails/README.md). Start from the particle-memory or gesture idea: keep limited
oldest-to-newest samples, but create two explicit palette roles and controlled
persistence. Edit `starter/src/design/trail_design.cpp` for endpoints, decay, opacity, radii, and history. Edit
`starter/src/ofApp.cpp` for motion/gesture source, blend mode, geometry, and composition.
Preserve checked normalized channels, finite guards, pure plan output, keyboard access,
reset, reduced motion, and scoped renderer state.

## Check your work

Linux x86-64 or macOS arm64:

```sh
CXX=g++ tests/run-section-12-tests.sh
CXX=clang++ tests/run-section-12-tests.sh
bash -n scripts/section-12.sh tests/run-section-12-tests.sh
shellcheck scripts/section-12.sh tests/run-section-12-tests.sh
scripts/check-authoring.sh
```

Use `tests/run-section-12-tests.ps1` on Windows. With `OF_ROOT`, generate and compile starter and
solution in Debug and Release, then launch each manually. The pure suite covers known
cases, exact boundaries, channel and composition properties, invalid/`NaN` or
infinite safety, endpoint preservation, decay monotonicity, history caps, bad-input handling that preserves the previous output, and state-independent replay.
It does not compare the pixels drawn by the graphics card.

## Optional notes for future you

Explain the difference between RGB and alpha, then show either your red-over-blue
calculation or one trail-decay value. Name one non-color cue and one visual decision you
made. Save a capture with alt text.

## Make it yours

Use a gesture instead of autonomous particles, map speed to interpolation amount, make
one palette cool-to-warm and the other low-to-high value, compare normal and additive
blend in separate panels, quantize ages into print-like bands, or use decaying line
width while keeping alpha fixed. Preserve two non-color roles, limited history, valid
channels, explicit age, repeatable plan output, scoped style/blend/matrix state, reset,
keyboard access, and a reduced-motion result.

## Quick visual check

Automated tests cannot judge the finished picture, so open both apps and use the list
below. If you are sharing a platform-specific rendering bug, noting your OS,
openFrameworks version, graphics hardware, and build type may help someone reproduce
it.

- Starter uses normal alpha blending; solution uses additive blending; their overlap,
  geometry, density, paths, and background visibly diverge.
- Palette A and B remain identifiable from circles/squares or crosses/diamonds when
  viewed in grayscale or with hue differences hidden.
- Old-to-new order remains readable through alpha plus size or spacing, not color alone.
- P pauses, R resets phase and histories, M removes persistence, and pointer/drag and
  arrows move the same visible origin.
- Text contrast remains readable; no flashing or audio-only information appears.
- Resize and very small windows remain safe; marks do not produce obvious cross-window
  artifacts.
- Drawing after the trail block shows no leaked blend mode, fill, line width, color, or
  matrix transform.
- Your result differs from both examples in path/gesture source, palette roles,
  geometry, decay, density, composition, and blend treatment.
- Capture alt text names both shape roles, overlap, age ordering, palette roles,
  controls, and reduced-motion behavior.
- Any reused palette, code, reference, or asset is credited.

## If you get stuck

If a trail becomes a muddy rectangle, check alpha, decay, and renderer state in that
order. Clear the frame, draw one mark, then add persistence back in. Color mixing is
weighted arithmetic with better lighting; a tiny two-color example will usually reveal
the mistake faster than a full particle orchestra.
