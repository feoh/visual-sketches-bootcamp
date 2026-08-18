# Exercise 05: repeated phase-driven field

Build a responsive field in which nested row/column loops assign each mark a
phase. The standard-library C++17 model converts degrees and radians, maps
polar coordinates with `cos`/`sin`, recovers radius and quadrant-aware angle
with `atan2`, and computes deterministic amplitude/frequency/phase motion.

## Learner-owned choices

Edit exactly `starter/src/design/phase_field_design.cpp` first. Choose `rows`
and `columns` from 2–16, amplitude from 0–16 pixels, frequency from 0.05–2 Hz,
row/column phase steps from `-tau` to `tau`, mark radius from 2–8 pixels, and
three valid RGB colors whose ink/background and accent/background differ. Then
edit `starter/src/ofApp.cpp` to create your own repeated geometry.

The starter shows outlined orbit circles, crosshair arms capped at the mark
radius, and filled circular travelers. The explained solution is visibly
different: connected row threads carry alternating filled/outlined diamonds
whose four axis vertices stay one mark radius from their centers, with a denser
diagonal phase rhythm and different negative space. Create a third spatial
treatment, not a recolor. There is no target screenshot and no pixel gate.

Space or `P` pauses/resumes; `R` resets time. The adapter advances an explicit
`1/60`-second step. The pure model takes supplied time, so its replay and tests
do not depend on frame timing.

## Test and build

```sh
tests/run-section-05-tests.sh
scripts/section-05.sh generate --project starter
scripts/section-05.sh build --project starter --configuration Release
```

On Windows Developer PowerShell:

```powershell
.\tests\run-section-05-tests.ps1
.\scripts\section-05.ps1 generate -Project starter
.\scripts\section-05.ps1 build -Project starter -Configuration Release
```

Project Generator 0.103.0 owns generated metadata. Native support requires
green `section-05-linux-build`, `section-05-macos-build`, and
`section-05-windows-build` statuses on the exact commit. Compilation does not
prove graphical runtime, accessibility, or visual quality.

## Public contract

Keep `makePhaseFieldDesign()` and declarations in
`shared/phase_field_model.h`. Tests compile the starter design—not the
solution—and strictly parse independent fixture fields. Approximate numerical
properties cover degrees/radians, cardinal `sin`/`cos`, polar/cartesian round
trips, all `atan2` quadrants and the zero-vector angle policy, amplitude,
frequency, phase, nested row/column indexing and count, phase-plus-`2*pi` and
time-plus-period periodicity, finite and stroke-aware base/orbit/mark bounds
including independently calculated circle, crosshair, and diamond extrema at
`64 x 64`, explicitly invalid smaller viewports, determinism, learner parameter
variation, and learner-owned design validity. Tests do not inspect source text,
pixels, contrast, or resemblance.

## Manual accessibility and originality review

- Pause/reset controls work, and no pattern flashes.
- Repeated marks remain distinguishable without color alone at narrow, square,
  wide, and minimum useful sizes.
- Ink/background and accent/background contrast are suitable.
- Geometry, mark shape, or spatial relationships differ from starter and
  solution—not only palette.
- Capture alt text names the grid, orbit/phase direction, shape encoding, and
  viewport relationship.
- Reused code and assets are credited and license-compatible.
