# Five-primitive visual signature

## Visual behavior

Compose a static 800 × 600 first frame from **exactly five primitives** and
**exactly three palette colors**. Every primitive's nominal bounding box must
remain inside the window after resize. There is no reference-image or pixel
gate; manually inspect whether the renderer's added stroke width appears
clipped.

## Learner-owned choices

Edit only `starter/src/design/signature_design.cpp` at first. Choose the five primitive
kinds, normalized positions and sizes, drawing order, three RGB colors, and how
the forms relate. Keep the result meaningfully different from the starter row
and reference solution; palette alone is not enough.

## Assessed concepts and interface

The C++ mechanism is calling functions across a header/source boundary. The
math is normalized screen coordinates: `pixel_x = normalized_x * width` and
`pixel_y = normalized_y * height`. A spec's `normalized_size` is its full
viewport-relative width and height, so `half_width = normalized_size.x * width
/ 2` (and likewise for height). For size `0.10 × 0.20` at 800 × 600, the built
half-extents are 40 × 60 pixels. Supplied tests call:

```cpp
signature::Signature buildSignature(const signature::SignatureSpecs&, signature::Viewport);
bool signature::isInBounds(const signature::Primitive&, signature::Viewport);
bool signature::signatureIsInBounds(const signature::Signature&, signature::Viewport);
```

`Signature` is a `std::array<Primitive, 5>` and `Palette` is a
`std::array<Color, 3>`, making both counts inspectable without rendering.

## Deterministic inputs and edge cases

There is no time, randomness, live input, or asset dependency. The fixture
supplies viewports from 1 × 1 through 1920 × 1080. The geometry helper clamps
normalized geometry values and treats non-finite or non-positive viewport
extents as one pixel. It preserves color indices: invalid indices fail the
public contract instead of being hidden by modulo wrapping. Rendering chooses
palette entry 0 as a deterministic fallback only if an invalid design is
launched before testing. Public tests compile
`starter/src/design/signature_design.cpp` and cover exact spec/palette counts, finite
normalized fields, RGB channels, primitive kinds, palette indices, a known
mapping with approximate floating-point comparisons, helper boundaries, and
nominal in-bounds geometry at every fixture viewport. They do not account for
pixels added by line width.

## Build and test

First complete the pinned setup/doctor steps in the
[lesson](../../authoring/sections/00-cross-platform-setup/index.md). Generated
Make, Xcode, Visual Studio, and VS Code files are ignored and must never be
committed.

Linux or macOS, from the repository root:

```sh
export OF_ROOT=/absolute/path/to/of_v0.12.1_platform_release
scripts/section-00.sh generate --project starter
scripts/section-00.sh build --project starter --configuration Release
tests/run-section-00-tests.sh
```

Windows Developer PowerShell, from the repository root:

```powershell
$env:OF_ROOT = 'C:\absolute\path\to\of_v0.12.1_vs_64_release'
.\scripts\section-00.ps1 generate -Project starter
.\scripts\section-00.ps1 build -Project starter -Configuration Release
.\tests\run-section-00-tests.ps1
```

Use `solution` instead of `starter` to generate/build the explained reference.
Generation, compilation, and pure deterministic test execution are separate
gates. Running the resulting windowed executable is a fourth, manual gate.

## Starter, tests, fixtures, and solution

- [`starter/`](starter/) is a complete plain row whose intended completion gap
  is visual authorship in `signature_design.cpp`.
- [`tests/`](tests/) contains known-case, boundary, and property checks.
- [`fixtures/`](fixtures/) contains explicit viewport inputs and provenance.
- [`solution/`](solution/) contains one instructional reference and explanation,
  not a target composition.
- [`shared/`](shared/) contains renderer-independent C++17 geometry used by the
  apps and tests.

## Manual visual and originality review

- Exactly five forms and three colors are visibly intentional.
- No fill or thick stroke appears clipped at 800 × 600 or after resizing
  narrower and wider; this appearance check is manual.
- Shape or placement—not color alone—distinguishes forms where needed.
- Contrast is sufficient for the intended viewing context.
- The composition differs from both supplied rows in geometry and spatial logic.
- The terminal shows a clean build and passing model test; appearance is judged
  by a person, never a cross-platform screenshot comparison.

## Reflection

Save an accessible capture with alt text. In 80–120 words, identify the
coordinate choice that most affected the composition, explain one header/source
boundary, and name two visual decisions that are yours.
