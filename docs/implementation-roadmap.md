---
title: Repository and delivery plan
weight: 30
draft: false
---

# Repository and delivery plan

## Content portability

Lesson files will use standard Markdown plus conservative YAML front matter:

```yaml
---
title: Vectors — direction and distance
weight: 40
draft: false
objectives:
  - Explain a 2D vector as direction and distance
  - Implement fixed-step seek motion
---
```

Rules:

- standard Markdown links and fenced code blocks;
- repository-relative media links;
- no required Hugo shortcodes, Obsidian-only wikilinks, or plugin-specific syntax;
- each instructional lesson is a Hugo leaf bundle whose folder contains `index.md` and colocated media;
- links use source-file-relative POSIX paths that also resolve in Obsidian;
- descriptive alt text and transcripts/captions for motion media;
- stable slugs and explicit ordering; and
- source links and asset licenses recorded alongside the lesson.

The optional Hugo layer now adds layouts, configuration, and Markdown render
hooks around these files without changing lesson prose; see
[the publication guide](publication.md). The required Hugo build plus
link/media checker proves the mounted source contract, while the same source
tree remains usable as an Obsidian vault.

## Implemented repository shape

```text
visual_sketches_bootcamp/
├── README.md
├── docs/
│   ├── curriculum-map.md
│   ├── testing-strategy.md
│   ├── implementation-roadmap.md
│   └── source-notes.md
├── authoring/
│   └── sections/            # canonical mounted lesson leaf bundles
│       ├── 00-cross-platform-setup/index.md
│       ├── ...
│       ├── 16-three-sketch-studies/index.md
│       └── 17-original-visual-instrument/index.md
├── site/                    # optional Hugo config, layouts, and CSS
│   ├── hugo.toml
│   ├── layouts/
│   └── static/
├── exercises/
│   └── <section-slug>/
├── shared/
│   ├── core/              # reusable deterministic helpers
│   ├── test-support/      # tolerances, fixtures, runner support
│   └── assets/            # explicitly licensed course assets
├── scripts/               # platform setup/build/test wrappers
└── .github/workflows/     # separate build and unit-test statuses
```

The repository remains standalone, with openFrameworks installed separately. Project Generator remains authoritative for native project files, which are regenerated per host and kept out of version control because they contain machine-local `OF_ROOT` references. See the [platform matrix](platform-matrix.md) and [Project Generator architecture](project-generator-architecture.md).

## Delivery phases

### Phase 0 — Foundation spike

Deliver:

- pinned openFrameworks/toolchain matrix;
- clean-machine setup guides for macOS, Linux, and Windows;
- one generated sketch project that builds on all three and renders one bundled, licensed asset in the manual probe;
- one `ofxUnitTests`/`ofAppNoWindow` project that builds and runs on all three;
- scripts that report actionable failures;
- CI with separate build/test statuses; and
- the accepted standalone-repository and clean-regeneration architecture.

**Exit gate:** on every selected lane, a contributor can clone the repository, follow the platform guide, generate and run the sketch (including its bundled-asset probe), and run deterministic tests without undocumented manual edits. Every lane must be both `build-proven` and `unit-proven`. An `unsupported` result remains visible with a reason and follow-up criterion, but blocks this phase rather than being silently skipped or shown as green.

### Phase 1 — Authoring system and Unit 0

Deliver the leaf-bundle lesson template, exercise template, media conventions, accessibility checklist, licensing policy, Hugo/link smoke check, testing-literacy interlude, and sections 00–02 with tested starters and solutions.

**Exit gate:** a pilot learner reaches a moving sketch in the first session and can run tests on their platform.

### Phase 2 — Units 1 and 2

Deliver mapping/interpolation, vectors, oscillation, randomness, transforms, and gesture geometry.

**Exit gate:** the learner completes a foundations diptych and a reproducible generative edition without copying a worked composition.

### Phase 3 — Unit 3

Deliver particles, forces/springs/steering, and noise/flow fields with fixed-step simulation tests.

**Exit gate:** the learner can explain and test the difference between independent randomness and coherent field-driven motion.

### Phase 4 — Unit 4

Deliver blending/trails, temporal buffers, image/type geometry, and depth/live-input lanes. Camera/audio work must include privacy, fixture, and no-device fallback guidance.

**Exit gate:** the learner produces an interactive instrument whose mapping is legible and whose core can replay recorded input deterministically.

### Phase 5 — Synthesis and publication

Deliver three sketch-study briefs, capstone brief/rubric, gallery/process-note templates, instructional reference solutions, divergent capstone process exemplars (not one canonical solution), full link/license checks, and optional Hugo deployment.

**Exit gate:** the complete path has been piloted, cross-platform validation is green or limitations are explicit, and a learner can publish a documented original capstone.

## Definition of done for an instructional section

An instructional section (00–15) is complete only when it has:

- YAML front matter and stable relative links;
- stated prerequisites and 3–6 observable objectives;
- visual result preview with accessible description;
- one math idea taught visually, numerically, then symbolically;
- no more than one substantial new C++ mechanism;
- two or three runnable worked examples;
- one repair/debugging example;
- one exercise starter, tests, and instructional reference solution;
- platform commands validated in CI or clearly marked unsupported;
- automated tests for known cases, boundaries, and at least one property;
- a manual visual/originality checklist;
- credits and licenses; and
- a pilot note recording completion time and points of confusion.

A synthesis project is complete when its brief, milestones, deterministic evaluator fixtures, technical/creative/originality rubric, accessibility/license expectations, captures/process-note template, and multiple visually divergent partial exemplars are ready. It does not require a canonical finished answer.

## Debugging progression

Debugging is scheduled rather than left as an implicit outcome:

| Section | Deliberate failure | Technique |
|---|---|---|
| 00 | compiler diagnostic | read the first relevant location/message and make one change |
| 02 | linker error from a missing definition | distinguish declaration, definition, compile, and link |
| 03 | failed float assertion | inspect values and use explicit tolerance |
| 07 | asset/path or transform-state mistake | relative paths, logs, and scoped renderer state |
| 08 | empty input and invalid numeric value | assertions, finite checks, and minimal reproduction |
| 09 | frame-time spike/NaN in a simulation | fixed-step tracing, clamping, and sanitizer/debug builds where supported |
| 11 | frame-budget pressure | measure update/draw time and particle/field cost before optimizing |
| 13–15 | GPU/device failure | feature fallback, actionable logs, and renderer/device isolation |

## Quality review

Each unit receives independent review from four angles:

1. **technical:** correct C++, openFrameworks APIs, deterministic core, and honest platform claims;
2. **mathematical:** notation and examples match the implemented behavior;
3. **pedagogical:** visual payoff arrives early, scaffolding fades, and assumed knowledge is explicit; and
4. **creative/accessibility:** the brief leaves meaningful aesthetic agency, avoids cloning, and media has usable alternatives.

Pilot checkpoints after Units 0 and 2 should adjust pacing from observed setup time, test success, independent reconstruction, and learner confidence rather than visual polish alone.

## Explicit risks

| Risk | Response |
|---|---|
| openFrameworks/toolchain drift | Pin 0.12.1 initially; review upgrades deliberately; never write “latest” in setup commands. |
| Platform-specific generated projects | Treat Project Generator as source of truth and validate regeneration in CI where feasible. |
| Too much C++ before art | Require a visual artifact every section and maintain a visible “not needed yet” list. |
| Too much math notation | Introduce picture → plain language → numbers → notation → code. |
| Brittle graphics snapshots | Gate model/state/geometry; keep sparse render probes in one pinned environment. |
| Frame-rate dependence | Use explicit `dt`, fixed-step tests, and extreme-`dt` handling. |
| Imitation rather than transfer | Require remixes, novel mappings, citations, and an originality check. |
| Addon or asset churn | Keep addons optional, pin versions, bundle only licensed assets, and test failure paths. |
| Cross-platform scope slows authoring | Complete the foundation spike first and separate build support from runtime-test support. |

## Witan coordination

Witan is the system of record for execution:

- Workflow project: `wp-visual-sketches-bootcamp-9c2c95`
- Repository: <https://github.com/feoh/visual_sketches_bootcamp>
- Tasks are repository-scoped and roll up through foundation, curriculum-unit, synthesis, and publication epics.
- Dependencies enforce “toolchain first,” then progressively authored units, then capstone/publication.
- Sessions should record changed files, validation evidence, remaining work, and curriculum decisions.
