---
title: Forces, steering, and springs
slug: 10-forces-steering-and-springs
weight: 120
draft: false
course_kind: instructional
objectives:
  - Accumulate small behavior forces, cap their sum, integrate once, and clear it for the next fixed step
  - Use a scoped behavior mode to switch a limited system between seek and spring-chain composition
  - Calculate softened attraction or repulsion, arrival steering, Hooke-like restoring force, and axial damping safely
  - Explain dot product, force and speed limits, semi-implicit order, and fixed-step state transitions
  - Design an accessible chain or swarm whose motion tests can check without screenshots
prerequisites:
  - Completion of section 09 or equivalent vectors, objects, fixed-step simulation, finite-value guards, and event adapters
source_records: sources.yaml
asset_records: assets.yaml
---

# Forces, steering, and springs

This section has one path: learn how small forces combine, practice with one
seeking swarm and spring pair, then solve one tested force-system problem.

1. [Lesson: understand force composition](#lesson)
2. [Practice: calculate, run, and repair](#practice)
3. [Exercise: design a tested force instrument](#exercise)

## Lesson

### One system, two behaviors

![A teal circle swarm converges on a ring target above a coral chain of diamond-shaped agents hanging from a square anchor; dark arrows show paired spring forces in opposite directions, and shape, links, and vertical placement distinguish the two behavior modes without color alone.](media/force-preview.svg "The same objects can seek a target or connect as a springy chain.")

*The same limited group becomes a seeking swarm or an elastic chain by combining small forces.*

In this model, a **force** is a request to change velocity. Seeking compares the
velocity an agent wants with the velocity it has. A spring measures how far two
endpoints are from their resting distance, while damping removes motion along
the spring so it can settle.

A scoped [`enum class`](https://en.cppreference.com/w/cpp/language/enum.html)
names the `seek` and `spring_chain` modes. The mode selects which small force
functions contribute to one accumulator; it does not create a second simulation
loop.

### Accumulate, limit, integrate, clear

Every fixed step follows one order:

```text
clear force
compose behavior forces into force
force = limit(force, maximum_force)
velocity = limit((velocity + force / mass * dt) * exp(-damping_rate * dt), maximum_speed)
position = position + velocity * dt
clear force
```

The new velocity moves position, an order called semi-implicit Euler. All agents
receive their contributions before any agent integrates, so function order does
not secretly give one object an early update. Maximum force limits how quickly
motion changes; maximum speed separately limits the resulting movement.

### Seek compares desired and current velocity

Let `d = target - position`. Away from the target, desired velocity is the unit
direction of `d` multiplied by maximum speed. Inside the arrival radius, speed
shrinks by `distance / arrival_radius`:

```text
seek force = (desired_velocity - velocity) * mass / fixed_dt
```

A radial attraction or repulsion uses softened distance:

```text
force = unit(source - position) * strength / (distance squared + softening squared)
```

Positive strength attracts and negative strength repels. At exact coincidence
there is no direction to normalize, so the honest answer is `{0, 0}`. Positive
softening prevents huge near-zero values. Pair repulsion applies equal and
opposite forces before caps and boundaries.

### Springs restore distance and damp along their axis

A [Hooke-like force](https://openstax.org/books/physics/pages/16-1-hookes-law-stress-and-strain-revisited)
is proportional to extension from rest length. This model asks for the force on
the first endpoint and points its axis from first to second:

```text
force_on_first = axis * stiffness * (distance - rest_length)
```

Put endpoints at x = 0 and x = 20, with rest length 10 and stiffness 2. The
first receives `+20`; the second receives `-20`. At distance 10 the force is
zero. Compression makes the scalar negative and pushes the pair apart.

Damping uses the dot product of relative velocity and the spring axis. If the
second endpoint approaches the first at 10 units per second and axial damping is
1, that dot product is `-10`, so the first endpoint's force becomes
`20 + 1 × -10 = 10`. Sideways motion has a zero axial dot product.

### Fixed-step transitions and adapter controls

Keys 1 and 2 choose a mode; Space toggles it. A mode change clears partial
accumulator time and old forces so a half-step from one mode cannot run under
the other. The bounded accumulator follows the
[fixed-timestep technique](https://gafferongames.com/post/fix_your_timestep/)
and reports dropped catch-up work. The model rejects unusable calculations,
limits the system to 64 agents, includes each radius in boundary checks, and
reverses only velocity pointing out of the window.

Pointer, drag, and arrow events move the same target or anchor through the
[openFrameworks event adapter](https://openframeworks.cc/documentation/events/ofEvents/).
P pauses, R resets, and M freezes motion while leaving current shapes visible.
The renderer clamps visible controls including stroke width and draws geometry
inside each model radius. Tests inspect forces and state, not GPU pixels.

## Practice

Practice is guided and has no unit-test gate. Work one spring by hand, trace one
seek update, run both modes, and repair one visible stale-force mistake.

### 1. Replay the spring arithmetic

Read the independent values:

```sh
cat exercises/10-forces-steering-and-springs/fixtures/spring-oracle.txt
```

On Windows Developer PowerShell:

```powershell
Get-Content .\exercises\10-forces-steering-and-springs\fixtures\spring-oracle.txt
```

Confirm the equilibrium, stretched, and stretched-plus-damping results are 0,
20, and 10. Then reverse the endpoint order: the magnitudes stay the same while
the axis and paired signs reverse.

### 2. Trace one seek update

Choose an agent at `(100, 100)`, a target at `(130, 100)`, zero current
velocity, maximum speed 60, mass 1, and fixed step `1/60`. The desired velocity
is `(60, 0)`. Before the configured force cap, the seek request is
`(60 - 0) × 1 / (1/60) = (3600, 0)`. Locate the cap and semi-implicit update in
`shared/force_model.cpp`, then predict why changing maximum force affects
responsiveness without changing maximum speed.

### 3. Run the two modes

Set `OF_ROOT` to openFrameworks 0.12.1. Linux or macOS:

```sh
scripts/section-10.sh generate --project starter
scripts/section-10.sh build --project starter --configuration Release
```

Launch `exercises/10-forces-steering-and-springs/starter/bin/starter` on Linux,
or open the generated `.app` on macOS. On Windows Developer PowerShell:

```powershell
.\scripts\section-10.ps1 generate -Project starter
.\scripts\section-10.ps1 build -Project starter -Configuration Release
& .\exercises\10-forces-steering-and-springs\starter\bin\starter.exe
```

Press 1 and move the ring with pointer and arrows. Watch arrival slow near the
target. Press 2 and compare stretched, compressed, and rest-length links. Try
Space, P, R, M, every edge, and a viewport narrower than one agent diameter.

### 4. Repair stale accumulated force

In `exercises/10-forces-steering-and-springs/shared/force_model.cpp`, temporarily
remove `clearForces(next);` near the start of `composeForces`. Rebuild and run
the app. Old requests now leak into later steps, so motion keeps strengthening
instead of responding cleanly to the current mode. Restore the call and confirm
the behavior settles again.

If that was your only intended edit:

```sh
git restore -- exercises/10-forces-steering-and-springs/shared/force_model.cpp
```

That command discards every uncommitted change in the named file.

## Exercise

### Problem: create a switchable force instrument

Create a chain or swarm with seek and spring-chain modes whose geometry differs
from both supplied examples. Choose the force caps, equilibrium, damping,
boundaries, mode cues, and reduced-motion appearance. Preserve positive
softening, paired spring forces, finite guards, the fixed-step/drop rule, the
hard agent cap, keyboard mode access, pause, and reset.

Use the
[switchable force instrument exercise brief](../../../exercises/10-forces-steering-and-springs/README.md)
as the authoritative source for editable files, controls, constraints, fixtures,
and the explained solution.

### Run the unit tests

Linux or macOS:

```sh
CXX=g++ tests/run-section-10-tests.sh
CXX=clang++ tests/run-section-10-tests.sh
```

Windows Developer PowerShell:

```powershell
.\tests\run-section-10-tests.ps1
```

The deterministic suite checks the independent spring oracle, exact equilibrium,
stretched and damped pairs, zero-distance safety, softened attraction and
repulsion, seek arrival, force and speed limits, semi-implicit order, symmetric
pair forces, fixed-step partitions, dropped time, mode transitions, reset,
bounds, clamp helpers, finite-state rejection, and learner design limits. Build
starter and solution in Debug and Release, then open both apps; automated tests
cannot judge contrast, mode legibility, or originality.

### Quick visual check

- Pointer, drag, and arrows move the same visible target or anchor.
- 1, 2, and Space expose both modes without relying only on color.
- P pauses without a resume burst, R resets, and M gives a useful still view.
- Seek and spring remain distinct through shape, fill, links, or layout.
- Resize and every edge keep full marks visible; tiny viewports reject safely.
- Nothing flashes, text and palettes have suitable contrast, and no state is
  audio-only.
- Count, geometry, spacing, links, force response, and palette differ from both
  examples.
- Alt text names the mode, control point, force relationship, geometry, and
  palette roles; reused work remains credited.

### If you get stuck

If a force sends everything to infinity, inspect the zero-distance rule and
force cap first. If a spring jitters, reduce the problem to two endpoints and
check axis, extension, dot product, and update order. Run the smallest failing
case before changing the whole swarm; physics bugs are friendlier with only two
witnesses.
