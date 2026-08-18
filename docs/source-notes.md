---
title: Research and source notes
weight: 40
draft: false
---

# Research and source notes

## Visual vocabulary

The curriculum draws from recurring, transferable technique families visible in Zach Lieberman’s public practice:

- sampled gesture and expressive polylines;
- repeated geometry, oscillation, phase, and interference;
- particles, forces, springs, and coherent flow;
- typography and images treated as geometry;
- camera/sound/body input mapped into immediate visual feedback; and
- time used as material through trails, delay, frame history, and feedback.

These are mechanisms, not recipes for reproducing individual works. Lessons should cite precedents, identify collaborations, and require learners to choose original inputs, mappings, geometry, palettes, and compositions.

Primary or creator-owned starting points:

- [Zach Lieberman’s X feed](https://x.com/zachlieberman)
- [Official portfolio](https://zach.li/)
- [`ofZach/dailySketches`](https://github.com/ofZach/dailySketches)
- [`ofZach/gestureMachine`](https://github.com/ofZach/gestureMachine)
- [`ofZach/ofxCv`](https://github.com/ofZach/ofxCv)
- [The EyeWriter](http://www.eyewriter.org/)
- [Land Lines](https://experiments.withgoogle.com/land-lines)
- [Messa di Voce](https://www.flong.com/archive/projects/messa/)

The named works are mechanism references, not composition targets. Credit their
collaborative authorship precisely:

- **The EyeWriter** was created with TEMPT1 by a team including Zach Lieberman,
  Evan Roth, James Powderly, Theo Watson, and Chris Sugrue, with additional
  contributors credited by the project. Transfer only the principle of making
  an expressive control loop legible and accessible; do not copy its identity,
  interface, or documentary imagery.
- **Land Lines** credits Zach Lieberman and Google's Data Arts Team. Transfer
  the idea of mapping a gesture to a searchable or generative geometry system,
  not its satellite imagery, line-matching composition, or interaction copy.
- **Messa di Voce** credits Golan Levin and Zach Lieberman as creators and Jaap
  Blonk and Joan La Barbara as performers. Transfer the immediate mapping from
  embodied sound to visible behavior, not its vocal gestures, silhouettes,
  staging, or projected forms.

The creator-owned pages above remain the authority if a summary conflicts with
their full credit lists. Every lesson process note should name the specific
precedent, all prominently credited collaborators, the transferable mechanism,
and at least three deliberate changes. Do not bundle precedent imagery or use a
precedent still as a target screenshot unless redistribution rights are
separately verified.

## openFrameworks baseline

The initial curriculum baseline is [openFrameworks 0.12.1](https://github.com/openframeworks/openFrameworks/releases/tag/0.12.1), released 2025-05-02.

Relevant official sources:

- [Download and setup](https://openframeworks.cc/download/)
- [Learning portal](https://openframeworks.cc/learning/)
- [Create a new project](https://openframeworks.cc/learning/01_basics/create_a_new_project/)
- [C++ language basics](https://openframeworks.cc/ofBook/chapters/cplusplus_basics.html)
- [GLM syntax](https://openframeworks.cc/learning/02_graphics/how_to_use_glm/)
- [`ofxUnitTests` source](https://github.com/openframeworks/openFrameworks/tree/0.12.1/addons/ofxUnitTests)
- [Unit-test template](https://github.com/openframeworks/openFrameworks/tree/0.12.1/scripts/templates/unittest)
- [0.12.1 CI workflows](https://github.com/openframeworks/openFrameworks/tree/0.12.1/.github/workflows)
- [Project Generator 0.103.0 source bundled by 0.12.1](https://github.com/openframeworks/projectGenerator/tree/74afcd48555ba9b3d0c430b2853ddad306a89b63)
- [Pinned course platform matrix](platform-matrix.md)
- [Standalone Project Generator architecture and spike evidence](project-generator-architecture.md)

The framework’s platform configurations enable C++20 on the selected Linux compiler family, C++23 in the `osx` template, and the latest available C++ mode in the Visual Studio template. Shared lesson code deliberately remains within the enumerated conservative C++17 contract in the platform matrix.

## Pedagogy

The design combines constructionist, spiral, worked-example, retrieval-practice, and faded-guidance ideas:

- [Papert, “Situating Constructionism”](http://www.papert.org/articles/SituatingConstructionism.html)
- [Bruner, *The Process of Education*](https://www.hup.harvard.edu/books/9780674710016)
- [National Academies, *How People Learn II*](https://nap.nationalacademies.org/catalog/24783/how-people-learn-ii-learners-contexts-and-cultures)
- [Dunlosky et al., learning-technique review](https://journals.sagepub.com/doi/10.1177/1529100612453266)
- [Atkinson et al., worked-example review](https://doi.org/10.3102/00346543070002181)
- [Ashcraft and Kirk, math anxiety and working memory](https://doi.org/10.1037/0096-3445.130.2.224)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)

The recommended 12–16-week pace, accelerated 8+2-week option, and lesson ordering are curriculum judgments to be adjusted after pilot checkpoints; they are not claimed as research-established optima.

## Evidence limitations

- The X/Instagram feeds have not been exhaustively sampled or frequency-coded. Technique recurrence is qualitative and triangulated with creator-owned repositories and project pages.
- Native openFrameworks render variance has not yet been measured across the course’s three target platforms.
- Commit `f3c42f4` has repository-recorded Linux, macOS, and Windows generation,
  Debug/Release build, and foundation no-window unit proof in the
  [foundation evidence ledger](foundation-harness-evidence.md). That dated proof
  does not establish current-HEAD CI, graphical launch/appearance, or live-device
  behavior; those claims require fresh commit-addressed or manual evidence.
- Official CI and release-adjacent runner manifests document broad build
  environments, but every release must still link its exact generated-project
  and no-window test statuses on all three lanes.
- Links and API documentation may drift; pinned 0.12.1 headers and examples take precedence when web docs disagree.
- Addons, fonts, media, codecs, and live-device APIs require separate platform and license review before becoming required content.
