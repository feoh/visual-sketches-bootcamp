# Exercise 11: coherent-current instrument

## The short version

You will fill a grid with smoothly changing values, turn those values into arrow
directions, and let particles reveal the result. Think of a flow field as a grid of
invisible arrows.

The grid lives in one long C++ vector. The helper converts row and column into a list
position, then blends nearby values when a particle lies between cells.

Advect a limited set of particles through a time-varying scalar grid. The shared model
hashes integer lattice coordinates, smoothly interpolates repeatable value noise, stores
one scalar per row-major grid cell, bilinearly samples the grid at a particle position,
maps that scalar to an angle, and advances at a fixed step. openFrameworks only adapts
controls and draws easy to check state.

## What you choose

Edit `starter/src/design/flow_field_design.cpp` for grid dimensions, particle count, history cap, radius, speed,
spatial scale, temporal scale/rate, and palette. Edit `starter/src/ofApp.cpp` for geometry and
trail grammar. Replace the starter's connected triangular currents with a visual system
unlike both examples. The solution uses more particles, a finer/slower field, sparse
perpendicular memory bars, rotated squares, and a dark palette.

Pointer/drag and arrow keys move the same field-coordinate control. P pauses, R replays
the recorded seed, M reduces every history to its current head, and T freezes/unfreezes
field time. Wrapping uses the legal center interval `[radius, size - radius]`; either-axis wrap
resets model history at the new current point, so no discontinuous segment exists for a
renderer to connect. A viewport smaller than one diameter rejects simulation and
suppresses drawing.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

The course supplies checked commands for Linux x86-64, macOS arm64, and Windows Visual
Studio 2022 x64 Developer PowerShell. On another system, the sketch may work, but you
may need to adapt the build steps.

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

Keep `makeFlowFieldDesign()` and the declarations in `shared/flow_field_model.h`. Tests
compare the hash, blended samples, and angles with saved expected values. They also
check list indices, boundaries, repeatable seeds, smooth change over time, fixed time
steps, wrapping particles, history limits, controls, and bad numbers. A failed update
must leave the previous state unchanged. Tests make sure the design function remains
editable and valid; you judge originality and pixels by opening the app.

## Notes for future you

Leave yourself answers to any of these practical questions:

- How does a row and column become one safe position in the flat list?
- What makes nearby field samples point in smoothly changing directions instead of
  jumping randomly?
- Which controls and limits keep the sketch manageable when time moves, particles wrap,
  or a frame arrives late?

Also note the shape or direction cue that still works without color and what `M` changes
for someone who wants less motion. The saved seed makes this implementation repeatable;
it does not promise identical pixels on every graphics card.

## Reference solution

The [explained solution](solution/README.md) is one answer, not a target image. It documents its parameter and
rendering decisions. Preserve the repeatable model interface and tests while changing
density, field scales, geometry, memory marks, composition, controls, and palette.

## Check the result yourself

- Pointer/drag and arrows change the same visible field-coordinate control.
- P pauses without a resume spike; R replays; M removes trails; T freezes time.
- Heads retain direction marks when histories are reduced.
- Frozen/live field state uses square/circle or cross/square cues, not color alone.
- Text and palettes remain legible when the help UI is shown; nothing flashes and there
  is no audio-only cue.
- Resize, exact edges, and wrapping remain safe; unusably tiny viewports suppress marks
  and help instead of clipping them.
- Density, grid/time scales, geometry, trail treatment, composition, and palette differ
  from both examples.
- Capture alt text names coherent direction, memory length/order, wrapping, controls,
  shapes, and palette roles.
- Credit any reused reference, code, or asset.
