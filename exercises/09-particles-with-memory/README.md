# Exercise 09: history-trail emitter

## The short version

You will manage many small particle records. Each one has position, velocity, age,
lifetime, and a short history. The system repeats one update for every particle and
removes expired ones.

Counts and history lengths are capped so the sketch cannot grow forever. Fixed small
time steps keep motion steady when display frames vary.

Build a small system from a `std::vector<Particle>`. Each object owns position, velocity, age,
lifetime, and a limited oldest-to-newest history. A fixed-step emitter owns cadence,
seed, accumulator, catch-up rule, and safe removal. openFrameworks only adapts input and
draws the easy to check state.

## What you choose

Edit `starter/src/design/particle_design.cpp`: own cadence, lifetime, drag, acceleration, radius, restitution,
history cap, launch speed, and palette. Then replace the starter connected circular
ribbons with geometry that differs in structure, not only color. The solution uses
sparse square memories, triangular heads, a dark field, and distinct motion values. Its
current head keeps a full-radius outline while a limited interior triangle grows from
35% to 80% of the radius with normalized age, so age remains visible without color when
history is hidden.

Pointer/drag and arrow keys move the emitter. P pauses/resumes, R resets the same
explicit seed, and M toggles reduced motion by hiding history while keeping solid
current positions. Resize resets safely; a viewport smaller than one particle diameter
suppresses simulation.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

```sh
CXX=g++ tests/run-section-09-tests.sh
CXX=clang++ tests/run-section-09-tests.sh
scripts/section-09.sh generate --project starter
scripts/section-09.sh build --project starter --configuration Release
```

Use `.ps1` wrappers in Windows Developer PowerShell. Keep `makeParticleDesign()` and
`shared/particle_model.h` declarations. Tests parse an independent drag/integration oracle and
inspect cadence/count, frame partition, capped catch-up/drop, pause spikes, expiry
equality, simultaneous removal, exponential drag, four edges, history order/cap,
repeatable replay, seed variation, finite extremes, tiny bounds, and your design choices.
No pixel or real-clock test is used. The course supplies checked Linux, macOS, and
Windows build commands; open the app to check the finished motion.

## Check the result yourself

- Pointer and arrow routes move the same visible emitter; P, R, and M work.
- Reduced mode removes trails; nothing flashes and there is no audio-only cue.
- Shape and spacing communicate memory without color alone; in the solution, compare
  young and old interior-triangle sizes with trails hidden.
- Text and both palettes remain legible against the background.
- Resize, all edges, pause/resume, reset, and tiny viewports remain safe.
- Geometry, cadence, trail treatment, forces, and palette differ from both examples.
- Capture alt text names path, trail order/length, boundary contact, shapes, and palette
  roles.
- Credit any reused reference or asset.
