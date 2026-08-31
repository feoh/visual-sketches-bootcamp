# Exercise 17: clustered two-light population

Complete the [Lesson and Practice](../../authoring/sections/17-depth-light-and-populations/index.md#lesson)
before starting. This page is the authoritative exercise brief.

## Build a bounded mass

Generate a small set of cluster parents before generating child forms. Every
child inherits neighborhood, preferred orientation, typical scale, and material
role. Render one reusable rounded primitive with depth testing, a camera, and two
restrained lights.

Edit `starter/src/design/population_design.cpp` for seed, cluster/population
count, volume, scale range, spread, and warm/cool palette. Edit
`starter/src/ofApp.cpp` for primitive proportion, camera, material response,
light placement, density presentation, and help. Keep declarations in the
existing design and shared files; wrappers reject extra source files.

## Fixed behavior

- 2–12 clusters and no more than 2,000 pebbles;
- finite bounded parent centers and center-biased child offsets;
- child rotation within 18 degrees of the parent preference;
- child material role inherited exactly from its parent;
- positive bounded elongated scales;
- depth testing, one reusable rounded primitive, and no custom shader;
- two lights, orbit camera, seed replay, pause, reduced detail, and help;
- `R`, `N`, `P`, `M`, and `H` keyboard controls.

Changing only colors or count is not enough. Change the cluster/void arrangement,
form proportions, orientation rhythm, material response, camera, and lighting.

## Tests and builds

Linux or macOS:

```sh
CXX=g++ tests/run-section-17-tests.sh
CXX=clang++ tests/run-section-17-tests.sh
scripts/section-17.sh generate --project starter
scripts/section-17.sh build --project starter --configuration Release
```

Windows Developer PowerShell:

```powershell
.\tests\run-section-17-tests.ps1
.\scripts\section-17.ps1 generate -Project starter
.\scripts\section-17.ps1 build -Project starter -Configuration Release
```

Tests check deterministic clusters and transforms, inherited rotation and
material, finite bounded positions and scales, invalid settings, and work caps.
Open the app to review depth ordering, silhouette, light, material, camera,
controls, resize behavior, and performance.

## Reference solution

The [explained solution](solution/README.md) uses nine smaller clusters, more
forms, a slow reverse turn, a directional warm light, and a cool point light. It
is one answer, not a target image.
