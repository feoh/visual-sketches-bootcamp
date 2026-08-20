---
title: Noise and flow fields
slug: 11-noise-and-flow-fields
weight: 130
draft: false
course_kind: instructional
objectives:
  - Store and safely index a bounded scalar field as one row-major two-dimensional grid
  - Generate coherent deterministic value noise from a course-pinned integer lattice mixer and smooth interpolation
  - Bilinearly sample grid corners and boundaries, then map one scalar to a finite unit direction
  - Advect bounded particles with fixed steps, wrapping, short histories, temporal change, and explicit dropped time
  - Design an accessible flow instrument whose deterministic evidence is separate from its pixels
prerequisites:
  - Completion of section 10 or equivalent vectors, bounded containers, finite guards, fixed-step simulation, and event adapters
source_records: sources.yaml
asset_records: assets.yaml
---

# Noise and flow fields

## See what we're making

![A dark blue field contains a pale grid of short direction ticks; three mint dotted ribbons curve together toward amber diamond heads with forward ticks, while a square field-coordinate control and small clock distinguish space and time without color alone.](media/flow-preview.svg "Nearby samples reveal one coherent, time-varying field.")

*Nearby samples become coherent directions, then bounded particles reveal the field through memory.*

Direction ticks, diamond heads, forward marks, and control shapes carry meaning
without relying on the mint/amber palette.

## Take a guess

A 2-by-2 scalar grid stores top row `0, 1` and bottom row `1, 0`. Predict the
bilinear sample at normalized position `(0.5, 0.5)`. Map that scalar to
`angle = scalar * 2*pi`; which unit vector results? For a 4-column grid, which
flat index stores column 2, row 1? What must happen at normalized x exactly 1?

## Let's unpack it

### One vector can represent a two-dimensional grid

A bounded [`std::vector<float>`](https://en.cppreference.com/w/cpp/container/vector.html)
stores one scalar per cell. The row-major formula is:

```text
index = row * columns + column
```

For column 2, row 1 in a 4-column grid, the index is 6. The helper checks that
both dimensions are at least 2, each coordinate is in range, multiplication and
addition cannot overflow, and total cells do not exceed 65,536. A one-row or
one-column field is rejected because bilinear sampling requires two neighbors.
Exactly 256 by 256 is legal; 257 in either dimension is not.

The flat vector is the section's one substantial C++ mechanism. Grid creation,
sampling, and rendering all use the same checked row-major contract rather than
inventing pointer arithmetic or nested ownership.

### Coherent noise interpolates deterministic lattice values

Independent random directions jump from cell to cell. Coherent noise begins
with repeatable values at integer lattice corners and blends between them. The
course pins a small 32-bit mixer—not `ofNoise` and not a standard-library
distribution. Its combine step follows the permissively licensed
[Boost hash-combine pattern](https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html).
Its final avalanche credits Chris Wellons's public-domain
[lowbias32 work](https://nullprogram.com/blog/2018/07/31/).
The bundled expression retains its [third-party notice](../../../THIRD_PARTY_NOTICES.md)
and the full BSL-1.0 text; the Wellons public-domain provenance remains explicit.
The course-owned part is the explicit coordinate/seed composition and
integer-to-value mapping used here. Integer `(x, y, z, seed)` always produces
the same pinned hash. `hash & 0x00ffffff` maps to `[0, 1]`.

At query `(x, y, z)`, eight integer corners surround the point. Each fractional
coordinate uses cubic smoothstep:

```text
smooth(t) = t * t * (3 - 2 * t)
```

The eight values are interpolated along x, then y, then z. This is value noise,
not Ken Perlin's gradient-noise implementation; the creator's
[noise reference](https://mrl.cs.nyu.edu/~perlin/noise/) supplies context, not
copied code. Smoothstep has zero slope at each cell edge, so nearby queries vary
coherently instead of snapping.

Spatial scale multiplies grid column and row before the noise query. Temporal
scale multiplies field time; time rate controls how quickly field time advances.
Those are three different design decisions. You can imagine the scalar field as
a smooth height landscape: a gradient would point in the steepest uphill
direction. This section does not approximate that gradient; it intentionally
maps height directly to angle so the indexing and interpolation pipeline stays
visible. The field-coordinate offset comes from pointer or keyboard input and
shifts the queried region without moving the stored viewport grid.

### Bilinear sampling is a second interpolation stage

Noise generation fills a bounded grid. Particles then sample that stored grid
at normalized viewport position. Four neighboring grid values are mixed:

```text
upper = lerp(top_left, top_right, tx)
lower = lerp(bottom_left, bottom_right, tx)
value = lerp(upper, lower, ty)
```

The numerical Predict answer is 0.5. The
[bilinear interpolation reference](https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/interpolation/bilinear-filtering.html)
shows the same four-corner structure. At normalized 0 or 1, sampling lands
exactly on the first or final row/column; `x == 1` is legal, not an out-of-range
index. Values below 0, above 1, NaN, malformed grids, and stale cell counts are
rejected without changing the output.

Noise interpolation is trilinear because its source varies in x, y, and time.
Stored-grid sampling is bilinear because the viewport grid has only x and y.
Keeping those stages separate makes each inspectable.

### One scalar becomes one direction

The field maps scalar `s` to angle and a unit vector:

```text
angle = clamp(s, 0, 1) * 2*pi
direction = {cos(angle), sin(angle)}
```

Scalar 0.25 produces angle `pi/2` and vector approximately `(0, 1)`. Scalar
0.125 produces a genuinely two-dimensional direction approximately
`(0.7071, 0.7071)`. Tests use both so an x-only implementation cannot pass.
A scalar field stores one number at each point; this mapping turns it into a
vector field that stores—or derives—a direction at each point.

### Particles reveal rather than define the field

Each fixed step regenerates the scalar grid at explicit field time, bilinearly
samples one direction per particle, then advances:

```text
position = position + direction * advection_speed * fixed_dt
```

Centers wrap through the radius-aware interval `[radius, size - radius]`.
History remains oldest-to-newest and bounded. If either axis wraps, the model
resets history to the new current point, so no cross-boundary segment exists for
a renderer to connect. Particle count, grid cells, and total history points all have
hard caps. Reduced motion keeps exactly the current point, so direction heads
remain visible without animated trails.

The accumulator follows the bounded
[fixed-step policy](https://gafferongames.com/post/fix_your_timestep/): reject
invalid time, clamp a long frame, simulate at most the configured catch-up
steps, report unprocessed whole steps as dropped, and retain only a substep
remainder. P clears partial time when pausing. M clears histories. T freezes
field time while particles continue through one static field. R restores seed,
field time, positions, histories, controls, counters, and flags.

### Determinism has a boundary

The exact integer hash rows are pinned, and every `uint32_t` seed—including
zero—remains a distinct design input. Derived floats are checked with explicit
tolerances under the course's pinned implementation and toolchains. The course
does not promise byte-identical values under arbitrary floating-point modes,
and never treats GPU pixels as cross-platform evidence. Invalid dimensions,
coordinates, time, state, counter overflow, or unsafe intermediate arithmetic
rejects the copied frame before commit.

Pointer/drag and arrows change the same visible offset through the openFrameworks
[event adapter](https://openframeworks.cc/documentation/events/ofEvents/).
The renderer reads field, positions, histories, and flags; it does not own the
only state that tests need.

## Make it run: inspect three complete experiments

### 1. Replay the independent oracle

Linux x86-64 or macOS arm64:

```sh
cat exercises/11-noise-and-flow-fields/fixtures/flow-oracle.txt
CXX=g++ tests/run-section-11-tests.sh
```

Confirm the exact integer hashes, bilinear center 0.5, and quarter-turn vector
`(0, 1)`. Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
Get-Content .\exercises\11-noise-and-flow-fields\fixtures\flow-oracle.txt
.\tests\run-section-11-tests.ps1
```

### 2. Inspect a boundary sample

Sketch a 3-by-3 grid numbered 0.0 through 0.8 in row-major order. At normalized
`(0.5, 0.5)`, both scaled coordinates are exactly 1, so the sample is the center
cell 0.4. At `(1, 0.5)`, x selects the final column and y selects the middle row,
so the result is 0.5. Find those assertions before changing the model.

### 3. Run the flow instrument

Set `OF_ROOT` to openFrameworks 0.12.1. The supported native lanes are Linux
x86-64, macOS arm64, and Windows Visual Studio 2022 x64 Developer PowerShell.
Other architectures are currently unattested and unsupported by the wrappers.

Linux x86-64:

```sh
scripts/section-11.sh generate --project starter
scripts/section-11.sh build --project starter --configuration Release
exercises/11-noise-and-flow-fields/starter/bin/starter
```

macOS arm64:

```sh
scripts/section-11.sh generate --project starter
scripts/section-11.sh build --project starter --configuration Release
open exercises/11-noise-and-flow-fields/starter/bin/starter.app
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\scripts\section-11.ps1 generate -Project starter
.\scripts\section-11.ps1 build -Project starter -Configuration Release
& .\exercises\11-noise-and-flow-fields\starter\bin\starter.exe
```

Move the field-coordinate control with pointer and arrows. Compare P with T:
pause stops particles and field time, while freeze keeps particles moving through
one static field. Try R, M, every edge, wrapping, and a tiny window. Compilation
does not prove launch.

## Break it on purpose

In the exact tracked file
`exercises/11-noise-and-flow-fields/shared/flow_field_model.cpp`, temporarily
change `index = row * columns + column;` to
`index = column * rows + row;`. Run `tests/run-section-11-tests.sh`; predict the
first non-square or row-major failure. Restore the formula and rerun. If this
was your only edit:

```sh
git restore -- exercises/11-noise-and-flow-fields/shared/flow_field_model.cpp
```

That command discards every uncommitted change in the named file. Record the
failure, why square examples can hide the transposition, and the repaired run.

## Your turn

Open the [coherent-current instrument brief](../../../exercises/11-noise-and-flow-fields/README.md).
Own parameters in `starter/src/design/flow_field_design.cpp` and geometry/trail
grammar in `starter/src/ofApp.cpp`, then replace the starter current with a
materially different field drawing. Explain indexing, two interpolation stages, scalar-to-angle
mapping, three scale/time controls, seed boundary, fixed-step/drop/wrap policy,
and reduced-motion cue.

## Check your work

```sh
CXX=g++ tests/run-section-11-tests.sh
CXX=clang++ tests/run-section-11-tests.sh
```

Use the PowerShell suite on Windows. Generate and compile starter and solution
in Debug and Release. Manually launch pointer and keyboard routes; inspect
pause, reset, reduced motion, frozen time, resize, wrapping, contrast, direction
cues, and originality. Pure and native CI prove only their named contracts.

## Tell the story

In 170–210 words, distinguish lattice from stored grid, hash from noise,
trilinear noise from bilinear grid sampling, scalar field from vector field,
spatial scale from temporal scale and time rate, field time from render time,
wrap from clamp, and exact integer reproducibility from tolerant floats and
manual pixels. Explain row-major indexing, one non-color direction/time cue,
and one learner-owned choice. Include capture alt text.

## Make it yours

Map scalar to a half-turn instead of a full turn, use frozen field time as a
printmaking mode, draw local direction combs, change wrap to a tested bounce, or
advect short ribbons seeded along one edge. Preserve checked row-major access,
explicit interpolation boundaries, pinned seed mapping, finite guards, fixed-step
and dropped-time evidence, work caps, keyboard access, reset, and reduced motion.

## Quick visual check

- Pointer/drag and arrows change the same visible field-coordinate control.
- P pauses without a resume burst; R replays; M removes histories; T freezes field time.
- Direction remains readable from head geometry/ticks when trails are reduced.
- Frozen/live state uses shape, not color alone; nothing flashes or depends on audio.
- Text and palettes have suitable contrast when the help UI is shown.
- Resize, every edge, and radius-aware wrapping remain safe; unusably tiny viewports suppress marks and help instead of clipping them.
- Grid scale, temporal response, density, geometry, memory treatment, composition, and palette differ from both examples.
- Alt text names coherence, direction, time state, history order/length, wrapping, controls, shapes, and palette roles.
- Reused references, code, and assets are credited.

## If you get stuck

If the field looks like static, check whether nearby samples share values and
whether you are sampling the grid smoothly. If particles fly off, inspect the
angle mapping, bounds, and fixed-step policy. Freeze time and draw the field's
arrows; the invisible math gets much less mysterious once it has a tiny handle.
