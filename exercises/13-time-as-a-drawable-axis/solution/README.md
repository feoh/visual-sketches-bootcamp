# Section 13 reference solution: amber time loom

## What this example tries

This answer changes the starter's capacity, slice density, decay, radius, palette, and
complete drawing grammar. It replaces filled light strips with thin stems on a navy
field. Each stem ends at a rotated open diamond whose horizontal position chooses age
and whose rotation shows stored phase. Sparse center dots and NOW/THEN text provide
non-color order cues.

## Why these values

Capacity 180 and 36 slices expose a longer delay than the starter's 120 and 24 without
approaching 65,536 samples or the checked 8 MiB budget. Decay 2.1 fades older stems more
strongly. Ten-pixel diamonds allow dense separation. Amber recent marks and mint distant
marks contrast with navy, but stem order, open shape, ticks, and labels carry meaning
when hue is unavailable.

The renderer asks the model for one sample per normalized horizontal position. It never
indexes physical storage. Ring-buffer wrap therefore changes slots but not the
newest-to-oldest layout rule. The adapter supplies explicit frame and accumulated time,
rejects implausible/`NaN` or infinite deltas, and resets on resize. P pauses, R
restores frame/time zero, and M freezes capture as a still reduced-motion alternative.

The solution intentionally renders directly. Adding an FBO would require manual
allocation, orientation, alpha/clear, resize, GPU-limit, and read/write-target
validation; the pure suite cannot prove those pixels.

## Build and launch

Set `OF_ROOT` to openFrameworks 0.12.1. Linux x86-64 or macOS arm64:

```sh
scripts/section-13.sh generate --project solution
scripts/section-13.sh build --project solution --configuration Release
```

Launch `solution/bin/solution` on Linux or open `solution/bin/solution.app` on macOS. Windows Visual Studio
2022 x64 Developer PowerShell:

```powershell
.\scripts\section-13.ps1 generate -Project solution
.\scripts\section-13.ps1 build -Project solution -Configuration Release
& .\exercises\13-time-as-a-drawable-axis\solution\bin\solution.exe
```

On another system, you may need to adapt the build steps. After compiling, open the app
and use the exercise brief to check the picture, controls, and window-size boundaries.
