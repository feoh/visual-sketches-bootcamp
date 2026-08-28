# Exercise 03: responsive poster

Complete the [Lesson and Practice](../../authoring/sections/03-map-clamp-and-lerp/index.md#lesson)
before starting this problem. This page is the authoritative exercise brief.

## The short version

You will make a poster layout respond to the window size. Mapping turns progress in one
range into progress in another: halfway across the input becomes halfway across the
output. Clamping stops a value at an edge.

Start with the worked numbers in the lesson, then change the three design values that
control focus, vertical bias, and color.

Build a poster whose layout responds to window size without stretching. The
renderer-independent C++17 model normalizes viewport size, maps it into padding and type
ranges, applies one smoothstep easing curve, clamps unsafe inputs, and fits a 4:5 panel.

## What you choose

Edit `starter/src/design/poster_design.cpp`: choose a horizontal focus from 0–1, vertical bias from 0.2–0.8, and
three valid RGB colors with ink/background and accent/background different. Then edit
`starter/src/ofApp.cpp` to make your own hierarchy, geometry, and palette. The starter is a
filled panel with one dot and rule. The explained solution is an outlined panel with
concentric orbit rings and a triangle. Create a third treatment that differs in layout
or geometry, not color alone. There is no target screenshot and no screenshot
comparison.

## Test and build

```sh
tests/run-section-03-tests.sh
scripts/section-03.sh generate --project starter
scripts/section-03.sh build --project starter --configuration Release
```

On Windows Developer PowerShell:

```powershell
.\tests\run-section-03-tests.ps1
.\scripts\section-03.ps1 generate -Project starter
.\scripts\section-03.ps1 build -Project starter -Configuration Release
```

Project Generator 0.103.0 owns the generated files. The course supplies checked build
commands for Linux, macOS, and Windows. After compiling, open the app to check its
picture. The explained solution
also bounds each ring step by the available focus-to-panel distance so its outer orbit
stays contained at the `64 x 64` boundary.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

Keep `makePosterDesign()` and declarations in `shared/poster_layout.h`. Tests compile the starter
design—not the solution—and parse every fixture field. They use independent numerical
oracles and explicit absolute/relative tolerances for helper endpoints, clamped
out-of-range and `NaN` or infinite inputs, invalid equal or descending source
ranges, midpoint, monotonic easing, focus endpoints, 4:5 aspect, responsive resize,
finite in-bounds geometry, orbit containment at the 64-pixel boundary, invalid smaller
viewports, and your design ranges. They never inspect source text, rendered pixels, or
resemblance.

## Check the result yourself

- Hierarchy and relationships remain legible at narrow, wide, and square sizes.
- Shape, spacing, outline, or labels communicate structure without color alone.
- Text/ink and accent contrast are suitable; no flashing occurs.
- Geometry or mapping differs from the starter and solution, not only palette.
- Capture alt text names the 4:5 panel, focal placement, hierarchy, and palette
  relationship.
- Reused code and assets are credited and license-compatible.
