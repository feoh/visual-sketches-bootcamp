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

## See what you're making

![A teal circle swarm converges on a ring target above a coral chain of diamond-shaped agents hanging from a square anchor; dark arrows show paired spring forces in opposite directions, and shape, links, and vertical placement distinguish the two behavior modes without color alone.](media/force-preview.svg "The same objects can seek a target or connect as a springy chain.")

*The same limited group becomes a seeking swarm or an elastic chain by combining small forces.*

The still uses circle/diamond shape, target/links, and layout as well as color.

## Take a guess

Two agents lie on the x-axis at 0 and 20. Their spring rest length is 10 and stiffness
is 2. Predict the x force on the first agent. Then the second agent moves toward the
first at 10 units per second and axial damping is 1. Predict the new force. What should
a radial behavior return when agent and source have exactly the same position and there
is no direction to normalize?

## Let's unpack it

### Before the force vocabulary

In this sketch, a **force** is simply a request to change velocity. Steering asks, “What
velocity would take me toward the target?” and compares that with the velocity the
object has now.

A spring asks a different question: “How far am I from my resting length?” The farther
it is stretched or squeezed, the stronger the pull back. Damping removes some motion
along the spring so it settles instead of wobbling forever.

The formulas give precise versions of those questions. Keep the picture in mind first:
desired direction, current movement, stretch, and a gentle brake.

### One behavior mode chooses a composition

A scoped [`enum class`](https://en.cppreference.com/w/cpp/language/enum.html) names `seek` and `spring_chain` without magic integers.
The enum and the small behavior functions form one mechanism: mode selects which
functions contribute to the accumulator. It does not create two simulation loops.

Each fixed step follows one order:

```text
clear force
compose behavior forces into force
force = limit(force, maximum_force)
velocity = limit((velocity + force / mass * dt) * exp(-damping_rate * dt), maximum_speed)
position = position + velocity * dt
clear force
```

This is semi-implicit Euler because the new velocity moves position. Every agent
receives all contributions before integration, so function order does not secretly
update one agent early.

### Seek is desired velocity minus current velocity

Let `d = target - position`. Away from the target, desired velocity is the unit direction of
`d` times maximum speed. Inside the arrival radius, its speed is multiplied
by `distance / arrival_radius`. Steering force points from current velocity toward that desired
velocity:

```text
seek = (desired_velocity - velocity) * mass / fixed_dt
```

The force cap prevents one large error from becoming an impulse. The speed cap is
separate: maximum force limits how quickly motion changes; maximum speed limits the
resulting motion.

### Decide what happens exactly at the target center

A direction-only inverse-square expression is singular at zero. This course uses
softened distance:

```text
force = unit(source - position) * strength / (distance squared + softening squared)
```

Positive strength attracts and negative strength repels. At exact coincidence, there is
no honest direction, so the function returns `{0, 0}`. Positive softening prevents
enormous near-zero values. Pair repulsion applies equal and opposite results to two
agents, preserving symmetry before caps and boundaries.

### A spring measures displacement from equilibrium

[Hooke-like force](https://openstax.org/books/physics/pages/16-1-hookes-law-stress-and-strain-revisited) is proportional to extension from rest length. In conventional
one-dimensional signed coordinates, `F = -k(x - L)`: the restoring force opposes
displacement from equilibrium. This model instead asks for the force **on the first
endpoint** and defines its axis from first to second:

```text
force_on_first = axis_first_to_second * stiffness * (distance - rest_length)
```

At distance 20, rest length 10, and stiffness 2, the first endpoint receives
`+20` along that axis because a stretched second endpoint pulls it forward. The
second receives `-20`, which is the conventional opposing sign. At exactly 10
the force is zero. Compression makes the first scalar negative, pushing the endpoints
apart.

Damping uses a dot product. Here `relative_velocity` means the second endpoint's
velocity minus the first endpoint's velocity. The spring axis points from first to
second. A negative dot means the endpoints are approaching, zero means the motion is
sideways, and a positive dot means they are separating. In the Predict case,
`20 + 1 * -10 = 10`: damping opposes closing motion. The paired endpoint receives the
negative force. Global exponential damping remains useful for motion outside the spring
axis.

### Modes change only at a fixed-step boundary

1 and 2 set an explicit `BehaviorMode`; Space toggles it. A transition clears partial
accumulator time and old forces, so no half-step created under one mode runs under
another. P and M also clear the accumulator. The limited accumulator uses the same
capped catch-up and visible dropped-time rule as [fixed timestep simulation](https://gafferongames.com/post/fix_your_timestep/).

The system has at most 64 agents. It performs force calculations with extra numeric
room before converting them to the smaller values used by the model. Your much smaller
`maximum_force` is still the artistic limit. If any calculation produces `NaN`,
infinity, or another unusable number, the whole update is discarded and the previous
state remains unchanged. Boundaries include each agent's radius and reverse only
velocity that points out of the window.

### Input and pixels are adapter concerns

Pointer/drag and arrow keys move the same target and anchor through the openFrameworks
[event adapter](https://openframeworks.cc/documentation/events/ofEvents/). 1, 2, and Space expose mode without requiring a pointer. P pauses, R
resets, and M freezes the model and removes moving links while current shapes stay
visible. The adapter clamps the ring or square by the larger of the model radius and its
rendered half-extent including stroke; if that control cannot fit, it is not drawn.
Agent geometry is drawn inside the model radius, including rotated or stroked marks.
Pure tests check the clamp helper and model bounds without pretending to inspect pixels.
Tests inspect forces, transitions, bounds, and state. They do not compare GPU pixels or
open the finished app.

## Make it run: compare three small experiments

### 1. Replay the hand-calculated spring

From the repository root on Linux or macOS:

```sh
cat exercises/10-forces-steering-and-springs/fixtures/spring-oracle.txt
CXX=g++ tests/run-section-10-tests.sh
```

Confirm the equilibrium, stretched, and stretched-plus-damping results are 0, 20, and
10. On Windows Developer PowerShell:

```powershell
Get-Content .\exercises\10-forces-steering-and-springs\fixtures\spring-oracle.txt
.\tests\run-section-10-tests.ps1
```

### 2. Run the seeking swarm

Set `OF_ROOT` to openFrameworks 0.12.1. On Linux:

```sh
scripts/section-10.sh generate --project starter
scripts/section-10.sh build --project starter --configuration Release
exercises/10-forces-steering-and-springs/starter/bin/starter
```

On macOS:

```sh
scripts/section-10.sh generate --project starter
scripts/section-10.sh build --project starter --configuration Release
open exercises/10-forces-steering-and-springs/starter/bin/starter.app
```

On Windows Developer PowerShell:

```powershell
.\scripts\section-10.ps1 generate -Project starter
.\scripts\section-10.ps1 build -Project starter -Configuration Release
& .\exercises\10-forces-steering-and-springs\starter\bin\starter.exe
```

Press 1 and move the ring with pointer and arrows. Watch arrival slow near the ring and
pair repulsion keep overlapping agents from choosing arbitrary escape directions.

### 3. Run the elastic chain

Press 2, move the square anchor, and compare stretched, compressed, and roughly
rest-length links. Use Space repeatedly only after predicting the shape and link change.
Pause for several seconds and resume; there should be no catch-up burst. Try R, M, every
edge, and a viewport narrower than one agent diameter. After the build succeeds, open the app to check the actual behavior.

## Break it on purpose

In `exercises/10-forces-steering-and-springs/shared/force_model.cpp`, temporarily remove `clearForces(next);` near the
start of `composeForces`. Run `tests/run-section-10-tests.sh`; predict which accumulation or fixed-step
property fails as old force leaks into the next step. Restore the call and rerun. If
this was your only edit:

```sh
git restore -- exercises/10-forces-steering-and-springs/shared/force_model.cpp
```

That command discards every uncommitted change in that named file. Before moving on,
make sure you can connect the failure to stale force state.

## Your turn

Open the [switchable force instrument brief](../../../exercises/10-forces-steering-and-springs/README.md). Own the design record, then replace starter geometry with a
chain or swarm unlike the starter or solution. Explain one softening, cap, equilibrium,
damping, boundary, mode-cue, and reduced-motion choice.

## Check your work

```sh
CXX=g++ tests/run-section-10-tests.sh
CXX=clang++ tests/run-section-10-tests.sh
```

Use the PowerShell test on Windows. Generate and compile starter and solution in Debug
and Release. Manually launch both input routes and review seek, spring, mode changes,
pause, reset, reduced motion, resize, edges, accessibility, and originality. Automated
tests cover only the checks named above.

## Optional notes for future you

Explain the difference between force and velocity, then use your spring example to show
what rest length and damping do. Note one zero-distance rule and one appearance you
customized. Save a capture with alt text.

## Make it yours

Use a triangular spring network, let pointer distance tune equilibrium, replace seek
with arrive-and-orbit, or map spring extension to mark orientation. Preserve finite
guards, positive softening, force/speed caps, paired spring forces, fixed-step/drop
rule, hard agent cap, keyboard mode access, pause/reset, and a still reduced-motion
view.

## Quick visual check

- Pointer/drag and arrows move the same visible control point.
- 1, 2, and Space make both modes discoverable without color.
- P pauses without a resume burst, R resets, and M produces a still view.
- Nothing flashes; no state is communicated by audio or color alone.
- Seek and spring remain distinguishable by shape, fill, links, or layout.
- Text and palettes have suitable contrast.
- Resize and every edge keep full marks visible; tiny viewports reject safely.
- Count, geometry, spacing, links, force response, and palette differ from both
  examples.
- Alt text names mode, target/anchor, force relation, geometry, bounds, and palette
  roles.
- Reused references, code, and assets are credited.

## If you get stuck

If a force sends everything to infinity, check the zero-distance case and the force cap
first. If a spring jitters, inspect the update order and damping. Run one
hand-calculated pair of points before unleashing the whole swarm; physics bugs are much
less intimidating when they only have two witnesses.
