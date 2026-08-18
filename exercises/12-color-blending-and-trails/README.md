# Exercise 12: two-palette trail study

Turn a particle-memory or gesture-line idea into a study of luminous overlap and
controlled persistence. The pure C++17 model stores normalized straight RGBA,
interpolates two endpoint palettes, computes source-over known cases, applies
bounded exponential alpha decay, and derives deterministic render marks from
explicit oldest-to-newest samples. openFrameworks supplies motion/input and
pixels, but does not own the evidence.

## Learner-owned choices

Edit `starter/src/design/trail_design.cpp` for history cap, retention, base
opacity, radii, background, and both palette endpoints. Edit
`starter/src/ofApp.cpp` for path or gesture source, composition, blend mode,
sampling rhythm, and mark geometry. Replace the starter's normal-alpha paired
ribbons, circles, and squares with a visual system unlike both examples. The
solution is a dark additive rose/star loom using sparse crosses and diamonds.
Changing only colors is not sufficient.

Keep two explicit palette roles and a non-color cue for each. Pointer/drag and
arrows move the same visible origin. P pauses, R clears histories and restores
the initial phase, and M retains only current marks. Keep histories bounded and
renderer blend, style, fill, line width, color, and matrix changes bracketed.

## Public contract

Supported native lanes are Linux x86-64, macOS arm64, and Windows Visual Studio
2022 x64 Developer PowerShell. Other architectures are unattested and
unsupported by the current wrappers.

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

Keep `makeTrailDesign()` and declarations in `shared/color_trail_model.h`.
Tests parse an independent palette/composition/decay oracle. They cover exact
palette endpoints, channel bounds, source-over known cases and boundaries,
alpha properties, decay monotonicity and extremes, mark age/radius order,
history caps, invalid/non-finite rejection, transactional behavior, and
state-independent output across call order and preexisting output. The design
hook proves only that an editable, valid seam exists. Pixels are manually
reviewed, never cross-platform screenshot gates.

## Required explanation

Explain:

1. why normalized straight RGBA is checked before interpolation or composition;
2. how each channel uses `first + (second - first) * amount` and preserves endpoints;
3. how `Ao = As + Ab(1 - As)` and weighted RGB produce half-red over blue;
4. why `alpha(age) = initial * retention^age` is monotone for retention in `[0, 1]`;
5. how oldest-to-newest vector order determines age, size, and bounded work;
6. why the render plan replays from explicit inputs without global renderer state;
7. why source-over arithmetic, normal-alpha rendering, and additive rendering are different contracts;
8. one shape/spacing cue, reduced-motion choice, and scoped-style boundary.

## Reference solution

The [explained solution](solution/README.md) is one answer, not a target image.
Preserve deterministic numeric contracts while changing path source, palette
roles, geometry, persistence, density, blend treatment, and composition.

## Manual accessibility, pixel, and originality review

Pure tests and native compilation do not perform this review. Record host,
openFrameworks version, GPU/driver, build configuration, and observations after
launching both adapters.

- Normal-alpha starter and additive solution visibly differ in path, geometry, density, palette, background, and overlap.
- Circles/squares or crosses/diamonds identify palette roles without hue.
- Alpha plus size/spacing identifies old-to-new order; meaning is not color-only.
- Pointer/drag and arrows share an origin; P, R, and M work without a resume burst or stale trail.
- Text is legible, nothing flashes, and no information is audio-only.
- Resize and tiny-window behavior are safe.
- A diagnostic mark drawn after the trail block confirms no blend/style/matrix state leaked.
- The learner result differs from both examples beyond palette substitution.
- Capture alt text names roles, overlap, age, geometry, controls, and reduced motion.
- Reused palettes, code, references, and assets are credited.
