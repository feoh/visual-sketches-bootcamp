# Exercise 12: two-palette trail study

## The short version

You will keep older marks and fade them to make trails. Alpha controls how strongly a
new color covers the old picture. Repeated decay makes each older mark a little fainter
than the one before it.

Work through the small red-over-blue example first. Then use two palettes and non-color
cues so overlapping trails remain readable.

Turn a particle-memory or gesture-line idea into a study of luminous overlap and
controlled persistence. The pure C++17 model stores normalized straight RGBA,
interpolates two endpoint palettes, computes source-over known cases, applies limited
exponential alpha decay, and derives repeatable render marks from explicit
oldest-to-newest samples. openFrameworks supplies motion, input, and pixels, while the
plain C++ model keeps the calculations easy to check.

## What you choose

Edit `starter/src/design/trail_design.cpp` for history cap, retention, base opacity, radii, background, and
both palette endpoints. Edit `starter/src/ofApp.cpp` for path or gesture source, composition,
blend mode, sampling rhythm, and mark geometry. Replace the starter's normal-alpha
paired ribbons, circles, and squares with a visual system unlike both examples. The
solution is a dark additive rose/star loom using sparse crosses and diamonds. Changing
only colors is not sufficient.

Keep two explicit palette roles and a non-color cue for each. Pointer/drag and arrows
move the same visible origin. P pauses, R clears histories and restores the initial
phase, and M retains only current marks. Keep histories limited and renderer blend,
style, fill, line width, color, and matrix changes bracketed.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

The course supplies checked commands for Linux x86-64, macOS arm64, and Windows Visual
Studio 2022 x64 Developer PowerShell. On another system, the sketch may work, but you
may need to adapt the build steps.

Linux x86-64 or macOS arm64:

```sh
CXX=g++ tests/run-section-12-tests.sh
CXX=clang++ tests/run-section-12-tests.sh
scripts/section-12.sh generate --project starter
scripts/section-12.sh build --project starter --configuration Release
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\tests\run-section-12-tests.ps1
.\scripts\section-12.ps1 generate -Project starter
.\scripts\section-12.ps1 build -Project starter -Configuration Release
```

Keep `makeTrailDesign()` and declarations in `shared/color_trail_model.h`. Tests parse an independent
palette/composition/decay oracle. They cover exact palette endpoints, channel bounds,
source-over known cases and boundaries, alpha properties, decay monotonicity and
extremes, mark age/radius order, history caps, invalid/`NaN` or infinite
rejection, bad-input behavior that leaves previous output unchanged, and state-independent output across call order and
preexisting output. The design check only confirms that the editable values stay in safe ranges. Open the
app yourself to review its pixels; pictures can differ across graphics hardware.

## Notes for future you

A short note is enough. Pick the questions that help you remember your design:

- What happened when one partly transparent color crossed another?
- How did age change a mark's alpha, size, or spacing?
- How can someone tell the two trail roles apart without relying only on hue?

If you changed the blend mode, record which mode you used and why. Keep color channels
between 0 and 1 in the model, keep old marks before new marks, limit trail length, and
restore drawing settings after the trail is finished.

## Reference solution

The [explained solution](solution/README.md) is one answer, not a target image. Preserve repeatable number-based
tests while changing path source, palette roles, geometry, persistence, density, blend
treatment, and composition.

## Check the picture yourself

Tests cannot decide whether the trails are readable or appealing, so open both apps and
use the list below. If you are sharing a platform-specific bug report, it can help to
include your OS, openFrameworks version, graphics hardware, and build type.

- Normal-alpha starter and additive solution visibly differ in path, geometry, density,
  palette, background, and overlap.
- Circles/squares or crosses/diamonds identify palette roles without hue.
- Alpha plus size/spacing identifies old-to-new order; meaning is not color-only.
- Pointer/drag and arrows share an origin; P, R, and M work without a resume burst or
  stale trail.
- Text is legible, nothing flashes, and no information is audio-only.
- Resize and tiny-window behavior are safe.
- A diagnostic mark drawn after the trail block confirms no blend/style/matrix state
  leaked.
- Your result differs from both examples beyond palette substitution.
- Capture alt text names roles, overlap, age, geometry, controls, and reduced motion.
- Reused palettes, code, references, and assets are credited.
