---
title: Curriculum map
weight: 10
draft: false
---

# Curriculum map

## Audience and outcome

This course is for an experienced Python programmer who remembers some C or C++ but does not want to relearn the language academically. By the end, the learner should be able to:

- read and modify a typical openFrameworks `ofApp`;
- structure a sketch so simulation and geometry can be tested without a window;
- use vectors, interpolation, trigonometry, transforms, randomness, noise, and simple forces intentionally;
- build gesture lines, repeated geometric systems, particles, flow fields, trails, and media-derived geometry;
- diagnose ordinary compiler, linker, runtime, and frame-rate problems;
- create and document an original, visually compelling sketch; and
- explain how its code, mathematics, and visual behavior correspond.

The recommended pace is **12–16 weeks including synthesis**, normally one substantial section per week with shorter interludes paired where appropriate. An **accelerated 8+2-week track** may cover sections 00–12 at roughly two sections per week, choose one extension from sections 13–15, and spend two further weeks on synthesis. Pilot evidence, not the calendar, determines whether the accelerated path is genuinely manageable.

## Recurring lesson shape

Every instructional section (00–15) follows the same scaffold. The synthesis projects (16–17) use a separate project contract rather than pretending to be ordinary lessons:

1. **Look:** a short visual goal and animated/still reference.
2. **Predict:** a no-notes question about what a small code fragment will draw or how it will move.
3. **Learn:** one mathematical model and at most one substantial C++ mechanism.
4. **Build:** two or three small, complete example programs.
5. **Break and repair:** one planted bug or misleading implementation.
6. **Exercise:** a constrained but open-ended C++ sketch.
7. **Test:** supplied tests for deterministic behavior and edge cases.
8. **Reflect:** a capture plus a short explanation connecting code, math, and image.
9. **Remix:** one optional aesthetic variation that reuses earlier ideas.

Guidance fades across the course: complete example → completion problem → constrained remix → open brief.

## Unit 0 — Runway

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

**Primary assessed C++ mechanism:** decompose repeated drawing into functions operating on a `std::vector` of values. Supporting reading knowledge covers parameters/returns, `const`, references seen in APIs, range loops, scoped value lifetime, and why the course avoids `new`/`delete`; these are not all independently assessed here.

**Math:** indexed ranges and normalized progress from 0 to 1.

**Exercise:** produce a family of animated marks from a vector of parameters.

**Tests:** collection size, endpoint values, empty/single-item boundary coverage, and deterministic output. Sanitizers or checked/debug iterators are enabled where the platform supports them; tests do not claim to prove the absence of every possible out-of-bounds access.

### Interlude. Reading and writing the first C++ test

**Goal:** make the exercise harness part of the learner’s vocabulary before tests become an unexplained ritual.

Run one `ofxUnitTests`/`ofAppNoWindow` executable, repair one failed assertion, write one known-case test, and use the course’s approximate-floating-point helper. Explain test source versus sketch source, test executable linking, arrange/act/assert, fixtures, and why renderer output is usually outside this test layer. By Unit 2, every learner authors at least one meaningful property or invariant test.

## Unit 1 — Motion with intention

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

**Math:** sine/cosine as circular motion, radians, amplitude, frequency, phase, polar coordinates, and `atan2`.

**Exercise:** create an animated field of repeated forms driven by at least two independent phases.

**Tests:** period, phase relationships, polar/cartesian round trips, and bounded amplitudes.

## Unit 2 — Composition and expressive marks

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

**Primary assessed C++ mechanism:** maintain a bounded vector of sampled points while keeping input sampling, model processing, and rendering separate. A small class is optional refactoring rather than a second assessment target.

**Primary math model:** arc-length sampling from point distance. Velocity, exponential smoothing, and turning angle are introduced as small derived measurements used by the visual mapping.

**openFrameworks:** `ofPolyline`, paths, line width/color, optional triangle-strip construction.

**Exercise:** draw a gesture whose width responds to speed and whose color responds to curvature.

**Tests:** distance filtering, smoothing endpoints, uniform-ish resampling, finite output, and empty/single-point inputs.

## Unit 3 — Emergent systems

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

## Unit 4 — Drawing with light, time, and media

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

### 15. Elective: depth **or** embodied input

**Visual goal:** add one new dimension **or** one live input without losing a clear cause-and-effect loop. The learner chooses one lane; this is not a combined 3D/audio/camera survey.

**3D lane:** `glm::vec3`, camera and perspective intuition, one mesh-topology rule, depth, and projection. The primary C++ mechanism is building validated mesh data.

**Embodied-input lane:** choose microphone amplitude/spectrum **or** camera threshold/contours. The primary C++ mechanism is an input adapter with recorded fixtures and graceful device failure. Camera work includes consent and no frame retention.

**Exercise:** build a small instrument in which the input mapping can be inferred within five seconds.

**Tests:** the chosen lane tests either mesh index/coordinate validity or smoothing, dead zones, deterministic recorded input, and explicit no-device fallback.

## Unit 5 — Synthesis projects

### 16. Project: three sketch studies

Create three compact studies, each combining two technique families:

1. **gesture + temporal memory**;
2. **repetition + oscillation or transforms**; and
3. **particles + forces or flow**.

Each study gets one weeknight-sized constraint, one automated model test, one capture, and a 100-word explanation. At least one must use a visual grammar substantially different from the worked examples.

### 17. Project: an original visual instrument

Create one coherent sketch that:

- combines at least three earlier concepts;
- deepens exactly one area rather than adding every available feature;
- has deterministic/replayable core behavior where practical;
- handles resize, reset, missing assets/devices, and varying frame time;
- includes source, build notes, controls, credits/licenses, captures, and test results; and
- passes a mechanism-level originality review: name the precedent, state the transferable principle, document code/assets and licenses, and explain at least three deliberate changes among geometry, density, palette, input, mapping, interaction, motion, or temporal behavior. A confusingly similar still is one warning, not the entire test.

Suggested directions include a calligraphic flow instrument, a typographic spring system, a temporal portrait of an everyday object, a sound-reactive contour field, or a small 3D kinetic poem.

Unlike instructional exercises, the capstone has no canonical finished solution. It provides divergent partial process exemplars, architectural patterns, evaluator fixtures, and rubric examples so the course does not anchor the learner to one polished answer.

## Portfolio checkpoints

1. **Foundations diptych:** two vector/motion studies.
2. **Generative edition:** six to twelve seeded outputs and a contact sheet.
3. **Interactive instrument:** discoverable controls and a one-minute capture.
4. **Capstone:** finished source, process note, test summary, and known limitations.
5. **Variant challenge:** change one constraint live, predict the result, revise, and explain.

## Deliberately deferred topics

The bootcamp does not front-load manual memory management, template authoring, inheritance design, move-semantics theory, concurrency, custom allocators, formal calculus, hand-derived matrix multiplication, machine-learning training, or custom shaders. Shaders and addons appear only when a sketch creates a concrete need and the prerequisite CPU-side model is already understood and testable.
