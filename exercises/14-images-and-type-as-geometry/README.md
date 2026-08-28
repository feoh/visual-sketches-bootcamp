# Section 14 exercise: animated geometry from a mask

Complete the [Lesson and Practice](../../authoring/sections/14-images-and-type-as-geometry/index.md#lesson)
before starting this problem. This page is the authoritative exercise brief.

## The short version

You will read dark pixels from an image and turn them into points. A threshold chooses
which pixels count. The centroid is the average position of all kept points, giving you
a center for moving, rotating, and scaling the shape.

Start with the tiny supplied image. It is small enough to count every pixel and check
the list indexing yourself.

Transform an **original phrase** or a redistributable image into animated points,
contours, or mesh-like marks. The bundled `SHAPE` mask is an original starting
fixture, not a target to imitate.

## Build the pure model first

POSIX (GCC or Clang):

```sh
CXX=c++ tests/run-section-14-tests.sh
```

Windows Developer PowerShell (MSVC):

```powershell
./tests/run-section-14-tests.ps1
```

The tests do not require openFrameworks. They cover checked interleaved pixel indices,
luminance and inclusive thresholds, sample density/counts, bounds, centroids, failures,
hard limits, and transform invariants. Font raster pixels are deliberately not an
oracle: rasterization differs by font engine and OS.

## Build an openFrameworks adapter

Set `OF_ROOT` to a supported openFrameworks checkout, then from the repository root
run:

```sh
scripts/section-14.sh doctor
scripts/section-14.sh generate --project starter
scripts/section-14.sh build --project starter
```

On Windows:

```powershell
./scripts/section-14.ps1 doctor -OfRoot C:\path\to\of
./scripts/section-14.ps1 generate -OfRoot C:\path\to\of -Project starter
./scripts/section-14.ps1 build -OfRoot C:\path\to\of -Project starter
```

An openFrameworks build needs `OF_ROOT`; the plain C++ model tests remain available
without it. Generated project files are disposable. The wrapper verifies tools before
cleaning generated output.

## Design section and brief

Edit only `starter/src/design/image_geometry_design.cpp` first. Choose sampling step, threshold direction/value, mark
radius, motion amplitude/rate, and colors. Then replace `starter/bin/data/seed-mask.png` with an image you
created or may redistribute. Preserve that exact filename, or deliberately update
`loadAsset`. Keep the image modest: the model rejects more than 4,194,304 source
pixels and more than 100,000 sampling visits/points.

The adapter loads with `ofImage`, copies its `ofPixels` into owned standard C++
bytes, samples once, and transforms the resulting geometry every frame. Missing,
truncated, overlarge, or empty-threshold assets produce visible explicit errors.

Controls: `R` reload; `P` pause; `M` reduced motion.

## Check the picture yourself

- Use an image you made, or write down its creator, source, and license.
- Change the structure—not only the palette—so the silhouette, sampling, motion, or
  marks reflect your idea rather than the examples.
- Keep the source recognizable enough for your intent without merely reproducing its
  rectangle.
- Press `M`: nonessential animation should stop while the still image remains useful.
- Temporarily rename `seed-mask.png` and make sure the error message is readable.
- Try narrow and large windows and look for lost or broken shapes.
- Give the result alt text and a caption that explain shape and motion without relying
  only on color.

The solution is an explained reference, not an official target appearance: it samples
bright negative space and draws orbiting bars, while the starter samples dark ink and
draws a moving point constellation.
