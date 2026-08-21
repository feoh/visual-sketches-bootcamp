---
title: Particles with memory
slug: 09-particles-with-memory
weight: 110
draft: false
course_kind: instructional
objectives:
  - Keep particle count, trail length, and update work within clear limits
  - Integrate velocity and position with fixed steps, acceleration, exponential drag, and boundary response
  - Track age, lifetime, and limited oldest-to-newest history while removing many expired objects safely
  - Explain frame clamping, capped accumulator catch-up, dropped-time rule, pause, and repeatable spawning
  - Design an accessible history-trail emitter without making pixels the correctness gate
prerequisites:
  - Completion of section 08 or equivalent structs, vectors, functions, event adapters, and finite-value checks
source_records: sources.yaml
asset_records: assets.yaml
---

# Particles with memory

## See what you're making

![On a cream field, teal circles and coral squares follow two curved, limited oldest-to-newest trails from small earlier marks to large current heads; both trajectories stay inside the rounded viewport boundary, and shape plus size distinguish the palette roles without color alone.](media/particle-preview.svg "Objects carry short, limited memories through a fixed-step world.")

*Objects carry short, limited memories through a fixed-step world.*

The still uses head size, shape, and trail spacing as well as color.

## Borrow the idea, not the artwork

Use the course's [credited precedent notes](../../../docs/source-notes.md#visual-vocabulary) to identify a particle or trail mechanism without
treating its final frame as a recipe. Credit creators and collaborators, then document
deliberate changes to agent shape, emission, force, history, palette, and spatial
composition.

## Take a guess

Start one particle at `x=10` with `vx=10`, zero acceleration, drag rate
`1`, and a `0.25` second fixed step. Predict `vx` and
`x` after one and two steps using `drag = exp(-rate * dt)`, then `vx *= drag`, then
`x += vx * dt`. Which value changes if one half-second update is replaced by two
quarter-second updates? At exactly `age == lifetime`, should the object still be visible?

## Let's unpack it

### Before the particle vocabulary

A particle is a small record: position, velocity, age, lifetime, and perhaps a short
trail. A particle system owns many of those records and repeats the same update for each
one.

The course limits the number of particles, trail points, and catch-up steps so a slow
frame cannot create unlimited work. It also updates motion in fixed slices of time.
Think of a kitchen timer advancing one small tick at a time even when the screen
occasionally takes longer to redraw.

A particle's **lifetime** says when it should disappear. Its **age** says how long it
has existed so far. Keeping those as separate numbers makes fading and removal easier to
reason about.

### A vector can own objects

[`std::vector`](https://en.cppreference.com/w/cpp/container/vector.html) can store complete `Particle` objects rather than parallel position,
velocity, and age arrays. A constructor receives initial position, initial velocity, and
lifetime, establishes `age == 0`, and puts the initial point into history. Those
rules make bad starting state obvious: one object cannot accidentally begin with five
positions and four ages.

`Emitter` owns `std::vector<Particle> particles`. `push_back` constructs or moves a complete object
into the collection. Growth can invalidate references, pointers, and iterators, so do
not retain one across a spawn or erase. The explicit `maximum_particles` and `maximum_history`
limits bound both memory and work. History is always oldest-to-newest; after appending,
one range erase drops all excess oldest points without an unlimited loop.

### Update one object before updating the system

A particle update is a small, testable method boundary. This lesson uses a semi-implicit
order:

```text
v = (v + acceleration * fixed_dt) * exp(-drag_rate * fixed_dt)
position = position + v * fixed_dt
age = age + fixed_dt
```

The new velocity moves the position. Exponential drag is time-scaled: `exp(-r * 0.5)`
equals `exp(-r * 0.25)` multiplied by itself. That identity is an automated property; it
avoids making damping depend on render frequency. All configuration, time, state, and
intermediate results must be finite. An unsafe overflowing update leaves the particle
unchanged rather than letting NaN spread.

### Boundaries include the particle radius

The legal center interval is `[radius, width - radius]` and likewise for height. Crossing left,
right, top, or bottom clamps the center and reflects only an outward velocity, scaled by
restitution. If either viewport dimension is smaller than one diameter, there is no
legal interval: the frame is rejected before any clamp or simulation. Tests exercise all
four edges; screenshots do not.

### Lifetime and removal are different operations

Update increments age. A particle expires at `age >= lifetime`, including exact equality.
Removing while incrementing an index can skip adjacent expired objects, and erasing
through a range-for loop invalidates its iterator. Instead use [`std::remove_if`](https://en.cppreference.com/w/cpp/algorithm/remove.html) to move
survivors forward, then erase the returned tail:

```cpp
particles.erase(std::remove_if(particles.begin(), particles.end(), expired),
                particles.end());
```

Tests remove adjacent and non-adjacent expired objects simultaneously and verify
survivor order.

### Render frames feed a limited fixed-step accumulator

Variable render time is not simulation time. The accumulator follows the limited form of
[Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/):

1. reject negative or `NaN` or infinite frame time;
2. clamp accepted frame time to `maximum_frame_time` and count the excess as dropped;
3. add accepted time to the accumulator;
4. simulate at most `maximum_catch_up_steps` fixed updates;
5. explicitly drop additional whole steps, retaining only a substep remainder.

The cap prevents a slow frame from causing an unlimited catch-up loop—the “spiral of
death.” The amount of dropped time is recorded in both `FrameResult` and the emitter
total. This rule sacrifices wall-clock catch-up to keep work limited; it does not
silently pretend every elapsed second was simulated.

A spawn clock advances inside fixed steps, not draw calls. At each cadence it constructs
at most one particle because valid cadence is at least one fixed step. Capacity suppresses
additional spawns without overflowing the vector. The supplied random-number helper
makes launch directions repeat for the same seed and vary for another. Every seed,
including zero, replays the same sequence. The seed is a design input, not hidden global
randomness.

### Pause cannot accumulate a resume spike

P sets paused state and clears the accumulator. Paused frames simulate nothing and
accrue no backlog, even if the next reported frame is huge. Resume consumes only newly
supplied time. R resets particles, clocks, the dropped-time counter, the origin, and the
explicit seed. M is the reduced-motion control: it hides trails while retaining current
positions and controls. In the solution, every current head keeps a full-radius
triangular outline while its limited interior triangle grows from 35% to 80% of that
radius with normalized age. This noncolor age cue remains visible in reduced mode and
never exceeds the radius-aware bounds. Pointer/drag and arrow keys both move the same
model origin through the openFrameworks [event adapter](https://openframeworks.cc/documentation/events/ofEvents/).

### Test particle values; inspect the finished drawing yourself

The repeatable tests inspect fixture values, spawn counts, frame partitions, drop
accounting, expiry, removal, drag, boundaries, history order, seed replay, variation,
finite values, and your configuration. They do not sleep, measure real frame rate,
launch a window, or compare pixels. Contrast, motor access, reduced motion, launch
behavior, and originality still need manual review.

## Make it run: construct, step, and inspect

### 1. Replay the independent values

From the repository root on Linux or macOS:

```sh
cat exercises/09-particles-with-memory/fixtures/particle-oracle.txt
CXX=g++ tests/run-section-09-tests.sh
```

Confirm the first velocity is about `7.788008` and the first position about
`11.947002`. On Windows Developer PowerShell:

```powershell
Get-Content .\exercises\09-particles-with-memory\fixtures\particle-oracle.txt
.\tests\run-section-09-tests.ps1
```

### 2. Build the openFrameworks adapter

Set `OF_ROOT` to openFrameworks 0.12.1. On Linux:

```sh
scripts/section-09.sh generate --project starter
scripts/section-09.sh build --project starter --configuration Release
exercises/09-particles-with-memory/starter/bin/starter
```

On Windows Developer PowerShell:

```powershell
.\scripts\section-09.ps1 generate -Project starter
.\scripts\section-09.ps1 build -Project starter -Configuration Release
& .\exercises\09-particles-with-memory\starter\bin\starter.exe
```

Move with pointer and arrows. Pause for several seconds and resume; there should be no
catch-up burst. Try R and M, resize wide/narrow/tiny, and inspect all four edges.
A successful build still needs a quick run in the actual window.

## Break it on purpose

In `exercises/09-particles-with-memory/shared/particle_model.cpp`, temporarily change the expiry predicate from
`p.age >= p.lifetime` to `p.age > p.lifetime`. Run `tests/run-section-09-tests.sh`; predict the exact equality-edge
failure. Restore `>=` and rerun. If this was your only edit:

```sh
git restore -- exercises/09-particles-with-memory/shared/particle_model.cpp
```

That command discards every uncommitted change in that named file. Before moving on,
make sure the equality boundary makes sense.

## Your turn

Open the [history-trail emitter brief](../../../exercises/09-particles-with-memory/README.md). Own the design record, then replace starter ribbons with a
visual system unlike the starter or solution. Explain one force, trail-cap, spawn-cap,
boundary, and reduced-motion choice.

## Check your work

```sh
CXX=g++ tests/run-section-09-tests.sh
CXX=clang++ tests/run-section-09-tests.sh
```

Use the PowerShell test on Windows. Generate and compile starter and solution in Debug
and Release. Open both input routes and review pause, reset, reduced motion, resize, edges, keyboard
access, and your visual choices. Those visible details sit outside the number tests.

## Optional notes for future you

Explain the difference between a particle's age and lifetime, and between a drawn frame
and a fixed model step. Note one safe-removal rule and one aspect of the appearance you
chose. Save a capture with alt text.

## Make it yours

Emit only while input is held, map age to geometric fragmentation, use wrap rather than
bounce, or connect nearby current heads. Preserve explicit seed, finite guards,
radius-aware bounds, fixed-step/drop rule, hard container caps, safe removal, keyboard
access, pause/reset, and reduced motion.

## Quick visual check

- Pointer/drag and arrow keys move the same visible emitter.
- P pauses without a resume burst, R repeatably resets, and M hides trails.
- Nothing flashes; no state is communicated by audio or color alone.
- Current heads remain visible when trails are reduced.
- In the solution, verify young and old heads retain the same limited outline while
  interior triangle size visibly increases with normalized age, including in reduced
  mode.
- Text and palettes have suitable contrast; age also changes spacing, size, and
  interior/outline proportion.
- Resize and every edge keep full strokes visible; tiny viewports suppress simulation
  safely.
- Geometry, cadence, forces, trail treatment, and palette differ from both examples.
- Alt text names trajectory, memory order/length, boundary contact, shapes, and palette
  roles.
- Reused references and assets are credited.

## If you get stuck

When particles explode, vanish, or multiply like rabbits, inspect one particle through
one update. Check `dt`, velocity, lifetime, and the history size before
looking at the whole cloud. Keep the simulation limited: a beautiful sketch that quietly
allocates forever is just a glitter-covered leak.
