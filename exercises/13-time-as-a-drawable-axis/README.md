# Exercise 13: spatial-temporal collage

Complete the [Lesson and Practice](../../authoring/sections/13-time-as-a-drawable-axis/index.md#lesson)
before starting this problem. This page is the authoritative exercise brief.

## The short version

You will save a fixed amount of recent history and choose an older or newer entry from a
position on screen. The storage is a ring: after the last slot, the next write wraps
around and replaces the oldest value.

The tests use a tiny ring you can trace by hand before you run the larger visual
version.

Build a collage in which spatial position selects one retained moment. The shared C++17
model owns limited samples, next-write indexing, explicit frame and time, repeatable age
selection, exponential decay, checked memory, resize, and reset. openFrameworks adapts a
clock and renders selected state.

## What you choose

Edit `starter/src/design/temporal_design.cpp` for history capacity, slice count, decay, mark size, and palette.
Edit `starter/src/ofApp.cpp` for spatial mapping and geometry. Keep the checked shared model
interface and tests, but replace the starter's filled vertical windows with a
composition unlike both examples. The solution uses a dark field, thin stems, rotated
open diamonds, phase ticks, and a denser, longer history.

Your result must make newest-to-oldest direction understandable without color. P pauses,
R replays from frame/time zero, and M freezes capture as a reduced-motion still. Resize
resets against the new viewport rather than retaining out-of-bounds positions. Tiny
windows suppress the collage safely.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

The course supplies checked commands for Linux x86-64, macOS arm64, and Windows Visual
Studio 2022 x64 Developer PowerShell. On another system, the sketch may work, but you
may need to adapt the build steps.

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

Keep `makeTemporalDesign()` and declarations in `shared/temporal_history.h`. Tests cover chronological order,
modulo wraparound, limited length, spatial age selection, decay, resize/reset,
repeatable replay, actual `sizeof(Sample)` memory bounds, NaN/infinity/counter rejection,
fixture cardinality, and the rule that moving spatially toward 1 never selects a newer
age. The design test checks safe editable values, not appearance. Pixel output and real
clock timing still need a check in the running app.

## Notes for future you

Leave yourself a few plain-language reminders:

- Where is the newest sample stored, and how does the code walk backward to older ones?
- How does a left-to-right position choose one saved age?
- What happens when the history becomes full or you make its capacity smaller?
- Which shape, size, or spacing cue shows age without depending only on color?

You may include the modulo expression from the code if it helps you, but explain it as
“wrap around to the end of the list” rather than treating the formula as the lesson.

## Optional FBO boundary

The delivered projects render directly. If you add `ofFbo`, keep it out of the
pure model and tests. Manually inspect allocation, texture orientation, alpha/clear
behavior, resize reallocation, GPU limits, and separate read/write targets on the
systems you use. Do not expect one graphics driver's pixels to match every other one.

## Reference solution

The [explained solution](solution/README.md) is one answer, not a target image. It documents why its time loom
differs from the starter. Preserve the repeatable model checks while changing spatial
grammar, temporal range, geometry, density, composition, palette, and controls.

## Check the result yourself

- NOW/THEN or equivalent geometry communicates temporal direction without color.
- P, R, and M work; reset starts from frame/time zero and reduced motion is still.
- Fill, wrap, resize, and tiny-window behavior do not reveal unwritten samples.
- Text and marks have suitable contrast; nothing flashes and no cue is audio-only.
- Spatial grammar, density, temporal range, geometry, composition, and palette differ
  from both examples.
- Capture alt text names newest/oldest direction, shape, decay, controls, and palette
  roles.
- Any FBO experiment receives the full manual boundary review above.
- Reused code, references, and assets are credited.
