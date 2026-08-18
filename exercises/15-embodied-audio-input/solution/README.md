# Section 15 reference solution: voice-woven horizon

This answer is visually unlike the starter's radial sun. Amplitude widens a
horizontal horizon, increases a row of outlined diamonds, enlarges each diamond,
and lifts them into a two-cycle wave. A baseline and arrow preserve extent and
direction cues without color. M collapses density while the extent and source
labels remain readable.

## Response decisions

Smoothing 0.22 responds more slowly than the starter's 0.5. A 0.06 dead zone
admits softer values. Radius maps from 34 to 210 and repeated diamonds from 6 to
48, remaining below the shared core's 128-mark cap. The dark navy field, blue
quiet marks, and amber active marks have different lightness; outline count,
extent, meter-equivalent baseline/arrow, and text duplicate their meaning.

The adapter smooths before applying the dead zone. Above the boundary it maps
`(smoothed - dead_zone) / (1 - dead_zone)` to normalized level. Radius uses
linear interpolation; repeated count uses `floor(value + 0.5)`. Invalid values
reject, values above one clamp, batches/fixtures/callback loops are capped, and
recorded replay reconstructs state from zero. Only one RMS scalar crosses the
live callback boundary; no audio or amplitude history is retained.

The app starts in keyboard fallback. F and Up/Down prove the complete mapping
without a device. L is opt-in after nearby consent and OS default-device
selection; setup failure returns to fallback. N/exit stops the stream. The label
makes active source visible and sound/color are not sole cues.

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

These wrappers support openFrameworks 0.12.1 on those named lanes; other hosts
are unattested. Unit tests and compilation do not prove app launch, appearance,
permissions, selected-device behavior, microphone success, consent, or
five-second legibility. Inspect N/F/Up/Down first; live L is optional.
