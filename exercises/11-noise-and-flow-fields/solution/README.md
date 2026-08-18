# Section 11 reference solution: woven weather

This answer changes the starter's grid, fixed step, particle density, history,
spatial/temporal response, speed, palette, and geometry. Instead of connected
paths, every other history sample becomes a short perpendicular bar. Rotated
open squares and forward ticks preserve direction without color; a cross versus
square distinguishes frozen from live field time. M removes all memory bars but
keeps heads and direction ticks.

## Why these values

A 23-by-15 row-major grid keeps bilinear cells visible in the motion without
approaching the 65,536-cell work cap. The spatial scale 0.14 produces broader
coherence than the starter; temporal scale 0.21 and time rate 0.38 make the field
change slowly enough to follow. The 1/90-second step, ten-step catch-up cap, 108
particles, 28-point histories, four-pixel radius, and speed 112 trade denser
woven texture for explicitly bounded work. A dark field with mint memory bars
and amber heads gives strong contrast, while geometry and direction ticks carry
the same information without color.

The model's credited, course-pinned integer mixer supplies the eight lattice values around
each `(x, y, time)` query. Cubic smoothstep is applied independently to the
three fractions before trilinear interpolation. The generated grid then stores
one scalar per cell; world positions use ordinary bilinear interpolation across
four stored neighbors. Multiplying the scalar by `2*pi` maps it to a unit
advection vector. Exact grid and viewport edges clamp sampling to the final row
or column. Particle centers wrap within the radius-aware legal interval, and a
wrap resets history at the new point. Perpendicular bars use 60% of the
configured particle radius so their stroke remains inside that reserve.

The fixed-step accumulator clamps long frames, simulates at most ten steps, and
reports unprocessed whole steps as dropped. Seed, offset, field time, particle
positions, histories, and counters live in the model, so reset replay and
transactional rejection are inspectable. Exact integer hashes are pinned;
floating-point values use tolerances, and GPU pixels are not a portability
claim.

## Build and launch

These native commands support Linux x86-64, macOS arm64, and Windows Visual
Studio 2022 x64 Developer PowerShell. Other architectures are currently
unattested and unsupported by the wrappers.

Linux x86-64:

```sh
scripts/section-11.sh generate --project solution
scripts/section-11.sh build --project solution --configuration Release
exercises/11-noise-and-flow-fields/solution/bin/solution
```

macOS arm64:

```sh
scripts/section-11.sh generate --project solution
scripts/section-11.sh build --project solution --configuration Release
open exercises/11-noise-and-flow-fields/solution/bin/solution.app
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\scripts\section-11.ps1 generate -Project solution
.\scripts\section-11.ps1 build -Project solution -Configuration Release
& .\exercises\11-noise-and-flow-fields\solution\bin\solution.exe
```

Manually inspect both input routes, pause/reset/reduced/frozen controls, resize,
wrapping, contrast, and originality. Compilation and model tests do not prove
those visual claims.
