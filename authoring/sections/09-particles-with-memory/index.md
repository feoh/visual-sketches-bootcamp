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

This section has one path: learn how a limited particle system updates many
records, practice tracing one repeatable object, then build one tested emitter.

1. [Lesson: understand particles and fixed steps](#lesson)
2. [Practice: trace, build, and observe](#practice)
3. [Exercise: create a tested history emitter](#exercise)

## Lesson

### Many small records make one system

![On a cream field, teal circles and coral squares follow two curved, limited oldest-to-newest trails from small earlier marks to large current heads; both trajectories stay inside the rounded viewport boundary, and shape plus size distinguish the palette roles without color alone.](media/particle-preview.svg "Objects carry short, limited memories through a fixed-step world.")

*Objects carry short, limited memories through a fixed-step world.*

A particle is a small record containing position, velocity, age, lifetime, and a
short trail. A particle system owns many records and repeats the same update for
each one. **Age** says how long an object has existed; **lifetime** says when it
must disappear.

Use the course's
[credited precedent notes](../../../docs/source-notes.md#visual-vocabulary)
to identify a particle or trail mechanism, not a final frame to reproduce.
Credit creators, then change shape, emission, force, history, palette, and
composition.

### Vectors own complete objects

[`std::vector`](https://en.cppreference.com/w/cpp/container/vector.html)
can store complete `Particle` objects rather than parallel arrays. A constructor
receives position, velocity, and lifetime, establishes `age == 0`, and stores
the initial history point. `Emitter` owns the vector and moves complete records
into it.

Growth and erase can invalidate references, pointers, and iterators, so do not
retain one across spawn or removal. Explicit `maximum_particles` and
`maximum_history` values bound memory and work. History stays oldest-to-newest;
a range erase removes all excess oldest points at once.

### One fixed update has an explicit order

The model uses this semi-implicit order:

```text
v = (v + acceleration * fixed_dt) * exp(-drag_rate * fixed_dt)
position = position + v * fixed_dt
age = age + fixed_dt
```

The new velocity moves position. Exponential drag is time-scaled:
`exp(-r * 0.5)` equals two factors of `exp(-r * 0.25)`, so damping does not
depend on render frequency. Configuration, time, state, and intermediate values
must be finite. An overflowing update leaves the particle unchanged instead of
spreading `NaN`.

A legal center lies in `[radius, width - radius]` and the matching y interval.
Crossing an edge clamps the center and reflects only outward velocity, scaled by
restitution. A viewport smaller than one diameter has no legal interval, so the
frame is rejected before simulation.

### Expiry and removal are separate

A particle expires at `age >= lifetime`, including equality. Removing while
incrementing an index can skip adjacent expired objects, while erasing inside a
range-for loop invalidates its iterator. Use
[`std::remove_if`](https://en.cppreference.com/w/cpp/algorithm/remove.html)
to move survivors forward, then erase the returned tail:

```cpp
particles.erase(std::remove_if(particles.begin(), particles.end(), expired),
                particles.end());
```

Tests remove adjacent and separated expired objects together and preserve
survivor order.

### Render time feeds a limited fixed-step accumulator

Render frames do not define simulation steps. This model uses a bounded version
of [Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/):

1. reject negative or non-finite frame time;
2. clamp accepted time to `maximum_frame_time` and record excess as dropped;
3. add accepted time to the accumulator;
4. simulate at most `maximum_catch_up_steps`; and
5. drop additional whole steps while retaining only a substep remainder.

The cap prevents an unlimited catch-up loop. `FrameResult` and the emitter total
record dropped time rather than pretending every elapsed second was simulated.

The spawn clock advances inside fixed steps, not draw calls. Each cadence makes
at most one particle because cadence is at least one fixed step. Capacity
suppresses extra spawns. An explicit random seed repeats the same launch
sequence, including seed zero; another seed must vary it.

### Pause, reset, and reduced motion keep promises

P toggles pause and clears the accumulator, so resume uses only newly supplied
time. R resets particles, clocks, dropped time, origin, and seed. M hides trails
while retaining current heads and controls. Pointer or drag and arrow keys move
the same origin through the openFrameworks
[event adapter](https://openframeworks.cc/documentation/events/ofEvents/).

Current heads need a non-color age cue when trails are hidden. The solution keeps
a full-radius triangular outline while an interior triangle grows from 35% to
80% of radius with normalized age. Tests inspect numbers and bounds; contrast,
input access, motion, and originality remain manual checks.

## Practice

Practice is guided and has no unit-test gate. Calculate two updates, inspect the
saved oracle, build the adapter, and observe the fixed-step controls.

### 1. Calculate one particle twice

Start at `x = 10` with `vx = 10`, zero acceleration, drag rate `1`, and fixed
step `0.25` second. For each step calculate:

```text
drag = exp(-1 * 0.25)
vx = vx * drag
x = x + vx * 0.25
```

Predict velocity and position after one and two steps. Compare one half-second
update with two quarter-second updates and explain which position differs under
semi-implicit integration. Decide whether a particle at exactly
`age == lifetime` remains alive.

### 2. Inspect the independent values

Linux or macOS:

```sh
cat exercises/09-particles-with-memory/fixtures/particle-oracle.txt
```

Windows Developer PowerShell:

```powershell
Get-Content .\exercises\09-particles-with-memory\fixtures\particle-oracle.txt
```

Confirm first velocity about `7.788008` and first position about `11.947002`.
Trace how the fixture's update order produces those values.

### 3. Build and explore the adapter

Set `OF_ROOT` to openFrameworks 0.12.1. Linux or macOS:

```sh
scripts/section-09.sh generate --project starter
scripts/section-09.sh build --project starter --configuration Release
exercises/09-particles-with-memory/starter/bin/starter
```

Windows Developer PowerShell:

```powershell
.\scripts\section-09.ps1 generate -Project starter
.\scripts\section-09.ps1 build -Project starter -Configuration Release
& .\exercises\09-particles-with-memory\starter\bin\starter.exe
```

Move the origin with pointer and arrows. Pause for several seconds and resume;
there should be no burst. Try R and M, resize wide, narrow, and smaller than one
diameter, and inspect all four edges.

### 4. Compare history with reduced motion

Follow one current head from spawn to expiry. Note the oldest-to-newest trail
order, then press M and confirm that the current position and age cue remain
visible without history. Reset and verify that the same seed produces the same
early motion. This is an observation task; Exercise tests the numeric contract.

## Exercise

### Problem: build a limited history-trail emitter

Create a `std::vector<Particle>` system with explicit cadence, seed, fixed-step
and dropped-time rules, safe expiry removal, radius-aware boundaries, and capped
oldest-to-newest history. Choose lifetime, drag, acceleration, restitution,
launch speed, palette, and a visual language unlike both examples. Keep pointer
and keyboard input, pause, reset, and reduced motion usable.

Open the
[Exercise 09 brief, starter, tests, and solution](../../../exercises/09-particles-with-memory/README.md),
then edit `starter/src/design/particle_design.cpp` and
`starter/src/ofApp.cpp`. Keep `makeParticleDesign()` and the declarations in
`shared/particle_model.h` unchanged.

### Run the unit tests

Linux or macOS:

```sh
CXX=g++ tests/run-section-09-tests.sh
CXX=clang++ tests/run-section-09-tests.sh
```

Windows Developer PowerShell:

```powershell
.\tests\run-section-09-tests.ps1
```

Tests parse the independent integration oracle and check cadence, spawn count,
frame partitions, capped catch-up and dropped time, pause spikes, expiry at
equality, simultaneous safe removal, exponential drag, all four edges, history
order and cap, replay, seed variation, finite extremes, tiny bounds, and your
design choices. They use neither pixels nor a real clock.

For one test-driven repair, temporarily change the expiry predicate in
`shared/particle_model.cpp` from `p.age >= p.lifetime` to `p.age > p.lifetime`.
Predict the equality failure, run the suite, restore `>=`, and require a green
run. Then generate and compile starter and solution in Debug and Release and
inspect both input routes.

### Quick visual check

- Pointer and arrow keys move the same emitter; P, R, and M work.
- Pause resumes without a burst, reset repeats, and reduced mode hides trails.
- Nothing flashes and no state depends on audio or color alone.
- Current heads and an age cue remain visible without history.
- Palettes and text have suitable contrast.
- Resize, all edges, and tiny viewports stay safe with full strokes visible.
- Geometry, cadence, forces, trails, and palette differ from both examples.
- Alt text names trajectory, memory order, boundary contact, shapes, and palette.
- Reused references and assets are credited.

### If you get stuck

Inspect one particle through one fixed update. Check `dt`, velocity, age,
lifetime, and history size before looking at the whole cloud. Keep every count
limited: a beautiful sketch that allocates forever is a glitter-covered leak.
