# Exercise 01: a pointer-responsive wraparound traveler

Build a moving mark whose model is deterministic without a renderer. The mark moves toward the most recent pointer location. Before any pointer event—or whenever an arrow key is pressed—the keyboard direction is the fallback.

## Learner-owned choices

Edit only `starter/src/design/traveler_design.cpp` first. Choose a normalized start, speed from 30–500 pixels/second, radius from 4–80 pixels, and three RGB colors. The mark and background must differ. Then make the rendering recognizably yours: change the mark silhouette or add a non-flashing geometric trail while preserving the public model API. The starter uses a circle and one trail line; the explained solution uses a diamond and two parallel trail lines. Choose geometry distinct from both. There is no target screenshot and no pixel gate.

## Controls and edge policy

- Move/drag pointer: steer toward it.
- Arrow keys: select keyboard fallback and disable pointer steering.
- Space: pause/resume; paused frames discard accumulated catch-up time.
- `M`: toggle reduced motion at one-quarter speed.
- `R`: reset position, velocity, pause, and accumulator while preserving the reduced-motion preference.
- A center may travel one radius beyond an edge, then wraps with overshoot to the opposite outer edge. Resize is applied at the next fixed step.
- Frame durations are seconds. Non-finite and non-positive values do not move. Positive durations clamp to 0.1 second and are consumed as 1/120-second fixed steps. The remainder stays in the accumulator. This prevents a long pause from causing an unbounded catch-up burst.

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

Set `OF_ROOT` as in section 00. Project Generator 0.103.0 owns generated metadata; never commit or edit it. Native support is evidence-driven: require passing `section-01-linux-build`, `section-01-macos-build`, and `section-01-windows-build` statuses on the commit before claiming those builds. Running and visually inspecting the graphical app remains separate evidence.

## Public contract

Do not rename `makeTravelerDesign()` or the declarations in `shared/traveler_model.h`. Tests parse every frame-stream fixture column and drive one-step, partition, pause-spike, and boundary scenarios from those records. They also cover zero, negative, NaN, and infinite durations, all four exact wrap endpoints, reset, reduced-motion persistence, and finite/ranged learner choices with distinct mark/background RGB values. Sufficient visual contrast and learner-owned trail/silhouette distinction remain manual checks; tests inspect state and never pixels.

## Manual review

- Pointer and arrow controls have an immediately legible effect.
- Space pauses without a jump on resume; `M` visibly reduces speed; `R` repeats the start.
- Motion remains comprehensible at narrow and wide sizes and wraps cleanly.
- The traveler has a non-color direction/motion cue and sufficient contrast.
- No flashing or rapid full-field changes occur.
- Your silhouette, trail, mapping, or spatial behavior differs from starter and solution—not only colors.
- Capture a still with alt text describing position, direction cue, and spatial relationship.
