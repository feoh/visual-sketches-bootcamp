# Exercise 09: history-trail emitter

Build a small system from a `std::vector<Particle>`. Each object owns position,
velocity, age, lifetime, and a bounded oldest-to-newest history. A fixed-step
emitter owns cadence, seed, accumulator, catch-up policy, and safe removal.
openFrameworks only adapts input and draws the inspectable state.

## Learner-owned choices

Edit `starter/src/design/particle_design.cpp`: own cadence, lifetime, drag,
acceleration, radius, restitution, history cap, launch speed, and palette. Then
replace the starter connected circular ribbons with geometry that differs in
structure, not only color. The solution uses sparse square memories, triangular
heads, a dark field, and distinct motion values. Its current head keeps a
full-radius outline while a bounded interior triangle grows from 35% to 80% of
the radius with normalized age, so age remains visible without color when
history is hidden.

Pointer/drag and arrow keys move the emitter. P pauses/resumes, R resets the
same explicit seed, and M toggles reduced motion by hiding history while keeping
solid current positions. Resize resets safely; a viewport smaller than one
particle diameter suppresses simulation.

## Public contract

```sh
CXX=g++ tests/run-section-09-tests.sh
CXX=clang++ tests/run-section-09-tests.sh
scripts/section-09.sh generate --project starter
scripts/section-09.sh build --project starter --configuration Release
```

Use `.ps1` wrappers in Windows Developer PowerShell. Keep
`makeParticleDesign()` and `shared/particle_model.h` declarations. Tests parse
an independent drag/integration oracle and inspect cadence/count, frame
partition, capped catch-up/drop, pause spikes, expiry equality, simultaneous
removal, exponential drag, four edges, history order/cap, deterministic replay,
seed variation, finite extremes, tiny bounds, and learner ownership. No pixel
or timing-flaky test is used. Native Linux/macOS/Windows Debug/Release statuses
prove compilation only, not launch.

## Manual accessibility and originality review

- Pointer and arrow routes move the same visible emitter; P, R, and M work.
- Reduced mode removes trails; nothing flashes and there is no audio-only cue.
- Shape and spacing communicate memory without color alone; in the solution, compare young and old interior-triangle sizes with trails hidden.
- Text and both palettes remain legible against the background.
- Resize, all edges, pause/resume, reset, and tiny viewports remain safe.
- Geometry, cadence, trail treatment, forces, and palette differ from both examples.
- Capture alt text names path, trail order/length, boundary contact, shapes, and palette roles.
- Credit any reused reference or asset.
