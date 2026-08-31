---
title: Exercise and testing strategy
weight: 20
draft: false
---

# Exercise and testing strategy

Creative work needs two kinds of feedback:

- **automated checks** for the program's repeatable calculations; and
- **your own judgment** about composition, motion, legibility, delight, and intent.

The course keeps those jobs separate. It does not treat visual preference as a code bug
or expect screenshots to match perfectly across different graphics hardware.

## Exercise contract

Every instructional section’s exercise (00–17) will contain:

```text
exercises/<section-slug>/
├── README.md               # brief, controls, constraints, and test command
├── starter/                # incomplete learner project
├── tests/                  # tests supplied with the exercise
├── fixtures/               # explicit inputs/assets where needed
└── solution/               # reference implementation and explanation
```

The published lesson links to the starter and tests. Instructional solutions should be easy to hide from a deployed site if desired but remain available in the repository for self-study. Larger projects use guides, repeatable test inputs, practical self-checks, and several unfinished examples; the capstone deliberately has no official finished solution.

Each brief states:

- the visual behavior to implement;
- the learner-owned aesthetic choices;
- the main C++ and math ideas;
- public function/type signatures that tests expect;
- exact build/test commands for macOS, Linux, and Windows;
- deterministic inputs such as `dt`, seed, viewport, or fixture stream;
- edge cases; and
- a manual visual checklist.

## Architecture for testable sketches

A sketch should have three layers:

```text
input adapters ──> deterministic model/core ──> renderer
 mouse/audio/       state, geometry, math,       ofPolyline,
 camera/events      fixed-step updates           ofMesh, FBO…
```

`ofApp` connects these layers but does not hide the only copy of simulation logic inside `draw()`.

Prefer functions shaped like:

```cpp
Model makeModel(const Parameters& parameters, std::uint32_t seed);
void update(Model& model, const Input& input, float dt);
Geometry buildGeometry(const Model& model, const Viewport& viewport);
```

The renderer may be platform/GPU dependent; model and geometry outputs should be inspectable without opening a window.

## Automated test pyramid

### 1. Math and mapping tests — required where applicable

Test known cases, boundaries, and properties:

- `map`, `clamp`, interpolation, easing, and wraparound;
- vector magnitude, normalization, rotation, and limiting;
- cartesian/polar conversion;
- seeded generation and ranges;
- grid/ring-buffer indexing;
- palette and channel bounds; and
- coordinate mappings under resize.

Floating-point results use explicit absolute/relative tolerances.

### 2. Simulation and state tests — required where applicable

Run updates with explicit `dt`, recorded input, and fixed seeds. The foundation
probe defines invalid time explicitly: NaN, positive infinity, negative, and
zero `dt` increment the observable step counter but leave position and velocity
unchanged; positive finite `dt` is capped at 0.25 seconds. Assert invariants such
as:

- particles remain finite and obey the boundary policy;
- drag does not increase speed;
- expired objects are removed;
- fixed inputs cause expected state transitions;
- history remains bounded and ordered; and
- missing assets/devices enter a documented fallback state.

Do not read wall-clock time, random global state, live input devices, or window dimensions from core algorithms. Every exercise still needs at least one useful repeatable behavior test plus a basic build/run check; it should not invent a fake simulation layer merely to fill a category.

### Reproducibility contract

A recorded seed guarantees repeatability within the pinned implementation and toolchain unless an exercise explicitly says more. `std::mt19937` engine sequences are portable, but standard-library distribution mappings, noise implementations, floating-point contraction, and rendering need not be byte-identical across platforms. When an edition must replay identically elsewhere, serialize the generated model parameters (or use a course-owned integer-to-value mapping) rather than relying on platform distribution output.

### 3. Geometry tests — required where relevant

Before rendering, inspect:

- point/vertex/index counts;
- index bounds and mesh topology;
- centroids and bounding boxes;
- resampling spacing;
- transformed anchor points;
- UV and color-array lengths; and
- finite coordinates/normals.

### 4. Build and smoke tests — required cross-platform

CI should distinguish compilation from test execution:

- `linux-build`
- `linux-unit`
- `macos-build`
- `macos-unit` after the no-window runner is proven
- `windows-build`
- `windows-unit` after the no-window runner is proven

The initial harness should use openFrameworks’ shipped `ofxUnitTests` and `ofAppNoWindow` pattern. Exact generated project/scheme/executable commands must be proven on clean machines for all three platforms before lessons claim support. The 0.12.1 Linux spike found that the packaged Project Generator's `unittest` template logs an error, omits build metadata, and still reports success, so the course owns the no-window runner source and `addons.make`, uses the normal platform template, rejects generator error output, and validates generated files.

### 5. Deterministic render probes — sparse and pinned

A small number of selected scenes may render at a fixed viewport, seed, time, assets, font, pixel density, framebuffer format, and graphics environment. These probes should:

- run in one pinned reference environment;
- use tolerances or a perceptual diff rather than exact whole-image equality;
- retain expected, actual, and diff images; and
- require a person to approve baseline changes.

They are diagnostics, not arbiters of artistic success.

## What must not be a cross-platform pixel gate

- fonts and text rasterization;
- antialiased lines and curves;
- shaders;
- FBO feedback;
- lighting/materials;
- filtered or mipmapped textures;
- high-DPI captures;
- edge pixels under alpha blending; and
- anything that relies on system fonts, color profiles, live devices, or wall-clock timing.

## Manual visual checklist

Every exercise also asks the learner to inspect:

- Does the intended input/output relationship read immediately?
- Is motion stable at low and high frame rates?
- Does resize preserve the composition’s logic?
- Are density, contrast, and speed deliberate?
- Does reset reproduce the chosen seed/state?
- Is the result visibly distinct from the worked examples and references in geometry, mapping, interaction, motion, or time—not merely palette?
- Is meaning available through a non-color cue where relevant?
- Is there a keyboard-accessible reset/pause or alternative to pointer-only control?
- Can intense motion be paused or reduced, and has flashing/flicker been checked?
- Is contrast sufficient for the stated intent and viewing context?
- Are third-party assets and reused code credited and licensed?

## Cross-platform baseline work

Before authoring Unit 0, implementation must prove:

1. openFrameworks 0.12.1 Project Generator output can be cleanly regenerated on macOS, Linux, and Windows from tracked `src`, `addons.make`, data, and relative shared-source arguments; reproducibility means correct source membership and a successful build, not byte-identical UUIDs or ordering;
2. the same conservative C++17 lesson source compiles on each platform;
3. the no-window unit test executable builds and runs on each platform; an `unsupported` result must be clearly labeled while work is in progress and blocks the Phase 0 exit gate;
4. a standalone clone can reliably locate a separately installed openFrameworks tree;
5. paths are relative and case-correct;
6. bundled fonts/assets render without depending on system installation; and
7. CI statuses report build and runtime-test evidence separately.

Until this spike passes, cross-platform support is a project requirement—not a completed claim. The [platform matrix](platform-matrix.md) defines the evidence statuses, and the [Project Generator architecture](project-generator-architecture.md) defines canonical inputs, generated outputs, root discovery, and regeneration rules.
