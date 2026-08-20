---
title: Cross-platform setup and the first frame
slug: 00-cross-platform-setup-and-first-frame
weight: 10
draft: false
course_kind: instructional
objectives:
  - Verify the pinned openFrameworks and Project Generator versions before changing code
  - Explain the roles of main.cpp, ofApp.h, and ofApp.cpp
  - Map normalized screen positions to pixel coordinates
  - Build a first frame from color and primitive drawing calls
  - Use the first useful compiler diagnostic to repair one syntax error
prerequisites:
  - Comfort editing plain text files and running commands in a terminal
  - An x86-64 Ubuntu 24.04 or CachyOS environment, Apple Silicon macOS 15, or x64 Windows Server 2022 environment
source_records: sources.yaml
asset_records: assets.yaml
---

# Cross-platform setup and the first frame

## See what we're making

This entire image is one frame: a background plus exactly five calls that draw
geometry. It is a reference for constraints, not a composition to copy.

![Navy rectangle, coral circle, yellow triangle and diagonal, and navy ellipse form two uneven rows on a warm ground.](media/five-primitive-preview.svg "The reference uses exactly five primitives and three colors; position and silhouette still distinguish forms without color.")

*The reference uses exactly five primitives and three colors; position and silhouette still distinguish forms without color.*

The still has no motion or audio. Its SVG title and description, the alt text
above, and the visible caption carry the same spatial information without
requiring color perception.

## Take a guess

Without running this fragment, sketch the location of the circle and name which
statement changes later drawing calls:

```cpp
void ofApp::draw() {
    ofSetColor(238, 91, 71);
    ofDrawCircle(200, 150, 40);
}
```

Is `(200, 150)` measured from the lower-left, center, or upper-left? Is `40` a
diameter or radius? Keep your prediction; verify it during Build.

## Let's unpack it

### Install one pinned toolchain

The section wrappers target four host bands: Ubuntu 24.04 x86-64 with GCC 13,
x86-64 CachyOS/Arch with its current GCC family, Apple Silicon macOS 15 with
Xcode 16, and x64 Windows Server 2022 with Visual Studio 2022 v143 plus Windows
SDK 10.0.26100.0. Ubuntu, macOS, and Windows have distinct native CI statuses.
CachyOS is rolling, so its support evidence records the exact dated package
versions and local commands rather than pretending that one run proves every
future snapshot. Windows 11 is not yet a verified band.

All lanes use openFrameworks 0.12.1 and Project Generator 0.103.0. The course
wrappers verify downloaded hashes and versions; they do not search your
computer or use a rolling `latest` framework release. The upstream [setup guides](https://openframeworks.cc/setup/)
provide background, but do not run an upstream distro installer directly: the
course's Linux wrapper selects apt only on Ubuntu and pacman/paru only on
CachyOS or Arch.

Ubuntu or CachyOS from the repository root:

```sh
scripts/setup-of.sh --platform linux64 --destination "$HOME/openframeworks"
export OF_ROOT="$HOME/openframeworks/of_v0.12.1_linux64_gcc6_release"
scripts/setup-linux.sh install --of-root "$OF_ROOT"
scripts/foundation.sh doctor
```

On CachyOS/Arch, `setup-linux.sh` avoids the upstream OpenCV 3/4 rewrite (the
course does not use `ofxOpenCv`), rebuilds Project Generator against current
host libraries, adds the GCC-required `<algorithm>` include, and rebuilds
openFrameworks with an X11 GLFW hint. The hint is required because
openFrameworks 0.12.1 uses X11-native input and icon APIs while CachyOS GLFW
3.4+ can otherwise select Wayland. `xorg-xwayland` is therefore an explicit
course dependency. These changes affect only the separately downloaded
`OF_ROOT`; rerunning `setup-of.sh` restores the verified archive and requires a
new `setup-linux.sh prepare` or `install` afterward.

macOS from the repository root:

```sh
scripts/setup-of.sh --platform osx --destination "$HOME/openframeworks"
export OF_ROOT="$HOME/openframeworks/of_v0.12.1_osx_release"
scripts/foundation.sh doctor
```

Windows Developer PowerShell from the repository root, after installing the
Visual Studio 2022 **Desktop development with C++** workload and the selected
Windows SDK:

```powershell
.\scripts\setup-of.ps1 -Destination "$HOME\openframeworks"
$env:OF_ROOT = "$HOME\openframeworks\of_v0.12.1_vs_64_release"
.\scripts\foundation.ps1 doctor
```

A successful doctor prints `openFrameworks=0.12.1` and Project Generator
`0.103.0`. Stop on any error; do not patch a generated project to get around a
wrong root or version.

### Project Generator and the three source roles

Project Generator reads tracked `src`, `addons.make`, data, and an explicit
shared-source path, then creates Make, Xcode, Visual Studio, or VS Code metadata
for this machine. The official [Project Generator documentation](https://github.com/openframeworks/openFrameworks/blob/0.12.1/docs/projectgenerator.md)
explains the tool. Generated metadata and binaries are ignored: regenerate
them after source files change; never commit or hand-edit them.

- `main.cpp` creates an 800 × 600 window, hands it an `ofApp`, and starts the
  main loop.
- `ofApp.h` declares the app's shape: callback names and stored values. A header
  is a table of contents other source files can include.
- `ofApp.cpp` defines what those declared callbacks do. `ofApp::` says the
  function belongs to `ofApp`.

Compilation translates source files and reports language/type mistakes.
Linking joins compiled pieces with openFrameworks. Running starts the finished
program. A build can succeed even if nobody has manually inspected its window.
The [ofBaseApp reference](https://openframeworks.cc/documentation/application/ofBaseApp.html)
defines the callbacks: `setup()` runs once after window creation; `draw()` runs
again for each frame.

### Screen coordinates: visual, numeric, symbolic

```text
(0,0) ───────────────► +x = width
  │
  │       (200,150)
  │
  ▼
 +y = height
```

The origin is the upper-left; x grows right and y grows down. In an 800 × 600
window, `(200, 150)` is one quarter of the width and one quarter of the height.
Numerically, `0.25 × 800 = 200` and `0.25 × 600 = 150`. Symbolically, for
normalized choices `u` and `v`:

```text
x = uW    and    y = vH,    where 0 ≤ u,v ≤ 1
```

A specification's `normalized_size = (s, t)` stores its **full** width and
height as viewport fractions. The geometry model stores half-extents for easy
boundary checks:

```text
half_width = sW / 2    and    half_height = tH / 2
```

For `normalized_size = (0.10, 0.20)` at 800 × 600, those half-extents are
`0.10 × 800 / 2 = 40` and `0.20 × 600 / 2 = 60` pixels. The exercise keeps
`u`, `v`, `s`, and `t` in standard-library C++ geometry, then supplies `W` and
`H` at the window boundary. Tests can inspect the result without a GPU.

### Color and primitives

`ofSetBackgroundColor(red, green, blue)` sets the clear color. `ofSetColor(...)`
sets state for later drawing calls; it draws nothing by itself. Then calls such
as `ofDrawCircle`, `ofDrawRectangle`, `ofDrawTriangle`, `ofDrawLine`, and
`ofDrawEllipse` emit geometry. Channels range from 0 through 255. The official
[ofGraphics reference](https://openframeworks.cc/documentation/graphics/ofGraphics/)
is the API authority.

This is one substantial C++ mechanism: use declarations in a header and
function definitions/calls across source files. Arrays and enums in supplied
code are readable scaffolding here, not new assessed mechanisms.

## Make it run

Keep generation, compilation, and running separate so an error has one home.

### Example 1: the proven first window

Read the complete foundation adapter in
[`foundation/windowed/src/ofApp.cpp`](../../../foundation/windowed/src/ofApp.cpp).
Generate and compile it without editing generated files:

```sh
scripts/foundation.sh generate --project windowed
scripts/foundation.sh build --project windowed --configuration Release
```

On Windows use:

```powershell
.\scripts\foundation.ps1 generate -Project windowed
.\scripts\foundation.ps1 build -Project windowed -Configuration Release
```

Launch the emitted app from `foundation/windowed/bin/` manually. This is visual
evidence, separate from the successful build.

### Example 2: one shape

In a generated scratch copy, these definitions clear once and draw once per
frame:

```cpp
void ofApp::setup() {
    ofSetBackgroundColor(248, 245, 236);
}

void ofApp::draw() {
    ofSetColor(238, 91, 71);
    ofDrawCircle(200.0f, 150.0f, 40.0f);
}
```

Confirm your Predict answer: the center is 200 pixels right and 150 pixels down
from the upper-left; 40 is the radius.

### Example 3: the exercise starter

The complete [`starter`](../../../exercises/00-visual-signature/starter/) keeps
five normalized specifications separate from rendering:

```sh
scripts/section-00.sh generate --project starter
scripts/section-00.sh build --project starter --configuration Release
tests/run-section-00-tests.sh
```

The equivalent Windows commands are in the exercise brief. Open the app only
after generation and compilation pass.

## Break it on purpose

In the starter, delete the semicolon after one `ofSetColor(...)` call in the
exact tracked file `exercises/00-visual-signature/starter/src/ofApp.cpp`, then
build again. Compiler wording varies, but find the **first**
diagnostic that names your file and line. A useful GCC/Clang form is:

```text
error: expected ';' after expression
```

Ignore later cascading messages until that first local error is repaired. Read
the named line and the line immediately before it, restore the semicolon, save,
and build again. Do not regenerate: source syntax changed, not project
membership. If this was the only intentional change to that file, you can
restore the repository copy with
`git restore -- exercises/00-visual-signature/starter/src/ofApp.cpp`; this
command discards every uncommitted edit in that file. Record the diagnostic,
cause, and repair in one sentence.

## Your turn

Create a visual signature from exactly five primitives and three colors. Start
with the [exercise brief](../../../exercises/00-visual-signature/README.md),
then edit the tracked design source. Primitive kinds, positions, sizes, order,
and RGB values are learner-owned. Keep every primitive's nominal bounding box
in bounds after resize. The public geometry interface, fixture viewports,
deterministic edge policy,
Linux/macOS/Windows commands, starter, tests, and explained reference solution
are all in that bundle. There is intentionally no target screenshot.

## Check your work

Run the pure C++17 suite separately from the native build:

```sh
tests/run-section-00-tests.sh
```

Or in Windows Developer PowerShell:

```powershell
.\tests\run-section-00-tests.ps1
```

The known helper case checks normalized-to-pixel mapping with approximate
floating-point comparisons and verifies that palette indices are not wrapped.
The boundary helper case checks clamping, non-finite fields, and non-positive
viewport policy. Public learner-contract checks compile the starter's
`signature_design.cpp`, require exactly five finite normalized specifications,
three distinct in-range palette colors, valid kinds/channels/indices, and build
nominal geometry across every explicit fixture viewport. They do not model the
extra pixels added by line width, compare pixels, antialiasing, or aesthetic
choices. Finally inspect the window manually at 800 × 600 and after a narrow
and wide resize, including whether thick strokes appear clipped.

## Tell the story

Save one PNG capture and provide alt text that names shapes and spatial
relationships, not only colors. In 80–120 words, connect one normalized
coordinate to its pixel location, explain why geometry lives outside `draw()`,
and identify two composition choices that are yours. Include the first useful
compiler diagnostic you repaired.

## Make it yours

Keep the same five specifications and three colors, but change one spatial
rule: mirror x positions, exchange the two rows, or make all centers follow a
diagonal. Predict which pixel coordinates change before editing. This must
change geometry or placement, not only palette.

## Quick visual check

- The first-frame relationship is legible without animation or input.
- The app builds before it is run; tests keep nominal geometry in bounds, and a
  person checks stroke appearance after resize.
- Exactly five primitives and three palette entries remain inspectable in code.
- Shape or placement provides a non-color cue.
- Contrast supports the stated viewing context.
- The result differs from the starter, preview, and solution in geometry or
  spatial logic, not merely color.
- The capture has useful alt text; code/assets are credited and licensed.

## If you get stuck

That is normal; computers are very confident about tiny mistakes. Read the
first useful error, compare your code with the nearest example, and rerun the
smallest check before changing three things at once. If setup gets noisy, save
the command and error text, then come back to it after a short break. Your
visual choices are the point of the exercise—the reference is a guide, not a
personality transplant.
