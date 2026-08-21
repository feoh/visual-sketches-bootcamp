# Exercise 10: switchable force instrument

## The short version

You will compare two kinds of motion: objects steering toward a target and objects
connected by springs. A force changes velocity. Steering compares the velocity you want
with the one you have; a spring pulls toward its resting length.

The saved examples use small numbers you can work through before running the full
sketch.

Build a limited chain or swarm that switches between seek and spring-chain behavior. The
framework-free model accumulates small behavior forces, caps the sum at integration,
applies semi-implicit Euler with exponential damping, caps speed, and clears force for
the next fixed step. openFrameworks only adapts input and renders easy to check state.

## What you choose

Edit `starter/src/design/force_design.cpp`: own count, radius, mass, force/speed caps, damping, arrival radius,
attraction/repulsion and softening, spring rest length/stiffness/damping, restitution,
and palette. Replace the starter's circles/triangles plus straight spokes/links with a
geometry that differs in structure, not merely color. Keep one non-color cue for
behavior mode.

Pointer/drag and arrow keys move the shared target/anchor. 1 selects seek, 2 selects
spring chain, and Space switches. P pauses, R resets, and M freezes the model into a
still reduced-motion view while current positions and controls remain visible. Resize
resets safely; a viewport smaller than one agent diameter suppresses simulation.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

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

Keep `makeForceDesign()` and the declarations in `shared/force_model.h`. Tests compare
the spring with saved expected values and check steering, arrival, damping, force and
speed limits, mode changes, boundaries, pause/reset behavior, and bad calculations. If
an update fails, the entire previous system must remain unchanged. Tests also make sure
the editable design function is valid. You still judge originality and appearance by
opening the app; tests do not compare pixels. The course supplies checked Linux, macOS,
and Windows build commands.

## Notes for future you

A few plain-language notes can make the sketch much easier to return to later:

- Which two small behaviors did you combine, and what can you see each one doing?
- Why does the spring pull when it is stretched and push when it is compressed?
- Which limits keep the motion calm when a frame takes too long?

Keep the existing safety rules in the code: zero distance has no direction, force and
speed stay limited, a failed update leaves the old state alone, and shape or link style
—not only color—shows the current mode.

## Check the result yourself

- Pointer and arrow routes move the same visible control point.
- 1, 2, Space, P, R, and M work; pause/reduced mode cannot create a resume burst.
- Reduced mode is still, removes moving links, and retains current positions.
- Shape/fill/link structure distinguishes modes without color; nothing flashes.
- Text and both palettes remain legible against the background.
- Resize, all edges, pause/resume, reset, and tiny viewports remain safe.
- Geometry, force values, spacing, link treatment, motion, and palette differ from both
  examples.
- Capture alt text names mode, target/anchor, force relationship, geometry, bounds, and
  palette roles.
- Credit any reused reference, code, or asset.
