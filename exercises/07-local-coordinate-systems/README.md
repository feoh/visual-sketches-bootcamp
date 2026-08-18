# Exercise 07: three-frame hierarchical kinetic sculpture

Build one responsive composition containing the same parent-child sculpture at
three phases. The pure C++17 model composes translation, rotation, and scale,
then exposes pivot, elbow, and tip anchors for inspection without a pixel gate.

## Learner-owned choices

Edit `starter/src/design/sculpture_design.cpp` first. Own the arm lengths,
ornament radius, period, parent and child swing, and three-color palette. Then
replace the starter's line-and-ring language in `starter/src/ofApp.cpp` with
your own geometry. The explained solution uses filled triangular ribbons,
counter-motion, a square counterweight, and a night palette. Do not submit a
recolor of either route.

Keep each local drawing centered on its own origin and keep `MatrixScope`
around every pushed matrix. The scope guard balances `ofPushMatrix()` with
`ofPopMatrix()` on all exits; it is not an invitation to add pointers or manual
ownership.

## Test and build

```sh
CXX=g++ tests/run-section-07-tests.sh
CXX=clang++ tests/run-section-07-tests.sh
scripts/section-07.sh generate --project starter
scripts/section-07.sh build --project starter --configuration Release
```

Use the matching `.ps1` scripts from Windows Developer PowerShell. Project
Generator 0.103.0 owns native metadata. Green
`section-07-{linux,macos,windows}-build` statuses prove compilation only.

## Public contract

Keep `makeSculptureDesign()` and declarations in `shared/sculpture_model.h`.
Tests cover identity, translation, positive-down rotation, scale, explicit
zero/non-finite-scale policy, noncommutative transform order, a strictly parsed
independent three-frame anchor fixture, parent-child composition, deterministic
periodic time, invalid designs, `48 x 48` and narrow/wide stroke-aware bounds,
and the learner design contract. There is no pixel gate.

## Manual accessibility and originality review

- All geometry is static per frame and nothing flashes.
- Parent, child, pivot, and ornament remain distinguishable without color.
- Ink/background and accent/background contrast are suitable.
- All three frames remain legible at tiny, narrow, square, and wide sizes.
- Geometry and spatial relationships differ from starter and solution, not only palette.
- Capture alt text names the three phases, hierarchy, motion, shape encoding, and palette roles.
- Any reused visual reference is credited.
