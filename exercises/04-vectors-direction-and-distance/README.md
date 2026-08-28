# Exercise 04: distance-reactive constellation

Complete the [Lesson and Practice](../../authoring/sections/04-vectors-direction-and-distance/index.md#lesson)
before starting this problem. This page is the authoritative exercise brief.

## The short version

You will build a constellation around an arrow from one point to another. Here a vector
is just that arrow stored as x and y movement. Subtraction finds the arrow; length tells
you its distance; normalization keeps its direction while making its length 1.

Draw the `(3, 4)` example on paper first. The code follows the same steps.

Build a connector whose geometry responds to pointer or arrow-key input. The
renderer-independent C++17 model subtracts points to obtain direction, measures
distance, normalizes with an explicit zero-length guard, scales a unit direction by a
limited reach, and keeps every rendered node inside a stroke-aware safe inset.

## Tracked motion studies

The same `shared/constellation_model.{h,cpp}` also contains renderer-independent seek, orbit, and bounce
studies. `MotionState{{position}, {velocity}, {acceleration}}` uses aggregate initialization; fixed-step functions take that
small state by value, while read-only designs use `const Design&`. The native adapters
show safe named-component conversion between the core `Vec2` and openFrameworks'
`glm::vec2`.

- `seekAcceleration()`/`stepSeek()` guard a coincident target and limit speed.
- `orbitPoint()` maps phase and radius to a point, with radius zero at center.
- `stepBounce()` integrates a supplied fixed `dt`, clamps to `Bounds`,
  and reverses only the crossed velocity component.

They are intentionally pure numerical studies, not three more generated apps. The public
runner compiles and exercises their known cases. If you render one, provide pause/reset
keys and make target, path, or bounds distinguishable by shape/line as well as color.

## What you choose

Edit `starter/src/design/constellation_design.cpp`: choose anchor fractions from `0.15`–`0.85`, reach
from `24`–`200`, and three valid RGB colors with ink/background and
accent/background different. Then edit `starter/src/ofApp.cpp` to make your own connector and
node system. The starter is a line, a filled circular anchor, an outlined square target,
and an outlined traveler. The explained solution uses a triangular constellation,
perpendicular satellites, and graduated connector marks. Create a third treatment that
differs in geometry or spatial relationship, not color alone. There is no target
screenshot and no screenshot comparison.

## Test and build

```sh
tests/run-section-04-tests.sh
scripts/section-04.sh generate --project starter
scripts/section-04.sh build --project starter --configuration Release
```

On Windows Developer PowerShell:

```powershell
.\tests\run-section-04-tests.ps1
.\scripts\section-04.ps1 generate -Project starter
.\scripts\section-04.ps1 build -Project starter -Configuration Release
```

Project Generator 0.103.0 owns the generated files. The course supplies checked build
commands for Linux, macOS, and Windows. After compiling, open the app to check its
picture.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

Keep `makeConstellationDesign()` and declarations in `shared/constellation_model.h`. Tests compile the starter
design—not the solution—and parse every fixture field. Independent numerical oracles and
approximate tolerances cover components, subtraction, direction, length/distance,
normalization, zero and `NaN` or infinite guards, fixed-step
velocity/acceleration integration, seek/coincident-target behavior, orbit/zero-radius
cases, bounce boundaries and repeatable replay, scaling without overshoot, clamped
pointer boundaries, valid `64 x 64` viewports, invalid smaller viewports,
stroke-aware in-bounds centers, determinism, and design value variation. Tests never
inspect source text, rendered pixels, or resemblance.

## Check the result yourself

- Connector hierarchy remains legible at narrow, wide, square, and minimum useful sizes.
- Shape, spacing, outline, or labels communicate relationships without color alone.
- Ink/background and accent/background contrast are suitable; nothing flashes.
- Geometry or interaction differs from the starter and solution, not only palette.
- Pointer and arrow keys produce equivalent target movement; after an edge pointer
  event, one inward arrow-key event moves the clamped target exactly 12 pixels.
- Capture alt text names anchor, target, connector direction, distance response, and
  viewport relationship.
- Reused code and assets are credited and license-compatible.
