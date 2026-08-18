# Section 14 exercise: animated geometry from a mask

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

The tests do not require openFrameworks. They cover checked interleaved pixel
indices, luminance and inclusive thresholds, sample density/counts, bounds,
centroids, failures, hard limits, and transform invariants. Font raster pixels
are deliberately not an oracle: rasterization differs by font engine and OS.

## Build an openFrameworks adapter

Set `OF_ROOT` to a supported openFrameworks checkout, then from the repository
root run:

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

Without `OF_ROOT`, an OF build is unsupported; the portable model tests remain
available. Generated project files are disposable. The wrapper verifies tools
before cleaning generated output.

## Learner seam and brief

Edit only `starter/src/design/image_geometry_design.cpp` first. Choose sampling
step, threshold direction/value, mark radius, motion amplitude/rate, and colors.
Then replace `starter/bin/data/seed-mask.png` with an image you created or may
redistribute. Preserve that exact filename, or deliberately update `loadAsset`.
Keep the image modest: the model rejects more than 4,194,304 source pixels and
more than 100,000 sampling visits/points.

The adapter loads with `ofImage`, copies its `ofPixels` into owned standard C++
bytes, samples once, and transforms the resulting geometry every frame. Missing,
truncated, overlarge, or empty-threshold assets produce visible explicit errors.

Controls: `R` reload; `P` pause; `M` reduced motion.

## Originality, accessibility, and manual visual check

- [ ] Use your own phrase/image or record creator, source, and redistribution license.
- [ ] Change at least three of silhouette, sampling rule, motion, mark language, and palette.
- [ ] The source remains legible enough for your intent but does not merely reproduce the rectangle.
- [ ] `M` stops nonessential animation and the still composition remains meaningful.
- [ ] Error text is readable when `seed-mask.png` is temporarily renamed.
- [ ] At narrow and large windows, geometry remains visible without invalid values.
- [ ] Record the image alt text and a caption explaining shape and motion without relying on color.

The solution is an explained reference, not a canonical appearance: it samples
bright negative space and draws orbiting bars, while the starter samples dark
ink and draws a moving point constellation.
