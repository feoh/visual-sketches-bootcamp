# Exercise 16: constrained luminous composition

Complete the [Lesson and Practice](../../authoring/sections/16-structured-chance/index.md#lesson)
before starting. This page is the authoritative exercise brief.

## Build one related family

Generate one shared warped grid, then create lines at three scales of choice:
composition, region, and mark. Keep adjacent corners exact, preserve one quiet
region, and draw a non-glow structure before judging the translucent treatment.

Edit `starter/src/design/structured_chance_design.cpp` for seed, rows, columns,
node jitter, dominant angle, angle jitter, density range, quiet density, and
palette. Edit `starter/src/ofApp.cpp` for line grammar, layered widths, seams,
composition, and help presentation. Keep declarations in the existing design
and shared files; wrappers reject additional source files.

## Fixed behavior

- 2–8 rows and columns, at most 64 regions;
- one `(rows + 1) × (columns + 1)` shared node lattice;
- boundary nodes on the frame and interior jitter no greater than `0.09`;
- positive, non-folded quadrilateral regions;
- one quiet region and no more than 64 strokes per region;
- finite lines clipped in local 0-to-1 coordinates before panel mapping;
- regional angles inherited from one dominant composition angle;
- `R` replays, `N` changes seed, `G` toggles soft layers, and `H` toggles help.

Changing only palette values is not enough. Change at least grid proportions,
density rhythm, angle relationship, and mark or seam grammar.

## Tests and builds

Linux or macOS:

```sh
CXX=g++ tests/run-section-16-tests.sh
CXX=clang++ tests/run-section-16-tests.sh
scripts/section-16.sh generate --project starter
scripts/section-16.sh build --project starter --configuration Release
```

Windows Developer PowerShell:

```powershell
.\tests\run-section-16-tests.ps1
.\scripts\section-16.ps1 generate -Project starter
.\scripts\section-16.ps1 build -Project starter -Configuration Release
```

Tests check deterministic records, exact neighbor seams, convex area, inherited
angle bounds, line clipping, one quiet region, invalid settings, and work caps.
Open the app to review glow, contrast, seams, resize behavior, controls, and
whether 20 seeds remain one authored family.

## Reference solution

The [explained solution](solution/README.md) uses a taller 4 × 3 grid, fewer
marks, wider dark seams, and a more vertical amber/violet family. It is one
answer, not a target image.
