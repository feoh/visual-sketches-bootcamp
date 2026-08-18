---
title: Color, blending, and trails
slug: 12-color-blending-and-trails
weight: 140
draft: false
course_kind: instructional
objectives:
  - Represent straight RGBA colors and two endpoint palettes as checked data in the unit interval
  - Interpolate palette channels and calculate source-over alpha composition as weighted averages
  - Apply bounded exponential alpha decay to deterministic oldest-to-newest trail marks
  - Keep pure trail-plan output independent from renderer state and keep openFrameworks blend and style changes scoped
  - Design an accessible two-palette particle or gesture study whose geometry, decay, and overlap are learner-owned
prerequisites:
  - Completion of section 09 or section 08, or equivalent bounded histories, interpolation, finite guards, and openFrameworks event adapters
  - Ability to read a small struct, std::vector, loops, and normalized values from 0 through 1
source_records: sources.yaml
asset_records: assets.yaml
---

# Color, blending, and trails

## Look

![On a dark navy field, a cyan-to-mint trail marked by crosses and a violet-to-amber trail marked by diamonds loop through each other; older marks are smaller and more transparent, central overlap is pale and bright, and a labeled five-step alpha key shows age without relying on color.](media/trail-preview.svg "Two palettes cross while alpha and size reveal age.")

*Two palettes cross; shape identifies each path, while size and alpha reveal age and overlap.*

The crosses and diamonds identify palettes without color. The labeled size key
makes old-to-new order readable even if transparency or hue differences are
hard to see. The preview is an original static SVG, not a captured claim that
all GPUs produce identical pixels.

## Predict

A straight-alpha red mark `(1, 0, 0, 0.5)` is drawn over opaque blue
`(0, 0, 1, 1)`. Predict the output alpha and RGB channels. A mark starts with
alpha `0.75` and retains `0.8` each sample: what is its alpha at age 2? If a
palette runs from `(0.1, 0.2, 0.3, 1)` to `(0.9, 0.6, 0.3, 0.5)`, what is its
midpoint? Decide which answer describes model data and which still depends on a
renderer.

## Learn

### Color is checked data

`Color` stores straight red, green, blue, and alpha channels in `[0, 1]`.
Straight means RGB has not already been multiplied by alpha. `Palette` stores
two colors. `paletteColor(palette, amount, output)` rejects a non-finite or
out-of-range amount and otherwise interpolates every channel:

```text
channel(t) = first_channel + (second_channel - first_channel) * t
```

At `t = 0`, the exact first endpoint is returned. At `t = 1`, the exact second
endpoint is returned. At `t = 0.5`, the Predict palette becomes
`(0.5, 0.4, 0.3, 0.75)`. This is both a numerical weighted average and a small
palette function. Tests sample 101 amounts across each learner palette and
check that every channel remains between its endpoints and within `[0, 1]`.

The plain `Color`, `Palette`, `TrailSample`, and `TrailMark` records are the
section's data boundary. No renderer color, global random source, wall clock,
or GPU state enters the pure C++17 model.

### Alpha composition is a weighted average

Alpha is coverage/openness, not brightness. For source color `Cs` with alpha
`As` over backdrop `Cb` with alpha `Ab`, source-over composition is:

```text
Ao = As + Ab * (1 - As)
Co = (Cs * As + Cb * Ab * (1 - As)) / Ao
```

When `Ao` is zero, this model returns canonical transparent black. The
[W3C compositing standard](https://www.w3.org/TR/compositing-1/) defines the
source-over operation and distinguishes composition from artistic blend modes.
For half-alpha red over opaque blue:

```text
Ao = 0.5 + 1.0 * (1 - 0.5) = 1.0
Co = (red * 0.5 + blue * 0.5) / 1.0 = (0.5, 0, 0.5)
```

The preview shows this idea visually as a pale overlap, the arithmetic supplies
a known numerical case, and the equations state it symbolically. Actual pixels
also depend on color space, framebuffer format, antialiasing, and renderer
implementation, so the course tests the arithmetic rather than screenshots.

### Decay turns age into controlled persistence

The newest sample has age zero. If a trail retains fraction `r` of alpha per
sample, an age-`n` mark has:

```text
alpha(n) = initial_alpha * r^n
```

Numerically, `0.75 * 0.8^2 = 0.48`. Visually, the preview's outlined-circle key
shrinks and fades from newest to oldest. Symbolically, if `0 <= r <= 1`, then
`alpha(n + 1) <= alpha(n)`: decay is monotonically non-increasing. The model
checks finite inputs, bounds history to 4,096 samples, calculates age from
explicit vector order, and returns finite channels and radii.

`buildTrailMarks(history, design, output)` is transactional and pure with
respect to observable inputs. It builds a fresh output vector, then commits only
if every sample and mark is valid. Calling it before or after an unrelated
trail produces the same values. Existing contents of `output`, openFrameworks
style, blend mode, and wall-clock state cannot affect the plan.

### Composition and blend mode are different decisions

The pure model provides source-over arithmetic as a lesson-sized known case.
The starter adapter requests normal alpha blending; its filled circles and open
squares remain two non-color palette cues. The solution requests additive
blending; crosses and diamonds form a sparse luminous star loom. Additive
blending is intentionally a renderer decision, not smuggled into palette or
decay functions.

The openFrameworks adapter follows the
[ofGraphics state API](https://openframeworks.cc/documentation/graphics/ofGraphics/):

```cpp
ofPushStyle();
ofEnableBlendMode(OF_BLENDMODE_ADD);
// draw only from deterministic TrailMark values
ofDisableBlendMode();
ofPopStyle();
```

Matrix changes in the solution are also paired with push/pop calls. Help text
uses a separate style scope. A draw call never owns the only copy of trail age,
palette identity, alpha, or radius.

### Two palettes require two roles, not just more hues

Each input sample carries palette index 0 or 1 and interpolation amount. In the
starter, the two deterministic paths adapt the memory idea from section 09;
pointer/drag and arrows move their shared origin. Palette A uses circles and
palette B squares. The solution changes path equations, history length, decay,
size, background, both palettes, blend mode, sampling rhythm, and geometry.
It uses crosses for A and diamonds for B. A learner must create a third visual
grammar rather than recolor either example.

P pauses, R clears and replays the initial phase, and M keeps current marks only.
Pointer and keyboard routes alter the same visible origin. Very small windows
suppress origin movement safely. Nothing flashes, and no meaning is audio-only.

## Build: inspect three complete experiments

### 1. Replay the independent arithmetic oracle

Linux x86-64 or macOS arm64:

```sh
cat exercises/12-color-blending-and-trails/fixtures/trail-oracle.txt
CXX=g++ tests/run-section-12-tests.sh
```

The hand-computable rows pin palette start/middle/end, half-red over blue, and
two decay steps. The suite also proves malformed fixture rows fail through
cardinality checks before field indexing. Windows Visual Studio 2022 x64
Developer PowerShell:

```powershell
Get-Content .\exercises\12-color-blending-and-trails\fixtures\trail-oracle.txt
.\tests\run-section-12-tests.ps1
```

### 2. Trace one deterministic trail plan

For three samples, model age is `2, 1, 0` from oldest to newest. With radii 2 to
10, marks receive radii `2, 6, 10`. With opaque palette colors, base opacity
`0.75`, and retention `0.8`, their alphas are `0.48, 0.60, 0.75`. Find those
assertions in `color_trail_model_test.cpp`, then compare them with the symbolic
decay equation. Palette color alpha can lower those values further.

### 3. Compare the visibly divergent adapters

Set `OF_ROOT` to openFrameworks 0.12.1. Supported native lanes are Linux x86-64,
macOS arm64, and Windows Visual Studio 2022 x64 Developer PowerShell. Other
architectures are unattested and unsupported by these wrappers.

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

Launch both projects, not just one. Normal-alpha paired ribbons and the additive
star loom should be visibly divergent. Compilation cannot prove launch,
contrast, additive appearance, or scoped state after a draw.

## Break and repair

In the exact tracked file
`exercises/12-color-blending-and-trails/shared/color_trail_model.cpp`,
temporarily change:

```cpp
const std::size_t age = history.size() - 1 - index;
```

to:

```cpp
const std::size_t age = history.size() - index;
```

Predict why even the newest point now decays once. Run
`tests/run-section-12-tests.sh` and locate the first age or alpha failure.
Restore the original expression and rerun. If this was your only edit:

```sh
git restore -- exercises/12-color-blending-and-trails/shared/color_trail_model.cpp
```

That command discards every uncommitted change in the named file. Record the
observed failure, the off-by-one explanation, and the repaired result.

## Exercise

Open the [two-palette trail study brief](../../../exercises/12-color-blending-and-trails/README.md).
Start from the particle-memory or gesture idea: keep bounded oldest-to-newest
samples, but create two explicit palette roles and controlled persistence. Edit
`starter/src/design/trail_design.cpp` for endpoints, decay, opacity, radii, and
history. Edit `starter/src/ofApp.cpp` for motion/gesture source, blend mode,
geometry, and composition. Preserve checked normalized channels, finite guards,
pure plan output, keyboard access, reset, reduced motion, and scoped renderer
state.

## Test

Linux x86-64 or macOS arm64:

```sh
CXX=g++ tests/run-section-12-tests.sh
CXX=clang++ tests/run-section-12-tests.sh
bash -n scripts/section-12.sh tests/run-section-12-tests.sh
shellcheck scripts/section-12.sh tests/run-section-12-tests.sh
scripts/check-authoring.sh
```

Use `tests/run-section-12-tests.ps1` on Windows. With `OF_ROOT`, generate and
compile starter and solution in Debug and Release, then launch each manually.
The pure suite covers known cases, exact boundaries, channel and composition
properties, invalid/non-finite safety, endpoint preservation, decay monotonicity,
history caps, transactional output, and state-independent replay. It does not
pixel-gate GPU output.

## Reflect

In 160–200 words, distinguish RGB from alpha, interpolation from composition,
source-over from additive blending, sample age from wall-clock age, straight
from premultiplied color, model output from renderer state, and automated
numeric evidence from manual pixel evidence. Explain the palette midpoint,
red-over-blue result, decay equation, one non-color palette cue, and one
learner-owned visual decision. Include capture alt text.

## Remix

Use a gesture instead of autonomous particles, map speed to interpolation
amount, make one palette cool-to-warm and the other low-to-high value, compare
normal and additive blend in separate panels, quantize ages into print-like
bands, or use decaying line width while keeping alpha fixed. Preserve two
non-color roles, bounded history, valid channels, explicit age, deterministic
plan output, scoped style/blend/matrix state, reset, keyboard access, and a
reduced-motion result.

## Manual pixel validation

Automated tests do **not** complete this checklist. After a native launch,
record platform, openFrameworks version, GPU/driver, project, configuration,
and pass/fail notes for each item. Until that record exists, pixel validation is
honestly **not performed**.

- Starter uses normal alpha blending; solution uses additive blending; their overlap, geometry, density, paths, and background visibly diverge.
- Palette A and B remain identifiable from circles/squares or crosses/diamonds when viewed in grayscale or with hue differences hidden.
- Old-to-new order remains readable through alpha plus size or spacing, not color alone.
- P pauses, R resets phase and histories, M removes persistence, and pointer/drag and arrows move the same visible origin.
- Text contrast remains readable; no flashing or audio-only information appears.
- Resize and very small windows remain safe; marks do not produce obvious cross-window artifacts.
- Drawing after the trail block shows no leaked blend mode, fill, line width, color, or matrix transform.
- The learner result differs from both examples in path/gesture source, palette roles, geometry, decay, density, composition, and blend treatment.
- Capture alt text names both shape roles, overlap, age ordering, palette roles, controls, and reduced-motion behavior.
- Any reused palette, code, reference, or asset is credited.

## Pilot note

Pilot evidence not yet collected. After one learner completes the section,
record exact platform/tool versions; reading, prediction, repair, exercise,
reflection, and manual review time separately; setup friction; pointer and
keyboard access; reduced-motion use; whether normalized color, endpoint
interpolation, straight alpha, source-over weighting, exponential decay,
oldest-to-newest age, two palette roles, deterministic output, and renderer
state scoping were understood; automated results; native launch/manual pixel
results; originality review; and confusions. Do not infer timing,
accessibility, originality, or graphical success from CI.
