# Exercise 11: coherent-current instrument

Advect a bounded set of particles through a time-varying scalar grid. The shared
model hashes integer lattice coordinates, smoothly interpolates deterministic
value noise, stores one scalar per row-major grid cell, bilinearly samples the
grid at a particle position, maps that scalar to an angle, and advances at a
fixed step. openFrameworks only adapts controls and draws inspectable state.

## Learner-owned choices

Edit `starter/src/design/flow_field_design.cpp` for grid dimensions, particle
count, history cap, radius, speed, spatial scale, temporal scale/rate, and
palette. Edit `starter/src/ofApp.cpp` for geometry and trail grammar. Replace
the starter's connected triangular currents with a visual system unlike both
examples. The solution uses more particles, a finer/slower
field, sparse perpendicular memory bars, rotated squares, and a dark palette.

Pointer/drag and arrow keys move the same field-coordinate control. P pauses, R
replays the recorded seed, M reduces every history to its current head, and T
freezes/unfreezes field time. Wrapping uses the legal center interval
`[radius, size - radius]`; either-axis wrap resets model history at the new
current point, so no discontinuous segment exists for a renderer to connect. A
viewport smaller than one diameter rejects simulation and suppresses drawing.

## Public contract

The supported native lanes are Linux x86-64, macOS arm64, and Windows Visual
Studio 2022 x64 Developer PowerShell. Other architectures are unattested and
unsupported by the current wrappers.

On Linux x86-64 or macOS arm64:

```sh
CXX=g++ tests/run-section-11-tests.sh
CXX=clang++ tests/run-section-11-tests.sh
scripts/section-11.sh generate --project starter
scripts/section-11.sh build --project starter --configuration Release
```

On Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\tests\run-section-11-tests.ps1
.\scripts\section-11.ps1 generate -Project starter
.\scripts\section-11.ps1 build -Project starter -Configuration Release
```

Keep `makeFlowFieldDesign()` and the declarations in
`shared/flow_field_model.h`. Tests parse an independent integer-hash,
bilinear-sample, and angle oracle. They inspect row-major indices, invalid
sizes/coordinates, corners and cell/viewport boundaries, deterministic seeds,
smooth temporal change, non-axis unit vectors, fixed-step partitions, wrapped
advection, history/count caps, controls, dropped time, counter bounds, and
transactional finite rejection. The design-hook check proves only that a valid,
editable seam exists; originality is manual. No pixels or wall-clock timing are
a correctness gate.

## Required explanation

Explain:

1. why `index = row * columns + column` is checked before access;
2. how eight hashed lattice corners become one smooth 3D value-noise sample;
3. why grid sampling is bilinear even though lattice noise is trilinear;
4. how `angle = scalar * 2*pi` produces a unit vector;
5. what spatial scale, temporal scale, and time rate change separately;
6. why the credited, course-pinned hash composition replays under this implementation but does
   not promise identical GPU pixels or unrestricted floating-point builds;
7. how fixed steps, catch-up cap, dropped time, wrap, and history caps bound work;
8. one non-color direction/time cue and one reduced-motion choice.

## Reference solution

The [explained solution](solution/README.md) is one answer, not a target image.
It documents its parameter and rendering decisions. Preserve deterministic
model contracts while changing density, field scales, geometry, memory marks,
composition, controls, and palette.

## Manual accessibility and originality review

- Pointer/drag and arrows change the same visible field-coordinate control.
- P pauses without a resume spike; R replays; M removes trails; T freezes time.
- Heads retain direction marks when histories are reduced.
- Frozen/live field state uses square/circle or cross/square cues, not color alone.
- Text and palettes remain legible when the help UI is shown; nothing flashes and there is no audio-only cue.
- Resize, exact edges, and wrapping remain safe; unusably tiny viewports suppress marks and help instead of clipping them.
- Density, grid/time scales, geometry, trail treatment, composition, and palette differ from both examples.
- Capture alt text names coherent direction, memory length/order, wrapping, controls, shapes, and palette roles.
- Credit any reused reference, code, or asset.
