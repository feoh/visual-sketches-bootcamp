---
title: Images and type as geometry
slug: 14-images-and-type-as-geometry
weight: 160
draft: false
course_kind: instructional
objectives:
  - Copy decoded image pixels into owned storage and compute checked row-major interleaved indices
  - Threshold a bounded sampling grid into points, then measure its bounds and centroid
  - Transform a point collection around its centroid while preserving count and predictable scaled distances
  - Report missing, malformed, empty, or overlarge assets explicitly before rendering
  - Create an accessible animated composition from an original phrase or redistributable image
prerequisites:
  - Completion of section 11 or equivalent vectors, finite guards, coordinate transforms, and openFrameworks event adapters
source_records: sources.yaml
asset_records: assets.yaml
---

# Images and type as geometry

## Look

![A grayscale block-letter SHAPE mask becomes a field of spaced navy dots, then curved amber bars orbit around a marked centroid; arrows label threshold sampling and transform, and distinct circle, crosshair, and bar shapes communicate each stage without color alone.](media/geometry-preview.svg "A mask becomes measured and animated geometry.")

*A flat mask becomes sampled points, measured geometry, and animated marks while retaining a visible centroid.*

Circle samples, a crosshair centroid, labeled arrows, and bar marks communicate
the stages without depending on color.

## Precedent and transfer

Use the course's [credited precedent notes](../../../docs/source-notes.md#visual-vocabulary)
to study the principle of treating image or type data as geometry. Credit the
creator and collaborators, use your own phrase or licensed source asset, and
change sampling, topology, motion, mapping, palette, and composition instead of
reconstructing a precedent image.

## Predict

A 4-by-3 grayscale image uses one byte per pixel. What flat index contains
`(x=2, y=1)`? If stride is 2, which x coordinates are visited? A dark mask
selects `(0,0)` and `(2,2)`: predict its bounds and centroid. Finally, if those
points rotate around the centroid and scale by 3, what changes and what stays
invariant?

## Learn

### Pixels become a checked grid

[`ofImage`](https://openframeworks.cc/documentation/graphics/ofImage/) decodes a
file, while [`ofPixels`](https://openframeworks.cc/documentation/graphics/ofPixels/)
exposes width, height, channel count, and bytes. The adapter checks `image.load`,
then copies those bytes into the pure model. The copy gives the model explicit
ownership; it never keeps a pointer into an image that might later be reloaded.

For interleaved channels, the symbolic index is:

```text
pixel = y * width + x
byte_index = pixel * channels + channel
```

Visually, rows are laid end to end. Numerically, `(2,1)` in width 4 and one
channel is `1*4+2 = 6`. With RGB it begins at byte `6*3 = 18`. The helper first
checks nonzero dimensions, 1–4 channels, every coordinate, multiplication and
addition overflow, and a 4,194,304-pixel source cap. It never guesses after a
failed check.

Grayscale uses its first channel. RGB/RGBA uses pinned integer luminance:

```text
Y = round((299*R + 587*G + 114*B) / 1000)
```

Alpha does not silently erase RGB. If transparency should define your mask,
make that an explicit new sampled rule and test it.

### Threshold and density turn a rectangle into a collection

For stride `step`, visit `(0, 0), (step, 0), ...`, then the next sampled row.
Dark selection includes `luminance <= threshold`; bright selection includes
`luminance >= threshold`. Inclusive boundaries are tested. In a 4-pixel row and
step 2, visited x values are 0 and 2, never 4.

This section's one substantial new C++ mechanism is transforming a copied
`std::vector` of points as owned geometry. Sampling can visit at most 100,000
grid positions and output at most 100,000 points. Step zero, unsafe dimensions,
truncated bytes, too much work, and a threshold selecting nothing return named
statuses. Output commits only on success, so a failed reload cannot create a
half-new collection.

### Bounds and centroid summarize shape

For selected points `p_i = (x_i,y_i)`, scan once for minimum and maximum and
accumulate:

```text
centroid = (sum(x_i)/N, sum(y_i)/N)
```

The two-point example `(0,0), (2,2)` has bounds `(0,0)` through `(2,2)` and
centroid `(1,1)`. In the preview, the rectangle encloses extent while the
crosshair marks average position. This is a point centroid, not area centroid:
a denser region has more influence. That is a useful artistic consequence of
sampling density, not a hidden equivalence.

### Transform around the centroid

Subtract the centroid, scale, rotate, then restore it and translate:

```text
q = p - centroid
rotated = (q.x*cos(a) - q.y*sin(a), q.x*sin(a) + q.y*cos(a))
result = rotated * scale + centroid + translation
```

Count remains exactly `N`. Translation moves the centroid by the same vector.
Rotation preserves pairwise distance; positive scale `s` multiplies every
pairwise distance by `s`. Tests verify these properties as well as a known
90-degree bounds case. Non-finite values, scale at or below zero, scale above
10,000, and excess work reject transactionally.

A tangent points along a contour; a normal points across it. The solution's
short rotating bars suggest local tangent direction, but it does not claim to
recover a mathematically ordered contour from unordered mask samples.

### Type contours are an extension, not the platform gate

Type can enter the same pipeline without making font raster pixels a test
oracle. Load a font you created or are licensed to redistribute with
[`ofTrueTypeFont`](https://openframeworks.cc/documentation/graphics/ofTrueTypeFont/),
request outlines, and inspect the resulting
[`ofPath`](https://openframeworks.cc/documentation/graphics/ofPath/) contours:

```cpp
ofTrueTypeFont font;
if (!font.load("licensed-font.ttf", 96, true, true, true)) {
    // Show an explicit asset/license error; do not render an empty success.
}
auto paths = font.getStringAsPoints("YOUR PHRASE");
```

Paths can be resampled into contour points or tessellated into an
[`ofMesh`](https://openframeworks.cc/documentation/3d/ofMesh/). Font hinting,
raster engines, and installed-font sets vary by platform, so cross-platform
tests gate the pure point/mask math instead. A font extension still needs a
bundled font license, a manual contour check, and the same work limit.

## Build: inspect three complete experiments

### 1. Read the independent tiny-mask oracle

```sh
cat exercises/14-images-and-type-as-geometry/fixtures/mask-oracle.txt
CXX=g++ tests/run-section-14-tests.sh
```

The cross has count 5, bounds 0–2 on each axis, and centroid `(1,1)`. The
separate bright and stride cases prevent one hard-coded selection policy.
Windows Visual Studio 2022 x64 Developer PowerShell uses:

```powershell
Get-Content .\exercises\14-images-and-type-as-geometry\fixtures\mask-oracle.txt
.\tests\run-section-14-tests.ps1
```

### 2. Compare both adapters

The starter samples dark ink as gently moving circles. The explained solution
samples bright negative space more coarsely and renders phased orbiting bars.
Both use the same checked source geometry, making visual divergence a design
choice rather than duplicated safety code.

### 3. Run the image instrument

Set `OF_ROOT` to openFrameworks 0.12.1. Supported native lanes are Linux x86-64,
macOS arm64, and Windows Visual Studio 2022 x64 Developer PowerShell. Other
versions and architectures are unattested and unsupported by these wrappers.

```sh
scripts/section-14.sh doctor
scripts/section-14.sh generate --project starter
scripts/section-14.sh build --project starter --configuration Release
```

On macOS, open the generated app bundle; on Linux, launch the generated binary.
On Windows:

```powershell
.\scripts\section-14.ps1 doctor -OfRoot C:\path\to\of
.\scripts\section-14.ps1 generate -OfRoot C:\path\to\of -Project starter
.\scripts\section-14.ps1 build -OfRoot C:\path\to\of -Project starter -Configuration Release
```

Without `OF_ROOT`, OF generation/build is unavailable. Pure C++ tests do not
prove an OF compile, launch, image decoder, GPU drawing, or font contours.

## Break and repair

In `exercises/14-images-and-type-as-geometry/shared/image_geometry_model.cpp`,
temporarily change `pixel * channels + channel` to
`pixel + channels + channel`. Run `tests/run-section-14-tests.sh` and explain
why one-channel origin pixels might hide the bug while RGB and final-byte cases
find it. Restore the exact formula and rerun. If this was your only edit:

```sh
git restore -- exercises/14-images-and-type-as-geometry/shared/image_geometry_model.cpp
```

This command discards all uncommitted work in that named file.

## Exercise

Open the [animated mask geometry brief](../../../exercises/14-images-and-type-as-geometry/README.md).
Replace the mask with an original phrase or image you may redistribute. First
use the learner seam in `starter/src/design/image_geometry_design.cpp`; then
change mark grammar or add licensed type contours. Preserve explicit failures,
hard limits, checked indexing, keyboard controls, and reduced motion.

## Test

```sh
CXX=g++ tests/run-section-14-tests.sh
CXX=clang++ tests/run-section-14-tests.sh
scripts/check-authoring.sh
```

Use the PowerShell suite on Windows. With `OF_ROOT`, generate and build starter
and solution in Debug and Release, then manually launch them. Temporarily rename
the asset to inspect failure behavior; restore it afterward. Automated tests
cover known, boundary, failure, limit, and transform properties, not pixels.

## Reflect

In 160–200 words, distinguish image ownership from borrowed bytes, pixel from
byte index, threshold from density, bounds from centroid, point centroid from
area centroid, source from transformed geometry, tangent from normal, and pure
invariants from platform-dependent pixels. Name one accessibility decision and
one original visual decision. Include alt text for a still.

## Remix

Sample edges instead of filled darkness, order points into tested scan-line
ribbons, connect a bounded mesh, use centroid distance as phase, or extend the
pipeline with a licensed font outline. Preserve the asset error, work caps,
checked indexing, deterministic geometry evidence, pause/reset, and reduced
motion. Do not treat an installed system font as redistributable by default.

## Manual review

- R reloads; P pauses without hidden state changes; M stops nonessential motion.
- Missing and empty-threshold assets show readable errors rather than blank success.
- The composition remains understandable by mark shape and position without color.
- Small and large windows keep finite geometry; no rapid flash or required audio exists.
- Source image or font creator, source, and redistribution license are recorded.
- Silhouette, sampling, motion, mark language, or palette materially differ from both examples.
- Captured alt text names source shape, sample treatment, centroid role, and motion.

## Pilot note

Pilot evidence not yet collected. Record platform and tool versions; reading,
prediction, repair, exercise, and reflection time separately; OF/image setup
friction; whether indexing, channel ownership, threshold boundaries, density,
bounds, point centroid, transform order/invariants, work limits, explicit asset
failure, type-contour portability, licensing, reduced motion, and originality
were understood; automated results; manual visual/accessibility results; and
points of confusion. Do not infer graphical success from pure model CI.
