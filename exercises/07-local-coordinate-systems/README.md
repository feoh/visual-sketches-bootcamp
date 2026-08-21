# Exercise 07: three-frame hierarchical kinetic sculpture

## The short version

You will build a small moving sculpture from parent and child parts. Each child uses its
parent's position and rotation as a starting point, so you can describe “the small arm
at the end of the large arm” instead of calculating everything from the window corner.

The push and pop calls save and restore that temporary measuring space.

Build one responsive composition containing the same parent-child sculpture at three
phases. The pure C++17 model composes translation, rotation, and scale, then exposes
pivot, elbow, and tip anchors for inspection without a screenshot comparison.

## What you choose

Edit `starter/src/design/sculpture_design.cpp` first. Own the arm lengths, ornament radius, period, parent and
child swing, and three-color palette. Then replace the starter's line-and-ring language
in `starter/src/ofApp.cpp` with your own geometry. The explained solution uses filled triangular
ribbons, counter-motion, a square counterweight, and a night palette. Make a structural
change rather than only recoloring either route.

Keep each local drawing centered on its own origin and keep `MatrixScope` around every
pushed matrix. The scope guard balances `ofPushMatrix()` with `ofPopMatrix()` on all exits;
it is not an invitation to add pointers or manual ownership.

## Test and build

```sh
CXX=g++ tests/run-section-07-tests.sh
CXX=clang++ tests/run-section-07-tests.sh
scripts/section-07.sh generate --project starter
scripts/section-07.sh build --project starter --configuration Release
```

Use the matching `.ps1` scripts from Windows Developer PowerShell. Project
Generator 0.103.0 owns the generated files. The course supplies checked build commands
for Linux, macOS, and Windows. After compiling, open the app.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

Keep `makeSculptureDesign()` and declarations in `shared/sculpture_model.h`. Tests cover identity,
translation, positive-down rotation, scale, explicit zero/`NaN` or
infinite-scale rule, noncommutative transform order, a strictly parsed independent
three-frame anchor fixture, parent-child composition, repeatable periodic time, invalid
designs, `48 x 48` and narrow/wide stroke-aware bounds, and the tested limits for
your design. There is no screenshot comparison.

## Check the result yourself

- All geometry is static per frame and nothing flashes.
- Parent, child, pivot, and ornament remain distinguishable without color.
- Ink/background and accent/background contrast are suitable.
- All three frames remain legible at tiny, narrow, square, and wide sizes.
- Geometry and spatial relationships differ from starter and solution, not only palette.
- Capture alt text names the three phases, hierarchy, motion, shape encoding, and
  palette roles.
- Any reused visual reference is credited.
