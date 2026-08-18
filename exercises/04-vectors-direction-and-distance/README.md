# Exercise 04: distance-reactive constellation

Build a connector whose geometry responds to pointer or arrow-key input. The renderer-independent C++17 model subtracts points to obtain direction, measures distance, normalizes with an explicit zero-length guard, scales a unit direction by a bounded reach, and keeps every rendered node inside a stroke-aware safe inset.

## Tracked motion studies

The same `shared/constellation_model.{h,cpp}` also contains renderer-independent
seek, orbit, and bounce studies. `MotionState{{position}, {velocity},
{acceleration}}` uses aggregate initialization; fixed-step functions take that
small state by value, while read-only designs use `const Design&`. The native
adapters show safe named-component conversion between the core `Vec2` and
openFrameworks' `glm::vec2`.

- `seekAcceleration()`/`stepSeek()` guard a coincident target and limit speed.
- `orbitPoint()` maps phase and radius to a point, with radius zero at center.
- `stepBounce()` integrates a supplied fixed `dt`, clamps to `Bounds`, and
  reverses only the crossed velocity component.

They are intentionally pure numerical studies, not three more generated apps.
The public runner compiles and exercises their known cases. If you render one,
provide pause/reset keys and make target, path, or bounds distinguishable by
shape/line as well as color.

## Learner-owned choices

Edit `starter/src/design/constellation_design.cpp`: choose anchor fractions from `0.15`–`0.85`, reach from `24`–`200`, and three valid RGB colors with ink/background and accent/background different. Then edit `starter/src/ofApp.cpp` to make your own connector and node system. The starter is a line, a filled circular anchor, an outlined square target, and an outlined traveler. The explained solution uses a triangular constellation, perpendicular satellites, and graduated connector marks. Create a third treatment that differs in geometry or spatial relationship, not color alone. There is no target screenshot and no pixel gate.

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

Project Generator 0.103.0 owns generated metadata. Native support requires green `section-04-linux-build`, `section-04-macos-build`, and `section-04-windows-build` statuses on the exact commit. Compilation does not prove graphical runtime or visual quality.

## Public contract

Keep `makeConstellationDesign()` and declarations in `shared/constellation_model.h`. Tests compile the starter design—not the solution—and parse every fixture field. Independent numerical oracles and approximate tolerances cover components, subtraction, direction, length/distance, normalization, zero and non-finite guards, fixed-step velocity/acceleration integration, seek/coincident-target behavior, orbit/zero-radius cases, bounce boundaries and deterministic replay, scaling without overshoot, clamped pointer boundaries, valid `64 x 64` viewports, invalid smaller viewports, stroke-aware in-bounds centers, determinism, and learner parameter variation. Tests never inspect source text, rendered pixels, or resemblance.

## Manual accessibility and originality review

- Connector hierarchy remains legible at narrow, wide, square, and minimum useful sizes.
- Shape, spacing, outline, or labels communicate relationships without color alone.
- Ink/background and accent/background contrast are suitable; nothing flashes.
- Geometry or interaction differs from the starter and solution, not only palette.
- Pointer and arrow keys produce equivalent target movement; after an edge pointer event, one inward arrow-key event moves the clamped target exactly 12 pixels.
- Capture alt text names anchor, target, connector direction, distance response, and viewport relationship.
- Reused code and assets are credited and license-compatible.
