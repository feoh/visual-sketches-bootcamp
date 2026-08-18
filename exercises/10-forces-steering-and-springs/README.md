# Exercise 10: switchable force instrument

Build a bounded chain or swarm that switches between seek and spring-chain
behavior. The framework-free model accumulates small behavior forces, caps the
sum at integration, applies semi-implicit Euler with exponential damping, caps
speed, and clears force for the next fixed step. openFrameworks only adapts
input and renders inspectable state.

## Learner-owned choices

Edit `starter/src/design/force_design.cpp`: own count, radius, mass, force/speed
caps, damping, arrival radius, attraction/repulsion and softening, spring rest
length/stiffness/damping, restitution, and palette. Replace the starter's
circles/triangles plus straight spokes/links with a geometry that differs in
structure, not merely color. Keep one non-color cue for behavior mode.

Pointer/drag and arrow keys move the shared target/anchor. 1 selects seek, 2
selects spring chain, and Space switches. P pauses, R resets, and M freezes the
model into a still reduced-motion view while current positions and controls
remain visible. Resize resets safely; a viewport smaller than one agent
diameter suppresses simulation.

## Public contract

Run the pure tests on Linux or macOS:

```sh
CXX=g++ tests/run-section-10-tests.sh
CXX=clang++ tests/run-section-10-tests.sh
```

Generate, build, and launch the starter on Linux:

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
.\tests\run-section-10-tests.ps1
.\scripts\section-10.ps1 generate -Project starter
.\scripts\section-10.ps1 build -Project starter -Configuration Release
& .\exercises\10-forces-steering-and-springs\starter\bin\starter.exe
```

Keep `makeForceDesign()` and `shared/force_model.h` declarations. Tests parse an
independent spring oracle and inspect seek/arrival, 2D equilibrium/symmetry,
damping, zero-distance softening, extreme finite arithmetic, accumulation/reset,
independent force/speed caps, explicit mode transitions, frame partitions and
step accounting, four edges/corners/exact bounds, control inset, pause/drop
policy, complete-system transactionality, and design-hook validity/customization
readiness. Originality remains a manual review. No pixel or real-time timing
test is used. Native Linux/macOS/Windows Debug/Release statuses prove
compilation only, not launch.

## Required behavior and explanation

- Accumulate at least two small behavior functions before integration.
- Define the no-direction zero-distance policy and positive force softening.
- Explain conventional `F = -k(x - L)` and why force on the first endpoint is `axis_first_to_second * k * (distance - L)`.
- Explain how the spring-axis dot product makes damping oppose approach/separation.
- State mass, maximum force, maximum speed, fixed step, and dropped-time policy.
- Show seek and spring modes through shape/fill/link structure, not color alone.
- Keep state finite, agent count/work bounded, and current marks radius-aware.

## Manual accessibility and originality review

- Pointer and arrow routes move the same visible control point.
- 1, 2, Space, P, R, and M work; pause/reduced mode cannot create a resume burst.
- Reduced mode is still, removes moving links, and retains current positions.
- Shape/fill/link structure distinguishes modes without color; nothing flashes.
- Text and both palettes remain legible against the background.
- Resize, all edges, pause/resume, reset, and tiny viewports remain safe.
- Geometry, force values, spacing, link treatment, motion, and palette differ
  from both examples.
- Capture alt text names mode, target/anchor, force relationship, geometry,
  bounds, and palette roles.
- Credit any reused reference, code, or asset.
