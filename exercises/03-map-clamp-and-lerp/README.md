# Exercise 03: responsive poster

Build a poster whose layout responds to window size without stretching. The renderer-independent C++17 model normalizes viewport size, maps it into padding and type ranges, applies one smoothstep easing curve, clamps unsafe inputs, and fits a 4:5 panel.

## Learner-owned choices

Edit `starter/src/design/poster_design.cpp`: choose a horizontal focus from 0–1, vertical bias from 0.2–0.8, and three valid RGB colors with ink/background and accent/background different. Then edit `starter/src/ofApp.cpp` to make your own hierarchy, geometry, and palette. The starter is a filled panel with one dot and rule. The explained solution is an outlined panel with concentric orbit rings and a triangle. Create a third treatment that differs in layout or geometry, not color alone. There is no target screenshot and no pixel gate.

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

Project Generator 0.103.0 owns generated metadata. Native support requires green `section-03-linux-build`, `section-03-macos-build`, and `section-03-windows-build` statuses on the exact commit. Compilation does not prove graphical runtime or visual quality. The explained solution also bounds each ring step by the available focus-to-panel distance so its outer orbit stays contained at the `64 x 64` boundary.

## Public contract

Keep `makePosterDesign()` and declarations in `shared/poster_layout.h`. Tests compile the starter design—not the solution—and parse every fixture field. They use independent numerical oracles and explicit absolute/relative tolerances for helper endpoints, clamped out-of-range and non-finite inputs, invalid equal or descending source ranges, midpoint, monotonic easing, focus endpoints, 4:5 aspect, responsive resize, finite in-bounds geometry, orbit containment at the 64-pixel boundary, invalid smaller viewports, and learner-owned design ranges. They never inspect source text, rendered pixels, or resemblance.

## Manual accessibility and originality review

- Hierarchy and relationships remain legible at narrow, wide, and square sizes.
- Shape, spacing, outline, or labels communicate structure without color alone.
- Text/ink and accent contrast are suitable; no flashing occurs.
- Geometry or mapping differs from the starter and solution, not only palette.
- Capture alt text names the 4:5 panel, focal placement, hierarchy, and palette relationship.
- Reused code and assets are credited and license-compatible.
