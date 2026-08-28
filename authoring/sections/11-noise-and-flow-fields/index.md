---
title: Noise and flow fields
slug: 11-noise-and-flow-fields
weight: 130
draft: false
course_kind: instructional
objectives:
  - Store and safely index a limited scalar field as one row-major two-dimensional grid
  - Generate coherent repeatable value noise from a course-pinned integer lattice mixer and smooth interpolation
  - Bilinearly sample grid corners and boundaries, then map one scalar to a finite unit direction
  - Advect limited particles with fixed steps, wrapping, short histories, temporal change, and explicit dropped time
  - Design an accessible flow instrument whose calculations can be checked without comparing pixels
prerequisites:
  - Completion of section 10 or equivalent vectors, limited containers, finite guards, fixed-step simulation, and event adapters
source_records: sources.yaml
asset_records: assets.yaml
---

# Noise and flow fields

This section has one path: learn how a flat grid becomes smooth directions,
practice indexing and interpolation, then solve one tested flow-instrument
problem.

1. [Lesson: turn a scalar grid into a flow field](#lesson)
2. [Practice: calculate, sample, and repair](#practice)
3. [Exercise: create a tested coherent current](#exercise)

## Lesson

### Reveal an invisible field

![A dark blue field contains a pale grid of short direction ticks; three mint dotted ribbons curve together toward amber diamond heads with forward ticks, while a square field-coordinate control and small clock distinguish space and time without color alone.](media/flow-preview.svg "Nearby arrows form one smoothly changing field.")

*Nearby samples become smooth directions, then a limited group of particles reveals the field through trails.*

A flow field is a grid of invisible arrows. A particle samples a nearby arrow
and follows it, revealing the field as a trail. Pure random arrows would jerk
between unrelated directions. Repeatable **value noise** supplies coherent
corner values, and interpolation blends those values so nearby queries change
smoothly.

### Store two dimensions in one vector

A limited [`std::vector<float>`](https://en.cppreference.com/w/cpp/container/vector.html)
stores one scalar per cell in row-major order:

```text
index = row * columns + column
```

Column 2, row 1 in a four-column grid has index `1 × 4 + 2 = 6`. The helper
requires at least two rows and columns, checks coordinates and arithmetic, and
caps the field at 65,536 cells. A 256 × 256 field is legal; 257 in either
dimension is not. This checked flat vector is the section's main C++ mechanism.

### Value noise blends repeatable lattice values

Each integer `(x, y, z, seed)` corner receives the same 0-to-1 value on every
run. The lattice mixer credits the permissively licensed
[Boost hash-combine pattern](https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html)
and Chris Wellons's public-domain
[lowbias32 work](https://nullprogram.com/blog/2018/07/31/).
The bundled expression retains its
[third-party notice](../../../THIRD_PARTY_NOTICES.md).

Between integer corners, each fractional coordinate uses smoothstep:

```text
smooth(t) = t * t * (3 - 2 * t)
```

Eight corner values are interpolated along x, then y, then z. This lesson uses
value noise rather than Ken Perlin's gradient-noise implementation; the
[creator's noise reference](https://mrl.cs.nyu.edu/~perlin/noise/)
provides context, not copied code. Spatial scale controls how quickly the field
changes across the grid. Temporal scale and time rate separately control what
region of time is sampled and how quickly time advances.

### Bilinear sampling blends four stored cells

Particles sample the stored viewport grid at normalized x and y. Four neighbors
are mixed:

```text
upper = lerp(top_left, top_right, tx)
lower = lerp(bottom_left, bottom_right, tx)
value = lerp(upper, lower, ty)
```

The [bilinear interpolation reference](https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/interpolation/bilinear-filtering.html)
shows the same four-corner structure. A 2 × 2 grid with rows `0, 1` and `1, 0`
samples to 0.5 at `(0.5, 0.5)`. Normalized 0 and 1 select the first and last
row or column exactly; `x == 1` is valid. Out-of-range, non-finite, malformed,
or stale grids reject without changing output.

Noise interpolation is trilinear because the source varies in x, y, and time.
Stored-grid sampling is bilinear because that grid has only x and y. Keeping the
stages separate makes each calculation testable.

### Map one scalar to one direction

A scalar `s` becomes an angle and unit vector:

```text
angle = clamp(s, 0, 1) * 2*pi
direction = {cos(angle), sin(angle)}
```

Scalar 0.25 gives `pi/2` and approximately `(0, 1)`. Scalar 0.125 gives
approximately `(0.7071, 0.7071)`, proving that both vector components matter.
Particles advance by `direction * advection_speed * fixed_dt`.

Centers wrap through the radius-aware visible interval. A wrap resets that
particle's oldest-to-newest history to its new point so the renderer cannot draw
a false line across the window. Reduced motion keeps only the current point.
Particle count, grid cells, histories, and catch-up work have hard caps.

The bounded accumulator follows the
[fixed-step rule](https://gafferongames.com/post/fix_your_timestep/): invalid time
is rejected, long frames are clamped, whole unprocessed steps are counted as
dropped, and only a substep remainder is retained. P pauses, M clears histories,
T freezes field time while particles continue, and R restores all repeatable
state.

Pointer, drag, and arrows adjust the same visible field-coordinate offset through
the [openFrameworks event adapter](https://openframeworks.cc/documentation/events/ofEvents/).
The renderer reads model state; it does not own the only copy of any tested
value. Exact integer hash rows are pinned, while derived floating-point values
use explicit tolerances rather than claiming cross-GPU pixel identity.

## Practice

Practice is guided and has no unit-test gate. Calculate one index and boundary
sample, inspect the independent oracle, run the instrument, and repair one
row-major mistake.

### 1. Calculate row-major and bilinear cases

Draw a 3 × 3 grid containing `0.0` through `0.8` in row-major order. In a
three-column grid, column 2, row 1 is flat index 5. At normalized `(0.5, 0.5)`,
both scaled coordinates equal 1, so the bilinear sample is the center cell 0.4.
At `(1, 0.5)`, x selects the final column and y the middle row, so the answer is
0.5.

For the 2 × 2 grid from the Lesson, work through the two horizontal lerps before
the vertical lerp. Then map its center value 0.5 to angle `pi` and direction
approximately `(-1, 0)`.

### 2. Inspect the independent oracle

Linux or macOS:

```sh
cat exercises/11-noise-and-flow-fields/fixtures/flow-oracle.txt
```

Windows Developer PowerShell:

```powershell
Get-Content .\exercises\11-noise-and-flow-fields\fixtures\flow-oracle.txt
```

Find the exact integer hash rows, bilinear center 0.5, and quarter-turn vector
`(0, 1)`. Distinguish the pinned integer values from the tolerance-checked
derived floats.

### 3. Run the flow instrument

Set `OF_ROOT` to openFrameworks 0.12.1. Linux or macOS:

```sh
scripts/section-11.sh generate --project starter
scripts/section-11.sh build --project starter --configuration Release
```

Launch `exercises/11-noise-and-flow-fields/starter/bin/starter` on Linux, or
open the generated `.app` on macOS. Windows Developer PowerShell:

```powershell
.\scripts\section-11.ps1 generate -Project starter
.\scripts\section-11.ps1 build -Project starter -Configuration Release
& .\exercises\11-noise-and-flow-fields\starter\bin\starter.exe
```

Move the field-coordinate control with pointer and arrows. Compare P with T:
pause stops particles and field time, while freeze keeps particles moving
through one static field. Try R, M, every edge, wrapping, and a tiny window.

### 4. Repair swapped grid indexing

In `exercises/11-noise-and-flow-fields/shared/flow_field_model.cpp`, temporarily
change:

```cpp
index = row * columns + column;
```

to:

```cpp
index = column * rows + row;
```

Rebuild and run the non-square starter grid. Nearby arrows now come from the
wrong stored cells. Restore the row-major formula and confirm the coherent
current returns. If this was your only intended edit:

```sh
git restore -- exercises/11-noise-and-flow-fields/shared/flow_field_model.cpp
```

That command discards every uncommitted change in the named file. Square grids
can hide this mistake, which is why the practice app uses unequal dimensions.

## Exercise

### Problem: create a coherent-current instrument

Create a materially different field drawing by choosing parameters in
`starter/src/design/flow_field_design.cpp` and geometry or trail grammar in
`starter/src/ofApp.cpp`. Preserve checked row-major access, two explicit
interpolation stages, scalar-to-angle mapping, distinct spatial and temporal
controls, seed identity, finite guards, fixed-step dropped-time accounting,
work caps, wrap behavior, keyboard access, reset, and reduced motion.

Use the
[coherent-current instrument exercise brief](../../../exercises/11-noise-and-flow-fields/README.md)
as the authoritative source for editable files, controls, constraints, fixtures,
and the explained solution.

### Run the unit tests

Linux or macOS:

```sh
CXX=g++ tests/run-section-11-tests.sh
CXX=clang++ tests/run-section-11-tests.sh
```

Windows Developer PowerShell:

```powershell
.\tests\run-section-11-tests.ps1
```

The suite checks pinned integer hashes, row-major indexing and caps, malformed
grids, bilinear known cases and exact edges, non-finite rejection, scalar-to-unit
vector cases, deterministic generation, fixed-step partitions and dropped time,
field-time freeze, radius-aware wrap and history reset, reduced motion, reset,
work limits, and learner design ranges. Build starter and solution in Debug and
Release, then launch both; tests never compare renderer pixels.

### Quick visual check

- Pointer, drag, and arrows change the same visible field-coordinate control.
- P pauses without a burst, R replays, M removes histories, and T freezes field
  time while particles continue.
- Head geometry or ticks communicate direction when trails are reduced.
- Frozen and live state use shape rather than color alone; nothing flashes.
- Resize, every edge, radius-aware wrapping, and tiny viewports remain safe.
- Grid scale, temporal response, density, geometry, memory, composition, and
  palette differ from both examples.
- Text and palettes have suitable contrast; alt text names coherence,
  direction, time state, history, wrapping, controls, and shape roles.
- Reused references, code, and assets remain credited.

### If you get stuck

If the field looks like static, inspect two neighboring grid values and the two
lerp amounts before touching particle motion. If particles fly away, freeze time
and draw direction ticks, then inspect angle mapping, bounds, and fixed-step
input. Making the invisible grid visible usually turns a mysterious flow bug
into one wrong index or number.
