# Exercise 06: six-image seeded edition

## The short version

You will generate six related images from saved random seeds. The same seed and rules
produce the same choices again, so you can test and revisit an image even though it
looks varied.

You choose the ranges, weighted choices, shapes, and colors. The code keeps the random
choices separate from drawing.

Build one responsive plate containing exactly six related images. A C++17 model uses
`std::mt19937`, explicit uniform draws, a six/three/one weighted motif choice, and the
mean of two uniform values as a center-biased distribution. The result is controlled
variation, not a target screenshot.

## What you choose

Edit `starter/src/design/edition_design.cpp` first. Choose a seed, 4–48 marks per edition, radii from 2–8 pixels,
and ink/accent/background RGB colors. Then edit `starter/src/ofApp.cpp` to invent a third
composition and palette. The starter presents six bordered paper samples with dots,
horizontal dashes, and rings. The explained solution instead joins records into routes
and uses small squares, vertical stems, and square outlines. Make a structural change
rather than only recoloring either example.

`R` rebuilds with the same seed, `N` advances the seed, and
`S` saves the standard `edition-parameters.txt`. A seed promises replay only with the
same code, standard-library implementation, and build rules. Save the parameter text when another toolchain must render the exact edition. The tests
can parse it without running a random distribution again; the supplied starter saves
the file but does not load it unless you add that route.

## Test and build

```sh
CXX=g++ tests/run-section-06-tests.sh
CXX=clang++ tests/run-section-06-tests.sh
scripts/section-06.sh generate --project starter
scripts/section-06.sh build --project starter --configuration Release
```

Use `tests/run-section-06-tests.ps1` and `scripts/section-06.ps1` from Windows Developer PowerShell. Project
Generator 0.103.0 owns the generated files. The course supplies checked build commands
for Linux, macOS, and Windows. After compiling, open the app to check its picture.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

Keep `makeEditionDesign()` and declarations in `shared/edition_model.h`. Public tests compile the starter design and parse the independent portable fixture. They check same-build seed
replay, exactly six editions, parameter ranges, exact six/three/one weighted-ticket
mapping, legal generated motifs and counts, a non-probabilistic triangular-distribution
shape check, stable serialization round trip, strict seed grammar, one standard spelling
for signed zero, malformed input, finite 3-pixel stroke-aware bounds at `64 x 64`,
invalid smaller viewports, and your choices. There is no screenshot comparison.

## Check the result yourself

- `R`, `N`, and `S` work from the keyboard and no image
  flashes.
- Motif roles and edition differences remain legible without color alone.
- Ink/background and accent/background contrast are suitable.
- Six images remain understandable in narrow, square, wide, and minimum panel sizes.
- Composition or shape relationships differ from starter and solution, not only palette.
- Capture alt text names all six images, distribution shape, motif encoding, and palette
  role.
- Saved parameters record the inspected image for tests or a loader you add; reused work
  remains credited.
