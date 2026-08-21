# Exercise 01: a pointer-responsive wraparound traveler

## The short version

You will make one mark move, pause, reset, wrap around the window, and respond to the
pointer. The important calculation is small: movement during one update is speed
multiplied by elapsed time.

Most of the support code is already written. You choose what the mark looks like and
edit a small design file before changing the drawing code.

Build a moving mark whose model is repeatable without a renderer. The mark moves toward
the most recent pointer location. Before any pointer event—or whenever an arrow key is
pressed—the keyboard direction is the fallback.

## What you choose

Edit only `starter/src/design/traveler_design.cpp` first. Choose a normalized start, speed from 30–500
pixels/second, radius from 4–80 pixels, and three RGB colors. The mark and background
must differ. Then make the rendering recognizably yours: change the mark silhouette or
add a non-flashing geometric trail while preserving the public model API. The starter
uses a circle and one trail line; the explained solution uses a diamond and two parallel
trail lines. Choose geometry distinct from both. There is no target screenshot and no
screenshot comparison.

## Controls and what happens at the edges

- Move/drag pointer: steer toward it.
- Arrow keys: select keyboard fallback and disable pointer steering.
- Space: pause/resume; paused frames discard accumulated catch-up time.
- `M`: toggle reduced motion at one-quarter speed.
- `R`: reset position, velocity, pause, and accumulator while preserving the
  reduced-motion preference.
- A center may travel one radius beyond an edge, then wraps with overshoot to the
  opposite outer edge. Resize is applied at the next fixed step.
- Frame durations are seconds. `NaN` or infinite and non-positive values do not
  move. Positive durations clamp to 0.1 second and are consumed as 1/120-second fixed
  steps. The remainder stays in the accumulator. This prevents a long pause from causing
  an unlimited catch-up burst.

## Test and build

Pure C++17 public tests compile the starter design, not the solution:

```sh
tests/run-section-01-tests.sh
scripts/section-01.sh generate --project starter
scripts/section-01.sh build --project starter --configuration Release
```

On macOS use the same POSIX commands. In Windows Developer PowerShell:

```powershell
.\tests\run-section-01-tests.ps1
.\scripts\section-01.ps1 generate -Project starter
.\scripts\section-01.ps1 build -Project starter -Configuration Release
```

Set `OF_ROOT` as in section 00. Project Generator 0.103.0 owns the generated files, so
leave them alone and keep them out of Git. The course supplies checked build commands for
Linux, macOS, and Windows. After building on your system, open the app to check its
motion and appearance.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

Do not rename `makeTravelerDesign()` or the declarations in `shared/traveler_model.h`. Tests parse every
frame-stream fixture column and drive one-step, partition, pause-spike, and boundary
scenarios from those records. They also cover zero, negative, NaN, and infinite
durations, all four exact wrap endpoints, reset, reduced-motion persistence, and
finite/ranged learner choices with distinct mark/background RGB values. Sufficient
visual contrast and your trail/silhouette distinction remain manual checks; tests
inspect state and never pixels.

## Manual review

- Pointer and arrow controls have an immediately legible effect.
- Space pauses without a jump on resume; `M` visibly reduces speed;
  `R` repeats the start.
- Motion remains comprehensible at narrow and wide sizes and wraps cleanly.
- The traveler has a non-color direction/motion cue and sufficient contrast.
- No flashing or rapid full-field changes occur.
- Your silhouette, trail, mapping, or spatial behavior differs from starter and
  solution—not only colors.
- Capture a still with alt text describing position, direction cue, and spatial
  relationship.
