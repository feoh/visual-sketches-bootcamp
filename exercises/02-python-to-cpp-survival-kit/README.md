# Exercise 02: a parameterized family of marks

Build one visual family from a small parameter record and a returned `std::vector` of mark values. The deterministic model is renderer-independent; the openFrameworks adapter draws the values.

## Learner-owned choices

Edit `starter/src/design/family_design.cpp`. Choose 3–24 marks, normalized center coordinates from 0.1–0.9, spacing from 8–80 pixels, base radius from 3–40 pixels, radius step from -1–3 pixels, and three RGB colors. Then replace the starter's circles with a recognizably different repeated silhouette, interior cue, or non-flashing connection rule. The starter is a circle with a vertical tick. The explained solution is alternating-color linked hourglasses. Differ from both in geometry or mapping, not palette alone. There is no target screenshot and no pixel gate.

## Test and build

```sh
tests/run-section-02-tests.sh
scripts/section-02.sh generate --project starter
scripts/section-02.sh build --project starter --configuration Release
```

On Windows Developer PowerShell:

```powershell
.\tests\run-section-02-tests.ps1
.\scripts\section-02.ps1 generate -Project starter
.\scripts\section-02.ps1 build -Project starter -Configuration Release
```

Project Generator 0.103.0 owns generated metadata. Native support requires passing `section-02-linux-build`, `section-02-macos-build`, and `section-02-windows-build` on the commit; compilation does not prove graphical runtime or visual quality.

## Public contract

Keep `makeFamilyDesign()` and declarations in `shared/mark_family.h`. Tests compile the starter design—not the solution—and parse every fixture field, including independent expected first/middle/last center, radius, and index values. They check those known oracles, collection size, finite/in-bounds marks, repeat determinism, one-parameter-at-a-time variation, invalid input, a constrained viewport, and returned-value independence. A viewport with either dimension below 2 pixels returns an empty family because it cannot contain the minimum 1-pixel radius. They do not inspect source text, pixels, or resemblance.

## Manual accessibility and originality review

- Every repeated mark is legible at narrow and wide window sizes.
- Shape, stroke, placement, or labels carry the family pattern without color alone.
- Mark/background contrast is suitable; no flashing or rapid full-field changes occur.
- Your geometry or parameter mapping differs from starter and solution, not only palette.
- Capture alt text states count, repeated shape, size/placement pattern, and relationship.
- Reused code and assets are credited and license-compatible.
