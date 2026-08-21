---
title: Cross-platform setup and the first frame
slug: 00-cross-platform-setup-and-first-frame
weight: 10
draft: false
course_kind: instructional
objectives:
  - Verify the pinned openFrameworks and Project Generator versions before changing code
  - Explain the roles of main.cpp, ofApp.h, and ofApp.cpp
  - Place shapes by percentage so they adapt when the window changes size
  - Build a first frame from color and basic shape-drawing calls
  - Use the first useful compiler error message to repair one syntax error
prerequisites:
  - Comfort editing plain text files and running commands in a terminal
  - A supported Linux, macOS, or Windows desktop; the exact automated test systems are listed below
source_records: sources.yaml
asset_records: assets.yaml
---

# Cross-platform setup and the first frame

## See what we're making

This image is a single still frame: one background and five drawn shapes. It shows the
few rules you will work with, but you do not need to copy its layout.

![Navy rectangle, coral circle, yellow triangle and diagonal, and navy ellipse form two uneven rows on a warm ground.](media/five-primitive-preview.svg "The example uses exactly five basic shapes and three colors; their shapes and positions still distinguish them without color.")

*The example uses exactly five basic shapes and three colors; their shapes and positions still distinguish them without color.*

Nothing moves or makes sound yet. The image description and caption explain the layout
without relying only on color.

## Take a guess

Without running this fragment, sketch the location of the circle and name which
statement changes later drawing calls:

```cpp
void ofApp::draw() {
    ofSetColor(238, 91, 71);
    ofDrawCircle(200, 150, 40);
}
```

Is `(200, 150)` measured from the lower-left, center, or upper-left? Is `40`
a diameter or radius? Keep your prediction; verify it during Build.

## Let's unpack it

### Install one pinned toolchain

Setup is the least glamorous part, so here is the short version: use the block for your
operating system and let the course scripts check the details. The automated builds
cover Ubuntu 24.04, Apple Silicon macOS 15, and Windows with Visual Studio 2022.
CachyOS/Arch changes frequently, so the repository records the package versions that
were tested rather than promising that every future update will behave exactly the same.
Windows 11 will probably work, but it has not been checked yet.

Everyone uses openFrameworks 0.12.1 and Project Generator 0.103.0. The scripts download
and check those exact versions. The upstream [setup guides](https://openframeworks.cc/setup/) are useful background, but
use the course commands below rather than running an upstream Linux installer yourself.
The course script knows whether it should use Ubuntu's `apt` or CachyOS/Arch's
`pacman` and `paru`.

Ubuntu or CachyOS from the repository root:

```sh
scripts/setup-of.sh --platform linux64 --destination "$HOME/openframeworks"
export OF_ROOT="$HOME/openframeworks/of_v0.12.1_linux64_gcc6_release"
scripts/setup-linux.sh install --of-root "$OF_ROOT"
scripts/foundation.sh doctor
```

On CachyOS/Arch, the setup script also handles a few compatibility fixes for you: it
skips an OpenCV step this course does not need, rebuilds Project Generator for the
libraries on your machine, and makes openFrameworks use its working X11 path. You do not
need to perform those fixes by hand. They only change the downloaded openFrameworks
folder stored in `OF_ROOT`. If you run `setup-of.sh` again, run `setup-linux.sh prepare` or
`install` again too.

macOS from the repository root:

```sh
scripts/setup-of.sh --platform osx --destination "$HOME/openframeworks"
export OF_ROOT="$HOME/openframeworks/of_v0.12.1_osx_release"
scripts/foundation.sh doctor
```

Windows Developer PowerShell from the repository root, after installing the Visual
Studio 2022 **Desktop development with C++** workload and the selected Windows SDK:

```powershell
.\scripts\setup-of.ps1 -Destination "$HOME\openframeworks"
$env:OF_ROOT = "$HOME\openframeworks\of_v0.12.1_vs_64_release"
.\scripts\foundation.ps1 doctor
```

A successful doctor prints `openFrameworks=0.12.1` and Project Generator `0.103.0`. Stop on
any error; do not patch a generated project to get around a wrong root or version.

### Project Generator and the three source roles

Project Generator creates the Make, Xcode, Visual Studio, or VS Code project files
needed on your machine. It reads the source code and add-on list already in the
repository. The official [Project Generator documentation](https://github.com/openframeworks/openFrameworks/blob/0.12.1/docs/projectgenerator.md) has the details. Treat generated project files
as disposable: regenerate them when needed instead of editing or committing them.

- `main.cpp` creates an 800 × 600 window, hands it an `ofApp`, and starts the
  main loop.
- `ofApp.h` lists the functions and saved values your app has. Think of this header
  as a table of contents that other source files can include.
- `ofApp.cpp` contains the code inside those functions. `ofApp::` simply says
  that a function belongs to `ofApp`.

When you build, the compiler checks and translates your C++ files, then the linker joins
them with openFrameworks. When you run, the finished program opens. A successful build
does not prove that the picture looks right, so you still need to open the window. The
[ofBaseApp reference](https://openframeworks.cc/documentation/application/ofBaseApp.html) lists the available functions. For now, remember only this:
`setup()` runs once when the app starts, and `draw()` runs again for every
frame.

### Place shapes with pixels and percentages

```text
(0,0) ───────────────► x moves right
  │
  │       (200,150)
  │
  ▼
 y moves down
```

The upper-left corner is `(0, 0)`. The first number is how many pixels to move
right; the second is how many pixels to move down. So `(200, 150)` means “start at the
upper-left, move 200 pixels right, then 150 pixels down.” In an 800 × 600 window, that
point is one quarter of the way across and one quarter of the way down.

Hard-coded pixel positions stop being useful when the window changes size. The starter
therefore writes positions as percentages, using decimals from `0.0` to
`1.0`. The code calls these values **normalized**, but there is no special math
hiding behind that word:

| Value | Meaning |
| --- | --- |
| `0.0` | at the left or top |
| `0.25` | 25% of the way across or down |
| `0.5` | halfway across or down |
| `1.0` | at the right or bottom |

To turn a percentage into pixels, multiply it by the window size. In an 800 × 600
window, a center of `(0.25, 0.25)` becomes `(200, 150)` because `0.25 × 800 = 200` and
`0.25 × 600 = 150`.

Sizes work the same way. `normalized_size = (0.10, 0.20)` means “10% of the window wide and 20% of the
window tall.” At 800 × 600, that is an `80 × 120` pixel shape.

You will see `half_width` and `half_height` in the supplied code. They are just half
of the shape's full size: an `80 × 120` shape has a half-width of `40` and
a half-height of `60`. Keeping those halves makes the edge check easy:

```text
left edge   = center x - 40       right edge  = center x + 40
top edge    = center y - 60       bottom edge = center y + 60
```

If all four edges stay inside the window, the shape fits. The helper code does this
arithmetic so the tests can catch an off-screen shape without opening a graphics window.

### Color and shapes

`ofSetBackgroundColor(red, green, blue)` chooses the background color. `ofSetColor(...)` chooses the color for
whatever you draw next; it does not draw anything by itself. Calls such as
`ofDrawCircle`, `ofDrawRectangle`, `ofDrawTriangle`, `ofDrawLine`, and `ofDrawEllipse` draw
the actual shapes. Each red, green, or blue number can range from 0 through 255. Keep
the official [ofGraphics reference](https://openframeworks.cc/documentation/graphics/ofGraphics/) nearby when you want to look up a drawing function.

You will also see arrays and named shape choices in the supplied code. You do not need
to understand all of their C++ syntax yet. In this lesson, focus on changing the five
shapes, their positions and sizes, and the three colors.

## Make it run

Run the setup, build, and app as separate steps. That way, when something goes wrong,
you know which step produced the error.

### Example 1: the proven first window

Start with the known-good sample in [`foundation/windowed/src/ofApp.cpp`](../../../foundation/windowed/src/ofApp.cpp). Generate its project files and build
it:

```sh
scripts/foundation.sh generate --project windowed
scripts/foundation.sh build --project windowed --configuration Release
```

On Windows use:

```powershell
.\scripts\foundation.ps1 generate -Project windowed
.\scripts\foundation.ps1 build -Project windowed -Configuration Release
```

Then open the app from `foundation/windowed/bin/`. Building proves that the code compiled; opening
it lets you check the picture with your own eyes.

### Example 2: one shape

In a generated scratch copy, these definitions clear once and draw once per frame:

```cpp
void ofApp::setup() {
    ofSetBackgroundColor(248, 245, 236);
}

void ofApp::draw() {
    ofSetColor(238, 91, 71);
    ofDrawCircle(200.0f, 150.0f, 40.0f);
}
```

Confirm your Predict answer: the center is 200 pixels right and 150 pixels down from the
upper-left; 40 is the radius.

### Example 3: the exercise starter

The complete [`starter`](../../../exercises/00-visual-signature/starter/) stores five shape choices in one small design file. Other
supplied code turns those choices into pixels and draws them:

```sh
scripts/section-00.sh generate --project starter
scripts/section-00.sh build --project starter --configuration Release
tests/run-section-00-tests.sh
```

The equivalent Windows commands are in the exercise brief. Open the app only after
generation and compilation pass.

## Break it on purpose

Open `exercises/00-visual-signature/starter/src/ofApp.cpp` and delete the semicolon after one `ofSetColor(...)` call. Build again.
Compiler wording varies, but find the **first error message** that names your file and
line. A useful GCC/Clang form is:

```text
error: expected ';' after expression
```

Ignore later cascading messages until that first local error is repaired. Read the named
line and the line immediately before it, restore the semicolon, save, and build again.
Do not regenerate: source syntax changed, not project membership. If this was the only
intentional change to that file, you can restore the repository copy with
`git restore -- exercises/00-visual-signature/starter/src/ofApp.cpp`; this command discards every uncommitted edit in that file. In one
sentence, write down the error message, what caused it, and how you fixed it.

## Your turn

Create a visual signature from exactly five shapes and three colors. Start with the
[exercise brief](../../../exercises/00-visual-signature/README.md), then edit `starter/src/design/signature_design.cpp`. You choose the shape types, their positions
and sizes, which ones are drawn first, and the three colors. Keep each shape inside the
window when you resize it.

The exercise folder contains the commands, starter, tests, and one example solution.
There is deliberately no picture you must reproduce. The goal is to make your own small
picture while staying within the five-shape, three-color rules.

## Check your work

Run the pure C++17 suite separately from the native build:

```sh
tests/run-section-00-tests.sh
```

Or in Windows Developer PowerShell:

```powershell
.\tests\run-section-00-tests.ps1
```

The tests check the parts a computer can judge reliably:

- there are exactly five shapes and three usable colors;
- percentage positions and sizes turn into the expected pixel values;
- shapes stay inside several window sizes; and
- color and shape choices use valid numbers.

The tests do **not** decide whether your picture looks good. They also do not check
every extra pixel from a thick outline. Open the app at 800 × 600, then make the window
narrow and wide. Look for clipped shapes or lines yourself.

## Optional notes for future you

Save one PNG and write alt text that describes the shapes and where they are, not only
their colors. Pick one percentage position and show what pixel position it became, then
name two visual choices you made. Also include the first useful compiler error you
fixed.

## Make it yours

Keep the same five shapes and three colors, but change one layout rule. You could mirror
the picture left-to-right, swap its two rows, or put every center on a diagonal. Before
editing, guess which pixel positions will change. Change the shapes or layout, not just
the colors.

## Quick visual check

- The picture makes sense as a still image; it does not need motion or input.
- The app builds, the tests pass, and no shape looks clipped after a resize.
- The design file still contains exactly five shapes and three colors.
- Shape or placement—not only color—helps distinguish the forms.
- The shapes stand out clearly from the background.
- Your layout differs from the starter, preview, and solution in more than its colors.
- Your saved image has useful alt text.

## If you get stuck

That is normal; computers are very confident about tiny mistakes. Read the first useful
error, compare your code with the nearest example, and rerun the smallest check before
changing three things at once. If setup gets noisy, save the command and error text,
then come back to it after a short break. Your visual choices are the point of the
exercise—the example is a guide, not a personality transplant.
