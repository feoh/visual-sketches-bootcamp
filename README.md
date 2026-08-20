# visual-sketches-bootcamp

A visual-first, learn-by-making guide to creating expressive images and animations with [openFrameworks](https://openframeworks.cc/). We teach the C++ and math we need as we go, without turning the course into a tour of every dusty corner of either subject.

The math starting line is **basic algebra**: plugging numbers into a formula and solving simple equations such as `2x + 3 = 11`. You do **not** need to know trigonometry, sine, cosine, radians, or any other math that sounds like it belongs on a haunted chalkboard. When a sketch needs those ideas, the lesson introduces them with pictures, plain language, and small calculations.

The intended learner is a curious programmer who can edit text and follow a few commands. Python experience is useful, but the lessons do not assume a formal C++ or college-math background. Every instructional section does the following:

1. introduce one visual idea;
2. teach only the C++ and mathematics needed for it;
3. build the idea through small openFrameworks examples;
4. end with a C++ sketch exercise; and
5. include automated tests for the deterministic math, geometry, and simulation behavior.

The visual vocabulary is informed by recurring techniques in [Zach Lieberman’s public work](https://x.com/zachlieberman)—gesture, repetition, oscillation, particles, flow, trails, typography, and responsive input—but the course teaches transferable techniques rather than reproducing individual artworks.

## Status

This repository contains the curriculum specification, implementation roadmap, tracked foundation probe, authored instructional sections 00–15, and authored synthesis projects 16–17. The instructional sections pair portable lesson prose with tested deterministic models, learner starters, explained reference solutions, and Linux/macOS/Windows build wrappers; the synthesis projects provide bounded briefs, evaluator fixtures, process templates, and divergent partial exemplars rather than canonical finished answers. Automated checks cover the repeatable parts; graphical appearance, accessibility, and live-device behavior are things you can inspect while making your own sketches.

- [Curriculum map](docs/curriculum-map.md)
- [Exercise and testing strategy](docs/testing-strategy.md)
- [Repository and delivery plan](docs/implementation-roadmap.md)
- [Pinned platform matrix](docs/platform-matrix.md)
- [Standalone Project Generator architecture](docs/project-generator-architecture.md)
- [Foundation spike evidence](docs/foundation-spike.md)
- [Foundation harness validation](docs/foundation-harness-evidence.md)
- [Portable authoring templates and conventions](authoring/README.md)
- [Optional Hugo publication](docs/publication.md)
- [Optional practice notes and checked routes](docs/pilot/README.md)
- [Delivery review process](docs/review-process.md)
- [Research and source notes](docs/source-notes.md)
- [License scope](LICENSE) and [third-party notices](THIRD_PARTY_NOTICES.md)

## Start here

1. Clone the repository, create a learner branch, and keep it as a disposable
   working copy so the repair exercises cannot erase unrelated work:

   ```sh
   git clone https://github.com/feoh/visual-sketches-bootcamp.git
   cd visual-sketches-bootcamp
   git switch -c learner/course
   ```

2. Check the [pinned platform matrix](docs/platform-matrix.md), then use the
   matching repository setup wrapper. On Ubuntu or CachyOS, do not run an
   upstream distro installer directly:

   ```sh
   scripts/setup-of.sh --platform linux64 --destination "$HOME/openframeworks"
   export OF_ROOT="$HOME/openframeworks/of_v0.12.1_linux64_gcc6_release"
   scripts/setup-linux.sh install --of-root "$OF_ROOT"
   ```

   Confirm that you have enough disk space, network access,
   installation/admin rights, Git, and the required compiler or IDE workload.
   CI and dated local bands are build evidence, not a claim that an ordinary
   desktop learner has completed setup on every OS.
3. Begin with [section 00: cross-platform setup and the first frame](authoring/sections/00-cross-platform-setup/index.md).
   Run commands from the repository root and set `OF_ROOT` as described in
   [the scripts guide](scripts/README.md).
4. Pick a pace from the [curriculum map](docs/curriculum-map.md#pick-a-pace-or-make-up-your-own), or ignore the map and follow your curiosity. Save captures, reflections, seed/replay inputs, and test results when they seem useful—not because a fictional registrar is checking. Resume by checking `git status` and the last section you enjoyed.

The currently authored section 15 elective is **microphone amplitude with a
recorded-input and no-device fallback**. The previously proposed 3D, spectrum,
and camera alternatives are deferred; the curriculum does not claim that those
lanes exist. Live audio is optional, and the sketch remains useful without a
microphone.

## Design constraints

- **Visual reward first:** make something move in the first lesson.
- **Just-in-time C++:** no general-purpose C++ survey before creative work begins.
- **Math you can see:** picture and motion first, plain-language explanation next, notation last.
- **No surprise prerequisites:** basic algebra is enough to begin; new math vocabulary gets introduced before it appears in an exercise.
- **Cross-platform:** macOS, Linux, and Windows are first-class targets.
- **Gentle by default:** suggested routes span 12–18 weeks; an 8+2-week fast route skips extensions, but you should stretch any route whenever that makes the work more enjoyable.
- **Talk like a human:** explain the idea before the terminology, keep the tone friendly, and allow the occasional terrible joke when it helps a concept stick.
- **Test behavior, not taste:** automated tests cover deterministic logic; visual quality remains a human judgment.
- **Plain Markdown:** content uses portable YAML front matter, relative links, and standard Markdown so it can be used by Hugo or Obsidian.
- **Originality over imitation:** exercises remix mechanisms with new inputs, mappings, geometry, palettes, and composition.

## Project tracking

The work is tracked in Witan as project `wp-visual-sketches-bootcamp-9c2c95`, with repository-scoped epics and tasks for the toolchain, curriculum units, capstones, and publication.

## Baseline

The initial implementation targets openFrameworks **0.12.1** and a deliberately conservative **C++17 language subset** for lesson code, even where individual platform toolchains enable newer language modes. Use the separate POSIX-shell and PowerShell commands documented in [`scripts/README.md`](scripts/README.md); no Python, Node, CMake, or committed native project metadata is required. Commit-addressed foundation evidence at `f3c42f4` proves Debug/Release builds and the exact 12-test no-window suite on Linux, macOS, and Windows. Current delivery-head CI and graphical appearance remain separate validation claims; see the [foundation evidence ledger](docs/foundation-harness-evidence.md) and [delivery review ledger](docs/review-process.md).

## License

Original curriculum prose and media are licensed under CC BY 4.0; source code, tests, and scripts are licensed under MIT; fixture data explicitly marked CC0-1.0 remains CC0. Third-party material retains its own terms. See [LICENSE](LICENSE), the full texts under [`LICENSES/`](LICENSES/), and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
