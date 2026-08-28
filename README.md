# Visual Sketches Bootcamp

Make expressive images and animations with
[openFrameworks](https://openframeworks.cc/) while learning the C++ and math you
need along the way. The course starts with a still image, then adds motion,
gestures, repeated forms, particles, flow, trails, images, and optional sound
input.

This is a learn-by-making guide for a curious adult who can edit a text file and
follow a few terminal commands. Every new math idea appears when a sketch needs
it.

## What you need before starting

You need basic arithmetic and a little algebra: plugging numbers into a formula
and solving a small equation such as `2x + 3 = 11`. Python experience is useful,
but not required.

You do **not** need prior trigonometry, calculus, physics, or C++ experience.
When a sketch needs a vector, sine, radians, interpolation, probability, or a
spring, the lesson begins with a picture and a worked example. The technical
name comes afterward.

If mathematical notation makes you freeze, read the words and numerical example
first. The notation is a shorter way to record the same idea, not a test of
whether you belong here.

## How the course works

Each guided section follows one clear sequence:

1. **Lesson:** learn one visual idea and the C++ and math it needs.
2. **Practice:** apply it in small guided changes and repair one focused mistake.
3. **Exercise:** solve one problem in a focused starter, then use unit tests to
   check attributes that indicate correctness.

Tests cannot judge your taste. They can check that a shape stays in the window
or that reset gives the same starting values. They cannot decide whether your
image is interesting, clear, or fun to play. You decide those parts by running
the sketch and looking, listening, and interacting.

The visual vocabulary is informed by recurring techniques in
[Zach Lieberman's public work](https://x.com/zachlieberman)—gesture, repetition,
oscillation, particles, flow, trails, typography, and responsive input. The
course teaches reusable techniques rather than asking you to copy an artwork.

## Start here

1. Clone the repository and make a course branch. Some lessons intentionally
   break a file, so keep your work separate from anything important.

   ```sh
   git clone https://github.com/feoh/visual-sketches-bootcamp.git
   cd visual-sketches-bootcamp
   git switch -c course
   ```

2. Open
   [section 00: setup and the first frame](authoring/sections/00-cross-platform-setup/index.md).
   Follow the block for Linux, macOS, or Windows. Run commands from the
   repository root.

3. Continue in order or use the
   [curriculum map](docs/curriculum-map.md) to choose a route. Stretch the pace
   whenever a concept needs more time.

4. Save screenshots, notes, seeds, and test output when they help **you** return
   to a sketch.

## What is included

The course contains:

- 16 guided lessons, numbered 00 through 15;
- three short sketch studies in section 16; and
- one larger visual-instrument project in section 17.

Every guided exercise has a focused starter with clear change points,
number-based tests, one possible solution, and build commands. The solution is
an example to study, never the picture you must reproduce.

The section 15 sound exercise always includes recorded-input and keyboard routes.
Live microphone input is optional. You can complete it without a microphone or
recording any sound.

## Course promises

- **Visual reward first:** make or change something visible early.
- **Math you can see:** picture first, ordinary explanation next, notation last.
- **No surprise prerequisites:** basic algebra is enough to begin.
- **Just-in-time C++:** learn a language feature when the sketch needs it.
- **Gentle pacing:** repeat an exercise, take a side trip, or ignore a suggested
  schedule without falling “behind.”
- **Human language:** technical words get explanations instead of being used as
  passwords.
- **Test behavior, not taste:** computers check calculations; people judge the
  visual result.
- **Originality over imitation:** change relationships, input, motion, shapes,
  density, and color instead of tracing a target screenshot.
- **Access matters:** lessons include keyboard routes, reduced-motion choices,
  non-color cues, alt text, and fallbacks where they apply.

## Useful links

For taking the course:

- [Curriculum map and suggested routes](docs/curriculum-map.md)
- [Supported operating systems and tools](docs/platform-matrix.md)
- [Scripts and commands](scripts/README.md)
- [Optional practice notes](docs/pilot/README.md)
- [Research and source notes](docs/source-notes.md)
- [License](LICENSE) and
[third-party notices](THIRD_PARTY_NOTICES.md)

For contributors and maintainers:

- [Testing strategy](docs/testing-strategy.md)
- [Implementation roadmap](docs/implementation-roadmap.md)
- [Project Generator notes](docs/project-generator-architecture.md)
- [Foundation checks](docs/foundation-harness-evidence.md)
- [Authoring guide](authoring/README.md)
- [Publication guide](docs/publication.md)
- [Review process](docs/review-process.md)

You do not need the contributor documents to take the course.

## Tool versions

The lessons use openFrameworks **0.12.1** and a small, conservative part of
**C++17**. Follow the shell or PowerShell commands in
[`scripts/README.md`](scripts/README.md). You do not need Python, Node, or CMake,
and you should not commit generated Xcode, Visual Studio, Make, or VS Code
project files.

Automated builds run on Linux, macOS, and Windows. A green build means the code
compiled and the number-based tests passed. It does not replace opening the app
and checking its appearance and controls yourself.

## License

Original curriculum prose and media are licensed under CC BY 4.0. Source code,
tests, and scripts are licensed under MIT. Saved test data marked CC0-1.0 remains
CC0. Third-party material keeps its own terms. See [LICENSE](LICENSE), the full
texts under [`LICENSES/`](LICENSES/), and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
