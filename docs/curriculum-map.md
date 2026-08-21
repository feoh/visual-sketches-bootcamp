---
title: Curriculum map
weight: 10
draft: false
---

# Curriculum map

## Audience and outcome

This is for a curious programmer who wants to make things move. If you can edit
text, run a command, and handle basic algebra—substitute values into a formula
and solve something like `2x + 3 = 11`—you have enough math to start. Python experience is welcome but
not required; rusty C++ is fine too. Trigonometry is **not** a prerequisite.
When circles need sine, cosine, or radians, we stop and teach those ideas from
scratch with pictures and tiny examples.

By the end, you should be able to:

- read and modify a typical openFrameworks `ofApp`;
- keep a sketch's simulation and geometry testable without a window;
- use vectors, interpolation, circle helpers, transforms, randomness, noise, and
  simple forces intentionally;
- build gesture lines, repeated geometric systems, particles, flow fields,
  trails, and media-derived geometry;
- diagnose ordinary compiler, linker, runtime, and frame-rate problems;
- create and document an original, visually compelling sketch; and
- explain how its code, math, and visual behavior connect.

The course fits a **12–18 week suggested rhythm including synthesis**, but you
can take longer, skip an extension, or spend an entire evening making one
circle wiggle. The calendar is a map, not a boss fight.

## Pick a pace (or make up your own)

These are optional ways to organize the material. The route names refer to
weeks, not lesson counts: each one contains 19 ordered bundles including the
first-C++ test interlude. The checked route file simply keeps the published pages in the intended order.

| Route | Provisional schedule | Scope and consequence |
|---|---|---|
| Complete, about 18 weeks | 1: 00; 2: 01; 3: 02 + test interlude; 4–13: one of 03–12 each week; 14: 13; 15: 14; 16: 15; 17: section 16 studies; 18: begin section 17, with as much extra capstone time as you want | Includes every lesson. Take extra time for the capstone whenever you want; unfinished sketches are allowed to remain unfinished for a while. |
| Core, about 12 weeks | 1: 00; 2: 01; 3: 02 + test interlude; 4: 03–04; 5: 05–06; 6: 07–08; 7: 09–10; 8: 11; 9: 12; 10: section 16; 11–12: section 17 | Omits extensions 13–15. Pair sections only when that still feels fun; otherwise stretch the route. |
| Accelerated, 8+2 weeks | 1: 00–01; 2: 02 + test interlude; 3: 03–04; 4: 05–06; 5: 07–08; 6: 09–10; 7: 11; 8: 12; synthesis weeks 1–2: sections 16–17 | Omits 13–15. This is a fast tour, not a race; extend it rather than skipping the parts that make the sketch understandable. |

Sections 00–12 are the core prerequisite chain. Sections 13 and 14 are
extensions; section 15 is an elective. Section 16 requires the mechanisms from
06–12 but does not require section 13: “temporal memory” may use the bounded
histories introduced in sections 09 and 12. Section 17 requires section 16 or
equivalent synthesis work. The only delivered section 15 lane is recorded
microphone amplitude with no-device fallback.

## The rhythm of a lesson

Sections 00–15 follow a friendly repeatable rhythm. We may combine or skip steps
when that keeps the creative momentum moving. The larger projects in sections
16–17 use a looser path.

1. **See it:** a short visual goal and an animated or still reference.
2. **Take a guess:** predict what a tiny piece of code will draw or how it will move.
3. **Unpack it:** one math idea and at most one substantial C++ mechanism, explained before the jargon arrives.
4. **Make it run:** one to three runnable examples or one complete progressive program.
5. **Break it on purpose:** repair one planted bug or misleading implementation.
6. **Your turn:** a constrained but open-ended C++ sketch.
7. **Check your work:** supplied tests for deterministic behavior and edge cases.
8. **Notes for later:** optionally save a capture or short explanation connecting code,
   math, and image.
9. **Make it yours:** an optional variation that reuses earlier ideas.

The guidance gradually opens up: complete example → small gap → constrained remix →
open brief.

## Part 0 — Get something on screen

### 00. Cross-platform setup and the first frame

**Visual goal:** open a window, clear it with a chosen color, and draw one shape.

**C++:** source/header files, compiling versus running, function calls, reading the first useful compiler diagnostic.

**Math:** screen coordinates, origin, width, height, and pixels.

**openFrameworks:** Project Generator, `main.cpp`, `ofApp::setup`, `ofApp::draw`, color and primitive drawing.

**Exercise:** create a static “visual signature” from exactly five primitives and three colors.

**Tests:** platform setup probe; project builds; pure helper returns the expected primitive count and in-bounds coordinates.

### 01. A mark that moves

**Visual goal:** animate a shape predictably and make it react to mouse input.

**C++:** variables, numeric types, assignment, conditionals, `setup`/`update`/`draw`, event callbacks.

**Math:** position, rate, elapsed time, seconds versus milliseconds, and the difference between frame count and time.

**Time policy:** sample frame time at the application boundary, convert it to seconds, clamp long pauses, and pass explicit `dt` into model updates. Show the difference between variable-step animation and a fixed-step accumulator; early sketches may use the simpler stated policy, while particle systems revisit substeps.

**Exercise:** implement a wraparound traveler whose speed and color respond to pointer position.

**Tests:** fixed-`dt` position updates, sensible equivalent frame-time partitions, pause clamping, wraparound boundaries, and resize behavior.

### 02. The Python-to-C++ survival kit

**Visual goal:** generate a row of related moving forms without duplicating code.

**Main C++ idea:** break repeated drawing into functions that work with a `std::vector` of values. You will also encounter parameters and return values, `const`, references in APIs, range loops, and local variable lifetime. The course avoids `new` and `delete`; you do not need to master every supporting term in this lesson.

**Math:** indexed ranges and normalized progress from 0 to 1.

**Exercise:** produce a family of animated marks from a vector of parameters.

**Tests:** collection size, endpoint values, empty/single-item boundary coverage, and deterministic output. Sanitizers or checked/debug iterators are enabled where the platform supports them; tests do not claim to prove the absence of every possible out-of-bounds access.

### Interlude. Reading and writing the first C++ test

**Goal:** make the exercise harness part of the learner’s vocabulary before tests become an unexplained ritual.

Run one `ofxUnitTests`/`ofAppNoWindow` program, repair one failed check, write one known-answer test, and use the course helper for comparing calculated decimal values. Learn which file holds the test, how inputs/call/answer form one check, and why a number-only test cannot judge the finished picture. Later lessons introduce more kinds of tests when they become useful.

## Part 1 — Make motion mean something

### 03. Map, clamp, and lerp

**Visual goal:** make composition respond smoothly to window size and input.

**C++:** small reusable functions and approximate floating-point comparisons.

**Math:** ranges, normalization, linear interpolation, clamping, aspect ratio, and simple easing.

**Exercise:** build a responsive poster whose spacing, scale, and palette transition smoothly.

**Tests:** endpoints, midpoint, out-of-range clamping, monotonicity, and resize invariants.

### 04. Vectors: direction and distance

**Visual goal:** steer one form toward another and create orbit/bounce studies.

**C++:** `glm::vec2`, structs, methods, member initialization, value versus reference.

**Math:** vector addition/subtraction, magnitude, normalization, distance, velocity, and acceleration.

**Exercise:** implement a small motion triptych: seek, orbit, and bounce.

**Tests:** vector known cases, zero-length safety, speed limit, and fixed-step trajectories.

### 05. Oscillation, circles, and phase

**Visual goal:** turn repeated geometry into waves, breathing, interference, and apparent depth.

**C++:** constants, helper functions, nested loops, and indexed phase offsets.

**Math:** circles from the ground up: sine/cosine as two number-producing helpers, radians, amplitude, frequency, phase, polar coordinates, and `atan2`.

**Exercise:** create an animated field of repeated forms driven by at least two independent phases.

**Tests:** period, phase relationships, polar/cartesian round trips, and bounded amplitudes.

## Part 2 — Compose expressive marks

### 06. Controlled chance

**Visual goal:** create many related outcomes without surrendering authorship to randomness.

**C++:** random engines, seeds, simple data records, and configuration values.

**Math:** uniform choices, weighted choices, distributions as shapes, and reproducibility.

**Exercise:** generate a numbered edition of six compositions from recorded seeds.

**Tests:** same seed/same model, different-seed variation, range constraints, and stable invariants rather than fragile full snapshots.

### 07. Local coordinate systems

**Visual goal:** build nested kinetic forms from simple parts.

**C++:** lexical scope, helper objects, and RAII intuition through push/pop transform guards.

**Math:** translation, rotation, scale, relative coordinates, and transform order.

**Exercise:** construct a hierarchical kinetic sculpture with at least three local coordinate frames.

**Tests:** transformed anchor points, rotation preserving distance, and parent/child composition.

### 08. Gesture as geometry

**Visual goal:** turn pointer motion into a responsive, calligraphic line.

**Main C++ idea:** keep a size-limited vector of sampled points, while separating input collection, point calculations, and drawing. You may move that work into a small class if it helps; it is optional.

**Primary math model:** arc-length sampling from point distance. Velocity, exponential smoothing, and turning angle are introduced as small derived measurements used by the visual mapping.

**openFrameworks:** `ofPolyline`, paths, line width/color, optional triangle-strip construction.

**Exercise:** draw a gesture whose width responds to speed and whose color responds to curvature.

**Tests:** distance filtering, smoothing endpoints, uniform-ish resampling, finite output, and empty/single-point inputs.

## Part 3 — Let simple rules get weird

### 09. Particles with memory

**Visual goal:** make many tiny agents read as smoke, hair, sparks, or handwriting.

**C++:** vectors of objects, constructors, update methods, lifespan, safe removal, and avoiding accidental copies.

**Math:** Euler integration, velocity, acceleration, drag, age, and bounded/wrapped space. Revisit frame time with a fixed-step accumulator, capped catch-up work, and a documented maximum-`dt` policy after pauses.

**Exercise:** build a particle emitter whose particles leave short personal histories.

**Tests:** spawn counts, fixed-step updates, equivalent elapsed time under different frame partitions within tolerance, expiry, drag reducing speed, and boundary policies.

### 10. Forces, steering, and springs

**Visual goal:** create motion that feels attracted, repelled, elastic, or flock-like without a physics engine.

**C++:** force accumulation, enums for behavior modes, and composing small behavior functions.

**Math:** inverse-distance intuition, seek force, damping, Hooke-like springs, dot product, and limiting vectors.

**Exercise:** make a chain or swarm that can switch between at least two legible behaviors.

**Tests:** equilibrium, damping, maximum force/speed, symmetry, and state transitions.

### 11. Noise and flow fields

**Visual goal:** replace independent randomness with coherent, fluid motion.

**C++:** 2D grids, indexing, field sampling, and deterministic model generation.

**Math:** scalar versus vector fields, spatial/temporal scale, gradients as intuition, and noise-to-angle mapping.

**Exercise:** advect particles or short ribbons through a time-varying flow field.

**Tests:** grid indexing, interpolation at cell boundaries, deterministic samples, finite vectors, and particle bounds.

## Part 4 — Draw with light, time, and media

### 12. Color, blending, and trails

**Visual goal:** create luminous overlap and controlled persistence.

**C++:** color as data, small palette functions, and renderer state boundaries.

**Math:** channels, alpha composition intuition, decay, weighted averages, and color interpolation.

**Exercise:** turn an earlier particle or gesture sketch into a two-palette trail study.

**Tests:** palette endpoints, channel bounds, decay monotonicity, and state-independent model output. Render appearance is reviewed, not pixel-gated across platforms.

### 13. Time as a drawable axis

**Visual goal:** make echoes, delay, feedback, and slit-scan-like structures.

**C++:** ring buffers, modulo indexing, explicit frame/time inputs, and memory budgeting.

**Math:** discrete history, sampling, decay curves, and feedback stability.

**openFrameworks:** frame histories, `ofFbo`, and optional ping-pong buffers.

**Exercise:** create a temporal collage in which spatial position chooses a moment from history.

**Tests:** ring-buffer order, wraparound, history length, deterministic temporal selection, and memory bounds. FBO output receives a manual visual check.

### 14. Images and type as geometry

**Visual goal:** treat a picture or glyph as points, contours, a mask, or a mesh rather than as a flat rectangle.

**C++:** pixel/container indexing, asset loading errors, data ownership, and transformations over collections.

**Math:** grids, thresholds, centroids, sampling density, tangent/normal intuition, and coordinate mapping.

**openFrameworks:** `ofImage`, `ofPixels`, `ofTrueTypeFont`, paths/contours, and meshes.

**Exercise:** transform an original phrase or licensed image into animated geometry.

**Tests:** sample counts, indices, bounding boxes, centroids, asset-failure behavior, and transform invariants. Font raster pixels are not a cross-platform gate.

### 15. Elective: embodied audio input

**Visual goal:** add microphone amplitude as one live input without losing a
clear cause-and-effect loop. This is the only currently delivered elective
lane; 3D, spectrum, and camera alternatives are deferred rather than implied.

**C++:** a recorded-amplitude input adapter, exponential smoothing, a
single-atomic latest-value mailbox for the callback boundary, and graceful
permission/device failure without retaining audio.

**Exercise:** build a small amplitude-responsive instrument with keyboard,
recorded-input, and no-device routes. Check visible labels, a clear quiet-to-loud
mapping, fallback operation, and non-color cues yourself. Live input is
optional.

**Tests:** smoothing, dead zone, deterministic recorded input, explicit
no-device fallback, bounded output, and reset behavior.

## Part 5 — Make your own thing

### 16. Project: three sketch studies

Create three compact studies, each combining two technique families:

1. **gesture + temporal memory**;
2. **repetition + oscillation or transforms**; and
3. **particles + forces or flow**.

Each study stays small enough for one focused sitting. A small model test, a
capture, and a short note can make an interesting study easier to revisit. Try at
least one visual structure that differs substantially from the worked examples.

### 17. Project: an original visual instrument

Create one coherent sketch that:

- combines at least three earlier concepts;
- deepens exactly one area rather than adding every available feature;
- has deterministic/replayable core behavior where practical;
- handles resize, reset, missing assets/devices, and varying frame time;
- keeps source, build commands, controls, and credits/licenses together; optional
  capture and test notes can help you revisit or share it; and
- changes the structure or behavior—not only the palette—from important influences, which are named and credited.

Suggested directions include a calligraphic flow instrument, a typographic spring system, a temporal portrait of an everyday object, a sound-reactive contour field, or a small 3D kinetic poem.

Unlike instructional exercises, the capstone has no canonical finished solution. It provides unfinished process directions, a suggested code structure, and saved test examples without anchoring the learner to one polished answer.

## Things worth saving

1. **Foundations diptych:** two vector/motion studies.
2. **Generative edition:** six to twelve seeded outputs and a contact sheet.
3. **Interactive instrument:** discoverable controls and a one-minute capture.
4. **Capstone:** finished source, plus any process notes, test results, and known
   limitations that will help you revisit or share it.
5. **Variant challenge:** change one constraint live, predict the result, revise, and explain.

## Not on today's menu

The bootcamp does not front-load manual memory management, template authoring,
inheritance design, move-semantics theory, concurrency, custom allocators,
calculus, hand-derived matrix multiplication, machine-learning training,
or custom shaders. It also does not expect trig knowledge at the door: section
05 introduces the circle vocabulary it needs. Shaders and addons appear only
when a sketch creates a concrete need and the prerequisite CPU-side model is
already understood and testable.
