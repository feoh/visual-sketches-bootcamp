# visual_sketches_bootcamp

A gentle, visual-first curriculum for learning just enough modern C++ and mathematics to create expressive images and animations with [openFrameworks](https://openframeworks.cc/).

The intended learner is an experienced Python programmer with rusty C/C++ knowledge. Every instructional section will:

1. introduce one visual idea;
2. teach only the C++ and mathematics needed for it;
3. build the idea through small openFrameworks examples;
4. end with a C++ sketch exercise; and
5. include automated tests for the deterministic math, geometry, and simulation behavior.

The visual vocabulary is informed by recurring techniques in [Zach Lieberman’s public work](https://x.com/zachlieberman)—gesture, repetition, oscillation, particles, flow, trails, typography, and responsive input—but the course teaches transferable techniques rather than reproducing individual artworks.

## Status

This repository contains the curriculum specification, implementation roadmap, tracked foundation probe, authored instructional sections 00–15, and authored synthesis projects 16–17. The instructional sections pair portable lesson prose with tested deterministic models, learner starters, explained reference solutions, and Linux/macOS/Windows build wrappers; the synthesis projects provide bounded briefs, evaluator fixtures, process templates, and divergent partial exemplars rather than canonical finished answers. Human-learner pilot results and manual graphical, accessibility, and live-device evidence remain pending and are not inferred from automated checks.

- [Curriculum map](docs/curriculum-map.md)
- [Exercise and testing strategy](docs/testing-strategy.md)
- [Repository and delivery plan](docs/implementation-roadmap.md)
- [Pinned platform matrix](docs/platform-matrix.md)
- [Standalone Project Generator architecture](docs/project-generator-architecture.md)
- [Foundation spike evidence](docs/foundation-spike.md)
- [Foundation harness validation](docs/foundation-harness-evidence.md)
- [Portable authoring templates and conventions](authoring/README.md)
- [Optional Hugo publication](docs/publication.md)
- [Pilot protocol and evidence ledger](docs/pilot-protocol-and-evidence.md)
- [Delivery review process](docs/review-process.md)
- [Research and source notes](docs/source-notes.md)
- [License scope](LICENSE) and [third-party notices](THIRD_PARTY_NOTICES.md)

## Design constraints

- **Visual reward first:** make something move in the first lesson.
- **Just-in-time C++:** no general-purpose C++ survey before creative work begins.
- **Math you can see:** picture and motion first, notation second.
- **Cross-platform:** macOS, Linux, and Windows are first-class targets.
- **Gentle by default:** the recommended path is 12–16 weeks; an 8+2-week accelerated route deliberately omits some extensions.
- **Test behavior, not taste:** automated tests cover deterministic logic; visual quality remains a human judgment.
- **Plain Markdown:** content uses portable YAML front matter, relative links, and standard Markdown so it can be used by Hugo or Obsidian.
- **Originality over imitation:** exercises remix mechanisms with new inputs, mappings, geometry, palettes, and composition.

## Project tracking

The work is tracked in Witan as project `wp-visual-sketches-bootcamp-9c2c95`, with repository-scoped epics and tasks for the toolchain, curriculum units, capstones, and publication.

## Baseline

The initial implementation targets openFrameworks **0.12.1** and a deliberately conservative **C++17 language subset** for lesson code, even where individual platform toolchains enable newer language modes. Use the separate POSIX-shell and PowerShell commands documented in [`scripts/README.md`](scripts/README.md); no Python, Node, CMake, or committed native project metadata is required. Commit-addressed foundation evidence at `f3c42f4` proves Debug/Release builds and the exact 12-test no-window suite on Linux, macOS, and Windows. Current delivery-head CI and graphical appearance remain separate validation claims; see the [foundation evidence ledger](docs/foundation-harness-evidence.md) and [delivery review ledger](docs/review-process.md).

## License

Original curriculum prose and media are licensed under CC BY 4.0; source code, tests, and scripts are licensed under MIT; fixture data explicitly marked CC0-1.0 remains CC0. Third-party material retains its own terms. See [LICENSE](LICENSE), the full texts under [`LICENSES/`](LICENSES/), and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
