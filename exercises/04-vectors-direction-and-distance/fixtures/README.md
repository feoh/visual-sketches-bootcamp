# Section 04 fixtures

`scene-cases.tsv` contains independently calculated numerical oracles for the fixed known design `{anchor_x=0.25, anchor_y=0.35, reach=80}`. Rows cover a 3–4–5 direction, the explicit zero-length normalization guard, and target clamping at the safe viewport boundary. The test parses every field and uses stated absolute/relative tolerances; no rendered pixels are fixtures.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid test input.
