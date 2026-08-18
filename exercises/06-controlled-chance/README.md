# Exercise 06: six-image seeded edition

Build one responsive plate containing exactly six related images. A C++17 model
uses `std::mt19937`, explicit uniform draws, a six/three/one weighted motif
choice, and the mean of two uniform values as a center-biased distribution.
The result is controlled variation, not a target screenshot.

## Learner-owned choices

Edit `starter/src/design/edition_design.cpp` first. Choose a seed, 4–48 marks
per edition, radii from 2–8 pixels, and ink/accent/background RGB colors. Then
edit `starter/src/ofApp.cpp` to invent a third composition and palette. The
starter presents six bordered paper samples with dots, horizontal dashes, and
rings. The explained solution instead joins records into routes and uses small
squares, vertical stems, and square outlines. Do not submit a recolor of either.

`R` rebuilds with the same seed, `N` advances the seed, and `S` saves canonical
`edition-parameters.txt`. A seed promises replay only with the same code,
standard-library implementation, and build contract. Save the parameter text
when another toolchain must render the exact edition; parsing it does not run a
random distribution again.

## Test and build

```sh
CXX=g++ tests/run-section-06-tests.sh
CXX=clang++ tests/run-section-06-tests.sh
scripts/section-06.sh generate --project starter
scripts/section-06.sh build --project starter --configuration Release
```

Use `tests/run-section-06-tests.ps1` and `scripts/section-06.ps1` from Windows
Developer PowerShell. Project Generator 0.103.0 owns native metadata. Green
`section-06-{linux,macos,windows}-build` statuses on the exact commit prove
compilation, not graphical runtime or visual quality.

## Public contract

Keep `makeEditionDesign()` and declarations in `shared/edition_model.h`.
Public tests compile the starter learner design and parse the independent
portable fixture. They check same-build seed replay, exactly six editions,
parameter ranges, exact six/three/one weighted-ticket mapping, legal generated
motifs and counts, a non-probabilistic triangular-distribution shape check,
stable serialization round trip, strict seed grammar, canonical signed zero,
malformed input, finite 3-pixel
stroke-aware bounds at `64 x 64`, invalid smaller viewports, and learner-owned
choices. There is no pixel gate.

## Manual accessibility and originality review

- `R`, `N`, and `S` work from the keyboard and no image flashes.
- Motif roles and edition differences remain legible without color alone.
- Ink/background and accent/background contrast are suitable.
- Six images remain understandable in narrow, square, wide, and minimum panel sizes.
- Composition or shape relationships differ from starter and solution, not only palette.
- Capture alt text names all six images, distribution shape, motif encoding, and palette role.
- Saved parameters replay the inspected image; reused work remains credited.
