# Section 15 reference solution: voice-woven horizon

## What this example tries

This answer is visually unlike the starter's radial sun. Amplitude widens a horizontal
horizon, increases a row of outlined diamonds, enlarges each diamond, and lifts them
into a two-cycle wave. A baseline and arrow preserve extent and direction cues without
color. M collapses density while the extent and source labels remain readable.

## Response decisions

Smoothing 0.22 responds more slowly than the starter's 0.5. A 0.06 dead zone admits
softer values. Radius maps from 34 to 210 and repeated diamonds from 6 to 48, remaining
below the shared core's 128-mark cap. The dark navy field, blue quiet marks, and amber
active marks have different lightness; outline count, extent, meter-equivalent
baseline/arrow, and text duplicate their meaning.

The adapter smooths before applying the quiet floor. Above that floor it maps
`(smoothed - dead_zone) / (1 - dead_zone)` into a 0-to-1 level. Radius blends
between its minimum and maximum. The diamond count rounds with
`floor(value + 0.5)`. Bad values are rejected, values above one stop at one, and
all loops have limits. Recorded replay starts from zero. Only one loudness
number crosses the live callback boundary; no audio or loudness history is
saved.

The app starts in keyboard fallback. F and Up/Down prove the complete mapping without a
device. L is opt-in after nearby consent and OS default-device selection; setup failure
returns to fallback. N/exit stops the stream. The label makes active source visible and
sound/color are not sole cues.

## Build and launch

Linux x86-64:

```sh
scripts/section-15.sh generate --project solution
scripts/section-15.sh build --project solution --configuration Release
exercises/15-embodied-audio-input/solution/bin/solution
```

macOS arm64:

```sh
scripts/section-15.sh generate --project solution
scripts/section-15.sh build --project solution --configuration Release
open exercises/15-embodied-audio-input/solution/bin/solution.app
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\scripts\section-15.ps1 generate -Project solution
.\scripts\section-15.ps1 build -Project solution -Configuration Release
& .\exercises\15-embodied-audio-input\solution\bin\solution.exe
```

The course supplies checked openFrameworks 0.12.1 commands for those systems. On
another system, you may need to adapt the build steps. After compiling, open the app and
try N/F/Up/Down first. Check the picture, permissions, device selection, and clarity of
the input mapping yourself; live L is optional.
