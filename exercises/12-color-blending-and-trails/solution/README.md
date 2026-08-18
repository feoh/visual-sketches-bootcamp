# Section 12 reference solution: additive star loom

This answer deliberately diverges from the starter. Two mirrored three-lobed
rose paths weave around an off-center origin on a dark field. Palette A uses
rotating diagonal crosses from blue to mint; palette B uses open diamonds from
violet to amber. Every third sample becomes a sparse spoke-like mark. Additive
blending makes overlap luminous, while shape carries palette identity and mark
size/spacing supports age.

## Why these values

The 110-sample cap gives longer persistence than the starter without exceeding
the model's 4,096-sample absolute bound. Retention `0.965` loses 3.5 percent per
sample; base opacity `0.48` leaves headroom for additive overlap. Radii 1 to 7
keep old marks fine and new marks legible. This combination changes history,
decay, opacity, size, palette, trajectory, density, blend mode, and geometry—not
just hue.

For each endpoint pair, every straight RGBA channel uses
`first + (second - first) * amount`. Trail age is
`history.size() - 1 - index`, so the newest mark has age zero. Alpha is palette
alpha times base opacity times `retention^age`. The pure plan validates all
samples and commits output only after every mark remains finite and bounded.
It has no global clock, random source, openFrameworks color, or renderer state.
The adapter owns motion and drawing only.

The draw block brackets additive blend mode and style with push/disable/pop;
each rotated shape also pairs matrix push/pop. Help text begins a separate style
scope. Cross and diamond roles, plus the square origin, remain readable without
color. M leaves current marks rather than animating long histories.

## Build and launch

Supported native lanes are Linux x86-64, macOS arm64, and Windows Visual Studio
2022 x64 Developer PowerShell. Other architectures are unattested and
unsupported by these wrappers.

Linux x86-64:

```sh
scripts/section-12.sh generate --project solution
scripts/section-12.sh build --project solution --configuration Release
exercises/12-color-blending-and-trails/solution/bin/solution
```

macOS arm64:

```sh
scripts/section-12.sh generate --project solution
scripts/section-12.sh build --project solution --configuration Release
open exercises/12-color-blending-and-trails/solution/bin/solution.app
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\scripts\section-12.ps1 generate -Project solution
.\scripts\section-12.ps1 build -Project solution -Configuration Release
& .\exercises\12-color-blending-and-trails\solution\bin\solution.exe
```

Manually launch both projects and record alpha/additive appearance, both input
routes, pause/reset/reduced motion, resize/tiny windows, contrast, state
restoration, and visual divergence. Compilation and model tests do not prove
those pixel claims.
