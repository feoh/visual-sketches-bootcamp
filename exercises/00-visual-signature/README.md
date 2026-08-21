# Five-shape visual signature

## What you are making

Make a still image from **exactly five shapes** and **exactly three colors**. The
starter opens an 800 × 600 window, but your shapes should also stay inside the window
when you resize it.

There is no “correct” picture to copy. The tests can count shapes, check colors, and
catch shapes that go off-screen. They cannot decide whether your design is interesting.
That part belongs to you.

## Start with one file

At first, edit only:

```text
starter/src/design/signature_design.cpp
```

This file contains a three-color palette and five lines that describe shapes. Here is
the starter's first shape:

```cpp
{PrimitiveKind::circle, {0.20f, 0.50f}, {0.07f, 0.10f}, 0U}
```

Read it from left to right:

- `PrimitiveKind::circle` — draw a circle;
- `{0.20f, 0.50f}` — put its center 20% across and halfway down the window;
- `{0.07f, 0.10f}` — make its full size 7% of the window width and 10% of the window
  height; and
- `0U` — use the first color in the palette. Use `1U` for the second
  color or `2U` for the third.

Change the five shape types, positions, sizes, order, and color numbers. Change the red,
green, and blue numbers in `makePalette()` to choose your three colors. Keep five shape
lines and three palette entries.

## A quick percentage refresher

The code uses decimals from `0.0` to `1.0` instead of fixed pixel
positions:

- `0.0` means 0%;
- `0.25` means 25%;
- `0.5` means 50%; and
- `1.0` means 100%.

The code calls these **normalized** values. That name only means “stored on a 0-to-1
scale.”

To turn a position into pixels, multiply by the window size. In an 800 × 600 window, a
center of `(0.25, 0.25)` lands at `(200, 150)` pixels:

```text
0.25 × 800 = 200
0.25 × 600 = 150
```

Sizes use the same idea. A size of `(0.10, 0.20)` means 10% of the window wide and 20%
tall. At 800 × 600, the full shape is `80 × 120` pixels.

The supplied helper stores half of each full size. That `80 × 120` shape gets a
`half_width` of `40` and a `half_height` of `60`. It can then
find the four edges by subtracting from and adding to the center. You do not need to
perform that bookkeeping yourself.

## What the supplied helper does

The starter calls these functions for you. In their names, `Viewport` simply means
the current window width and height.

```cpp
signature::Signature buildSignature(const signature::SignatureSpecs&, signature::Viewport);
bool signature::isInBounds(const signature::Primitive&, signature::Viewport);
bool signature::signatureIsInBounds(const signature::Signature&, signature::Viewport);
```

You do not need to call or rewrite them in this exercise. `buildSignature` turns your
percentage choices into pixel values. The other two functions check whether one shape—or
all five shapes—fits inside the window.

A `Signature` always contains five shapes, and a `Palette` always contains three
colors. That lets the tests count them without opening a graphics window.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

The tests try window sizes from 1 × 1 through 1920 × 1080. They check that:

- there are exactly five shapes and three different colors;
- every position and size is a normal number between `0.0` and `1.0`;
- every red, green, and blue value is between 0 and 255;
- every shape uses palette number `0U`, `1U`, or `2U`; and
- the shapes stay inside each test window.

The helper also protects itself from impossible window sizes and bad numbers. That
defensive code is already supplied; your job is to enter ordinary values from
`0.0` through `1.0`.

Tests do not judge the picture or account for every extra pixel added by a thick
outline. You will check those things by looking at the running app.

## Build and test

First complete the setup and doctor steps in the [lesson](../../authoring/sections/00-cross-platform-setup/index.md). Project Generator
creates machine-specific build files. Do not commit or hand-edit those generated files.

On Linux or macOS, run these commands from the repository root:

```sh
export OF_ROOT=/absolute/path/to/of_v0.12.1_platform_release
scripts/section-00.sh generate --project starter
scripts/section-00.sh build --project starter --configuration Release
tests/run-section-00-tests.sh
```

In Windows Developer PowerShell, run:

```powershell
$env:OF_ROOT = 'C:\absolute\path\to\of_v0.12.1_vs_64_release'
.\scripts\section-00.ps1 generate -Project starter
.\scripts\section-00.ps1 build -Project starter -Configuration Release
.\tests\run-section-00-tests.ps1
```

Use `solution` instead of `starter` if you want to build the example solution.
After the build and tests pass, open the resulting app and inspect it yourself.

## What is in this exercise folder

- [`starter/`](starter/) is the version you edit. It begins as five circles in a plain row.
- [`tests/`](tests/) checks the counts, numbers, and window boundaries.
- [`fixtures/`](fixtures/) lists the window sizes used by the tests.
- [`solution/`](solution/) shows one possible answer. It is an example, not a target.
- [`shared/`](shared/) contains the supplied percentage-to-pixel helper.

## Check the picture yourself

- Can you see five intentional shapes and three colors?
- Does anything look clipped at 800 × 600?
- What happens when you make the window narrow or wide?
- Can someone distinguish forms by shape or placement, not only by color?
- Do the shapes stand out from the background?
- Is your layout different from both the starter and the example solution?

## Optional notes for future you

Save a screenshot and give it useful alt text. Pick one percentage position and show
what pixel position it became, say why that choice mattered to your picture, and name
two visual decisions that are yours.
