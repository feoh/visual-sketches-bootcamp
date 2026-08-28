---
title: Controlled chance
slug: 06-controlled-chance
weight: 80
draft: false
course_kind: instructional
objectives:
  - Use a random engine, explicit seed, uniform draws, and weighted tickets deliberately
  - Explain uniform, weighted, and center-biased distributions as visible shapes
  - Distinguish same-build seed replay from serialized cross-toolchain parameter replay
  - Build exactly six related editions from simple generated records
  - Test repeatable properties, malformed serialization, and stroke-aware bounds without pixels
prerequisites:
  - Completion of section 05 or equivalent structs, vectors, loops, pure functions, and bounds literacy
source_records: sources.yaml
asset_records: assets.yaml
---

# Controlled chance

This section treats randomness as a design material: choose a repeatable seed,
choose the shape of the choices, and save exact parameters when a seed is not
enough.

1. [Lesson: control repeatable variation](#lesson)
2. [Practice: predict, run, and repair](#practice)
3. [Exercise: build a tested six-image edition](#exercise)

## Lesson

### One rule can produce a family

![Six bordered panels contain related but nonidentical arrangements of dark dots, horizontal orange dashes, and outlined rings; a small key labels the seed, weighted motif tickets, and center-biased position shape.](media/seeded-edition-preview.svg "Six related seeded editions.")

*One seed generates six related editions; shape and position distinguish motifs without relying on color.*

A computer's random-number generator behaves more like an elaborate card shuffle
than magic. A **seed** chooses the starting shuffle. The same seed and same rules
produce the same sequence again, giving a varied image a repeatable identity.

A **distribution** decides how engine output becomes useful choices. Uniform
choices treat equal ranges equally; weighted choices favor some outcomes;
center-biased choices make the middle more common. These are compositional tools,
not a substitute for choosing a composition.

### Engine, seed, and distribution have separate jobs

A random engine is a repeatable state machine:

```cpp
std::mt19937 engine(seed);
```

The standard defines the sequence for
[`std::mt19937`](https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine.html).
It is pseudo-random and repeatable, not secret.

A distribution maps that sequence into a range. The exercise uses
[`std::uniform_real_distribution<float>`](https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution.html)
for positions and radii. Exact distribution mapping is not required to match
between every C++ standard-library implementation, even when the engine and seed
match.

That creates three different replay promises:

1. **Concept replay:** a seed and algorithm describe the same process.
2. **Same-build replay:** the same source, library build, parameters, and seed
   reproduce the same serialized model.
3. **Cross-toolchain parameter replay:** saved generated records recreate the
   exact model without running the distribution again.

The adapter uses
[openFrameworks file utilities](https://openframeworks.cc/documentation/utils/ofFileUtils/)
when `S` saves `edition-parameters.txt`. The `CONTROLLED_CHANCE_V1` format stores
seed metadata and exactly six ordered editions. Its parser rejects wrong
versions, missing or extra records, non-finite numbers, unknown motifs, bad
ranges, and noncontiguous indices.

### Generate records before drawing

The model stores choices without any drawing call:

```cpp
struct MarkParameter {
    float x_unit;
    float y_unit;
    float radius;
    Motif motif;
};
struct Edition {
    int index;
    std::vector<MarkParameter> marks;
};
```

Separating records from rendering lets tests inspect values and another program
parse saved parameters. Drawing then becomes a separate visual interpretation.

### Weighted tickets make hierarchy explicit

Motifs use an integer from
[`std::uniform_int_distribution<int>`](https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution.html)
between 0 and 9:

```text
0 1 2 3 4 5 -> dot   (six tickets)
6 7 8       -> dash  (three tickets)
9           -> ring  (one ticket)
```

The intended proportions are 60%, 30%, and 10%. Exact ticket boundaries are
testable; a small random sample's observed percentages are not a reliable oracle.

### Averaging uniforms creates a visible distribution shape

One uniform coordinate spreads values evenly in expectation. Averaging two
uniform values favors the center:

```text
center_biased = (uniform_a + uniform_b) / 2
```

Few pairs average near 0 or 1; many average near 0.5. This triangular shape
creates a denser middle and quieter edges. The property test checks all pairs on
a fixed grid instead of trusting a lucky random sample.

One continuing engine creates editions 0 through 5. All six share the same rules
but receive different records. Responsive panel geometry maps normalized values
after generation and reserves maximum radius, half the stroke, and an outer
margin. A panel smaller than `64 × 64` or any non-finite parameter is invalid.

## Practice

Predict the choice rules and inspect a known-good edition without using the
section unit-test runner.

### 1. Work the ticket and layout numbers

1. Tickets 5, 6, 8, and 9 map to dot, dash, dash, and ring.
2. Uniform values 0.2 and 0.8 average to 0.5.
3. Six editions with 20 marks each contain 120 records.
4. With maximum radius 8, the inset is `8 + 1.5 + 2 = 11.5` pixels.
5. A seed is enough for same-build replay; saved parameters are needed for this
   section's exact cross-toolchain replay.

Predict each result first. Then draw ten ticket boxes and shade the six/three/one
groups so the hierarchy is visible.

### 2. Build and explore the working example

```sh
scripts/section-06.sh generate --project solution
scripts/section-06.sh build --project solution --configuration Release
```

On Windows Developer PowerShell:

```powershell
.\scripts\section-06.ps1 generate -Project solution
.\scripts\section-06.ps1 build -Project solution -Configuration Release
```

Open the app. Press `R` and confirm the same edition returns. Press `N` and
identify what changes while the six-panel grammar remains. Press `S`, then open
`edition-parameters.txt` and find its version, seed, edition indices, normalized
positions, radii, and motif codes.

### 3. Repair a weighted boundary

In `exercises/06-controlled-chance/shared/edition_model.cpp`, temporarily change:

```cpp
if (ticket >= 0 && ticket < 6) return Motif::dot;
```

to:

```cpp
if (ticket >= 0 && ticket < 5) return Motif::dot;
```

Before rebuilding, write the new ticket counts. Build and run the solution, then
inspect several `N` variations and compare the motif hierarchy with your ten-box
diagram. Restore `< 6` and rebuild; do not regenerate.

If that was your only intended edit:

```sh
git restore -- exercises/06-controlled-chance/shared/edition_model.cpp
```

That command discards every uncommitted change in the named file.

## Exercise

### Problem: create a six-image seeded edition

Build exactly six related images from one seeded model. Choose a seed, marks per
edition, radius range, and palette, then create a visual grammar distinct from
the starter's dots/dashes/rings and the solution's connected routes. Preserve
the explicit six/three/one ticket rule, center-biased positions, `R`/`N`/`S`
controls, responsive bounds, and portable serialization contract.

Use the
[Exercise 06 brief, starter, tests, and solution](../../../exercises/06-controlled-chance/README.md)
as the authoritative requirements. Begin in
`starter/src/design/edition_design.cpp`, then edit `starter/src/ofApp.cpp`.

### Run the unit tests

Linux or macOS, using both compilers when available:

```sh
CXX=g++ tests/run-section-06-tests.sh
CXX=clang++ tests/run-section-06-tests.sh
```

Windows Developer PowerShell:

```powershell
.\tests\run-section-06-tests.ps1
```

The tests check same-build seed replay, exactly six editions, legal ranges and
motifs, exact six/three/one ticket mapping, the center-biased distribution
property, stable serialization round trips, strict seed grammar, malformed
input rejection, finite three-pixel stroke-aware bounds at `64 × 64`, invalid
smaller viewports, and your design choices. They compile the starter design and
do not compare screenshots.

After tests pass, generate and build the starter. Exercise `R`, `N`, and `S`,
inspect the saved text, and view all six panels at minimum, narrow, square, and
wide sizes.

### Quick visual check

- `R`, `N`, and `S` work from the keyboard; no panel flashes.
- Motif roles and edition differences remain legible without color alone.
- Ink/background and accent/background contrast are suitable.
- Exactly six images remain clear at minimum, narrow, square, and wide sizes.
- Geometry differs from starter and solution in more than palette.
- Capture alt text names six images, density shape, motif encoding, and palette.
- Saved parameters record the inspected model; reused work remains credited.

### If you get stuck

If the sketch changes unexpectedly, print the seed and confirm the model uses
the seeded engine rather than a global generator. If every image looks alike,
verify that rendering reads the generated records. For a weighting failure,
draw tickets 0 through 9 and inspect the first boundary that differs.
