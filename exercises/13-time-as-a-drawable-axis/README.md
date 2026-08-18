# Exercise 13: spatial-temporal collage

Build a collage in which spatial position selects one retained moment. The
shared C++17 model owns bounded samples, next-write indexing, explicit frame and
time, deterministic age selection, exponential decay, checked memory, resize,
and reset. openFrameworks adapts a clock and renders selected state.

## Learner-owned choices

Edit `starter/src/design/temporal_design.cpp` for history capacity, slice count,
decay, mark size, and palette. Edit `starter/src/ofApp.cpp` for spatial mapping
and geometry. Keep the checked shared model contract, but replace the starter's
filled vertical windows with a composition unlike both examples. The solution
uses a dark field, thin stems, rotated open diamonds, phase ticks, and a denser,
longer history.

Your result must make newest-to-oldest direction understandable without color.
P pauses, R replays from frame/time zero, and M freezes capture as a
reduced-motion still. Resize resets against the new viewport rather than
retaining out-of-bounds positions. Tiny windows suppress the collage safely.

## Public contract

Supported native lanes are Linux x86-64, macOS arm64, and Windows Visual Studio
2022 x64 Developer PowerShell. Other architectures are unattested and
unsupported by the wrappers.

Linux x86-64 or macOS arm64:

```sh
CXX=g++ tests/run-section-13-tests.sh
CXX=clang++ tests/run-section-13-tests.sh
scripts/section-13.sh generate --project starter
scripts/section-13.sh build --project starter --configuration Release
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\tests\run-section-13-tests.ps1
.\scripts\section-13.ps1 generate -Project starter
.\scripts\section-13.ps1 build -Project starter -Configuration Release
```

Keep `makeTemporalDesign()` and declarations in `shared/temporal_history.h`.
Tests cover chronological order, modulo wraparound, bounded length, spatial age
selection, decay, resize/reset, deterministic replay, actual `sizeof(Sample)`
memory bounds, NaN/infinity/counter rejection, fixture cardinality, and the
property that moving spatially toward 1 never selects a newer age. Design tests
prove an editable valid seam, not originality. No pixels or wall-clock timing
are portable correctness gates.

## Required explanation

Explain:

1. why age zero uses `(next + capacity - 1) modulo capacity`;
2. capacity versus retained length and oldest-to-newest versus age order;
3. how normalized position rounds to one discrete age;
4. how exponential decay differs from feedback gain;
5. why frame and time are explicit, monotone inputs;
6. how `capacity * sizeof(Sample)` is checked before allocation;
7. why shrink keeps the newest suffix and resize resets the OF viewport adapter;
8. one non-color age cue and one reduced-motion choice.

## Optional FBO boundary

The delivered projects render directly. If you add `ofFbo`, keep it out of the
pure model and tests. Manually inspect allocation, texture orientation,
alpha/clear behavior, resize reallocation, GPU limits, and separate read/write
targets on every platform you claim. Do not treat one driver's pixels as a
cross-platform oracle.

## Reference solution

The [explained solution](solution/README.md) is one answer, not a target image.
It documents why its time loom differs from the starter. Preserve deterministic
model evidence while changing spatial grammar, temporal range, geometry,
density, composition, palette, and controls.

## Manual accessibility and originality review

- NOW/THEN or equivalent geometry communicates temporal direction without color.
- P, R, and M work; reset starts from frame/time zero and reduced motion is still.
- Fill, wrap, resize, and tiny-window behavior do not reveal unwritten samples.
- Text and marks have suitable contrast; nothing flashes and no cue is audio-only.
- Spatial grammar, density, temporal range, geometry, composition, and palette differ from both examples.
- Capture alt text names newest/oldest direction, shape, decay, controls, and palette roles.
- Any FBO experiment receives the full manual boundary review above.
- Reused code, references, and assets are credited.
