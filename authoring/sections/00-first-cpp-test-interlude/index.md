---
title: Interlude — reading and writing the first C++ test
slug: 00-first-cpp-test-interlude
weight: 40
draft: false
course_kind: instructional
objectives:
  - Trace the no-window test executable from setup through process exit
  - Read one failure using arrange, act, and assert
  - Repair one focused assertion and author one independent deterministic known case
  - Compare floating-point results with explicit absolute and relative tolerances
  - Explain why renderer pixels and visual quality stay outside headless model tests
prerequisites:
  - Completion of sections 00–02 or equivalent ability to build the foundation unit target
  - Familiarity with C++ functions, const values, and simple structs
source_records: sources.yaml
asset_records: assets.yaml
---

# Interlude — reading and writing the first C++ test

## See what we're making

A test is a small causal story, not a wall of assertions.

![Arrange fixes seed 17 and a 640 by 480 viewport, act calls makeModel, and assert compares x position 0.6848907471 within tolerance.](media/test-flow.svg "Arrange, act, assert test flow")

*A useful test separates fixed inputs, one behavior, and an observable claim.*

The diagram is static, contains no audio, and needs no motion alternative. Its
labels and left-to-right arrows carry the sequence without relying on color.

## Take a guess

Read the final line of `foundation/unit/src/main.cpp`:

```cpp
return ofRunMainLoop();
```

Predict what a shell sees when every assertion passes and when one assertion
fails. Then predict why replacing that line with `ofRunMainLoop(); return 0;`
would make a red suite look green to automation.

## Let's unpack it

### The executable lifecycle

The repository uses the `ofxUnitTests` addon shipped in openFrameworks 0.12.1.
Its pinned [`ofxUnitTests.h` implementation](https://github.com/openframeworks/openFrameworks/blob/0.12.1/addons/ofxUnitTests/src/ofxUnitTests.h)
is the source of truth for the test app behavior below. Read `main()` from top
to bottom:

```cpp
ofInit();
auto window = std::make_shared<ofAppNoWindow>();
auto app = std::make_shared<FoundationTests>();
ofRunApp(window, app);
return ofRunMainLoop();
```

[`ofAppNoWindow`](https://github.com/openframeworks/openFrameworks/blob/0.12.1/libs/openFrameworks/app/ofAppNoWindow.h)
provides an application target without creating a drawing window. In the
pinned [`ofAppRunner.cpp`](https://github.com/openframeworks/openFrameworks/blob/0.12.1/libs/openFrameworks/app/ofAppRunner.cpp),
the two-argument `ofRunApp` delegates to the main loop's `run(window, app)`.
The pinned [`ofMainLoop.cpp`](https://github.com/openframeworks/openFrameworks/blob/0.12.1/libs/openFrameworks/app/ofMainLoop.cpp)
shows what happens next: `run` registers app callbacks and, when there is no
window-owned loop, synchronously calls `notifySetup()` before `ofRunApp`
returns. That setup call runs `FoundationTests::run()`, prints the summary, and
calls `ofExit(numTestsFailed)`, which records close status. Thus the tests and
exit request happen *inside* the `ofRunApp` call, not later in
`ofRunMainLoop()`.

After `ofRunApp` returns, `ofRunMainLoop()` enters `ofMainLoop::loop()`. It
observes the already-requested close, performs exit notification and listener
cleanup, and returns the recorded status. Returning that value from `main()` is
the bridge from a failed C++ test to a failed shell or CI job. Compilation and
execution are separate proof: a test binary can compile and still return
failure.

### Which source files become each executable

A compiler translates each `.cpp` file separately, then the linker combines
those translation units into one executable. Headers such as `expect_near.h`
are included by a `.cpp`; they are not separate translation units here.

| Executable | Translation units owned by this repository | Who owns `main()` |
|---|---|---|
| Portable starter contract | `shared/core/course_probe.cpp`; `exercises/00-first-cpp-test/starter/learner_known_case.cpp`; `exercises/00-first-cpp-test/tests/learner_known_case_test.cpp` | The public contract test |
| Portable solution contract | `shared/core/course_probe.cpp`; `exercises/00-first-cpp-test/solution/learner_known_case.cpp`; `exercises/00-first-cpp-test/tests/learner_known_case_test.cpp` | The same public contract test |
| Native foundation unit | `foundation/unit/src/main.cpp`; the intentionally empty template adapters `foundation/unit/src/ofApp.cpp` and `ofApp.h`; `shared/core/course_probe.cpp` (plus openFrameworks and `ofxUnitTests` library sources selected by generated native metadata) | `foundation/unit/src/main.cpp` |

Only `.cpp` entries above are translation units; the unit `ofApp.h` is listed
to explain its tracked template-adapter role. The windowed sketch's
`foundation/windowed/src/main.cpp` and `ofApp.cpp` are
intentionally omitted from both test executables. The former would introduce a
second `main()` symbol, and the latter contains graphical callbacks that the
headless model contract neither needs nor proves. Shared model behavior reaches
both executables through `course_probe.cpp` instead.

### Arrange, act, assert

Use comments only when they clarify the roles; the values matter more than the
labels:

```cpp
// Arrange: fixed input and an independently reviewed expected value.
const std::uint32_t seed = 17U;
const course::Viewport viewport{640.0f, 480.0f};
const float expectedX = 0.6848907471f;

// Act: one behavior.
const course::Model actual = course::makeModel(seed, viewport);

// Assert: one observable claim with a useful diagnostic.
const auto near = course::test::expectNear(actual.position.x, expectedX,
                                           1e-5, 1e-6);
ofxTest(near.passed, "known seed produces reviewed x", near.message);
```

A fixture is reusable arranged state. `DeterministicFixture` owns the course's
fixed seed, viewport, input, and time step and supplies `freshModel()`. It does
not make expected results independent automatically. If expected values are
computed by calling the same function under test, the assertion merely agrees
with itself. A known-case fixture records a value that you calculated,
observed once, and reviewed.

### Approximate floating-point comparisons

Binary floating-point values do not generally follow decimal arithmetic as if
they were exact real numbers; the broader environment is described in the
[C++ floating-point reference](https://en.cppreference.com/w/cpp/types/fenv.html).
This course's helper accepts both absolute and relative tolerances:

```text
difference = |actual - expected|
allowed = max(absTol, relTol × max(|actual|, |expected|))
pass exactly when difference ≤ allowed
```

For `actual = 0.684891`, `expected = 0.684890`, `absTol = 0.00001`, and
`relTol = 0.000001`, the difference is `0.000001` and the allowed error is
`0.00001`, so the assertion passes. Absolute tolerance protects values near
zero; relative tolerance scales with magnitude. Choose tolerances from the
behavior's precision needs, not merely to silence a failure. Exact equality is
still appropriate for integer counts and deliberately exact state.

### What headless tests do not prove

The no-window suite can test model values, deterministic transitions, bounds,
and diagnostics. It does not create a graphics context or inspect `draw()`.
Pixels vary with renderer, GPU, driver, fonts, antialiasing, and color
management; more importantly, a matching screenshot cannot prove legibility,
meaning, originality, or accessibility. This interlude therefore adds no pixel
gate. Rendering remains a manual launch and review concern, while deterministic
model tests remain fast and actionable.

## Make it run

### Example 1: run the portable learner contract

```sh
for variant in starter solution; do
  CXX=g++ tests/run-first-cpp-test.sh "$variant"
  CXX=clang++ tests/run-first-cpp-test.sh "$variant"
done
```

In Windows Developer PowerShell:

```powershell
foreach ($variant in @("starter", "solution")) {
    .\tests\run-first-cpp-test.ps1 -Variant $variant
}
```

Each variant compiles its known-case test with the same course model, tolerance
helper, and public contract. The contract checks behavior rather than source
text: each test must accept the real model and reject the same injected
one-pixel mutation. It is compiled with `NDEBUG` and uses explicit conditionals,
`std::cerr` diagnostics, and nonzero returns instead of disabled `assert`
macros. A real-model failure prints `real.message`; an accepted or unexplained
mutation prints its mutation diagnostic. The public contract embeds no solution
seed or coordinates.

### Example 2: run the real no-window executable

After setting `OF_ROOT` to openFrameworks 0.12.1:

```sh
scripts/foundation.sh generate --project unit
scripts/foundation.sh build --project unit --configuration Release
scripts/foundation.sh test --project unit --configuration Release
```

In Windows Developer PowerShell:

```powershell
.\scripts\foundation.ps1 generate -Project unit
.\scripts\foundation.ps1 build -Project unit -Configuration Release
.\scripts\foundation.ps1 test -Project unit -Configuration Release
```

The strict wrapper requires process status zero and exactly one `12/12 tests
passed` summary. The tracked foundation harness—not this exercise—owns Project
Generator metadata and the native executable. Its commit-addressed
[three-platform evidence](../../../docs/foundation-harness-evidence.md)
records Linux, macOS, and Windows build and runtime statuses. Reuse avoids a
second, pedagogically identical native project. A local run proves only its
host and commit state.

## Break it on purpose

Keep the tracked branch green by applying this change only temporarily. In
`foundation/unit/src/main.cpp`, find the seeded viewport assertion and replace:

```cpp
first.position.x >= 0.0f
```

with the deliberately wrong assertion:

```cpp
first.position.x < 0.0f
```

Build and run the Release unit executable with the commands above. Read the
named failure and nonzero wrapper result. Repair it to `>= 0.0f`, rerun, and
require `12/12 tests passed`. If this is your only intended edit, this exact
command restores the tracked file:

```sh
git restore -- foundation/unit/src/main.cpp
```

It discards every uncommitted change in that file, so inspect `git diff` first.
Do not commit the broken assertion.

## Your turn

Open the [exercise brief](../../../exercises/00-first-cpp-test/README.md).
Edit only `starter/learner_known_case.cpp`. Use this concrete observe-review-
freeze workflow:

1. Choose a fixed seed and positive finite viewport, but do not change the four
   expected values yet.
2. Immediately after the one `actual` model call, temporarily add
   `std::cerr << actual.position.x << ' ' << actual.position.y << ' ' <<
   actual.velocity.x << ' ' << actual.velocity.y << '\n';` and add
   `#include <iostream>`.
3. Run `tests/run-first-cpp-test.sh starter` (or the PowerShell command). The
   old oracle may fail; copy all four printed values into notes labeled
   `position.x`, `position.y`, `velocity.x`, and `velocity.y`.
4. Review that each value is finite, position lies in the chosen viewport, and
   velocity is plausible for the model's documented speed range. Rerun once
   with the same seed and confirm all four values repeat exactly.
5. Only then replace the four expected constants and remove the temporary
   print/include. Run both compiler commands until the starter is green and its
   mutation diagnostic names a numerical difference.

This freezes observed-and-reviewed values rather than computing the oracle by
calling the implementation during arrangement. Keep one call through the
injected model factory as the act step and approximate comparisons in the
assert step. Then translate one coordinate into the `ofxTest` shape shown above
on paper or in your notes; do not add a thirteenth foundation test during this
focused exercise.

The tracked starter is already green so the repository and CI remain usable.
Your task is to replace its example oracle with your own. The explained
solution uses a different seed and cannot be inferred from the public test.

## Check your work

Run both available compiler variants and then the native foundation unit
commands. The public contract requires the learner-authored test to accept the
real model and reject a controlled wrong model while producing a diagnostic.
The starter itself checks model seed and initial step count exactly and all four
learner-authored expected floats with explicit tolerance. The existing native
suite supplies known cases, invalid
and boundary time cases, deterministic replay, and failure diagnostics.
Neither suite inspects source formatting or pixels.

Manual checks:

- The repaired assertion expresses the intended nonnegative lower bound.
- Expected coordinates came from one deliberately reviewed run, not a call to
  the injected model factory inside the arrange step.
- A failing diagnostic names the assertion and prints actual, expected,
  difference, and allowed error.
- The no-window executable returns nonzero during the temporary failure and
  zero after repair.
- No generated metadata, screenshots, or red assertion remain tracked.

## Tell the story

In 90–130 words, trace `ofInit` → `ofRunApp` registration → synchronous
`setup/run` and requested exit *inside `ofRunApp`* → `ofRunMainLoop` observing
close, cleaning up, and returning status → `main` return. Name your arrange,
act, and assert. Explain why your expected number is an independent oracle, why
tolerance is appropriate for that float, and why a passing no-window test says
nothing about rendered appearance. Include the temporary failing status and
repaired result.

## Make it yours

Add one more *note*, not code: propose a boundary or property test for
`makeModel`. State which inputs vary, which invariant should hold, and whether
exact or approximate comparison fits. Avoid testing several unrelated ideas in
one assertion.

## Quick visual check

There is no visual submission and no pixel baseline for this interlude. Review
terminal output for readable diagnostics; verify the reflection does not claim
graphical, accessibility, macOS, or Windows evidence from a local Linux run;
and credit any reused test logic. If you later capture terminal output, write
alt text that states the failing assertion and key numeric diagnostic rather
than saying only “test failure.”

## If you get stuck

A failed assertion is not a verdict on your programming career; it is a clue
with unusually dramatic punctuation. Read the actual and expected values, fix
one thing, and run the smallest test again. If the harness feels mysterious,
return to arrange → act → assert and write down the one value you expect before
running it.
