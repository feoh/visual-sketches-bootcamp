`family-cases.tsv` supplies named viewport and parameter records plus independent expected first, middle, and last center/radius/index values. Public tests parse every column, compare those numerical known oracles, build each family twice, and check size, finiteness, bounds, and determinism without inspecting rendered pixels.

A viewport narrower than 2 pixels in either dimension cannot contain the model's minimum 1-pixel radius. The model therefore returns an empty family for `1xN`, `Nx1`, and `1x1` viewports; the boundary tests enforce that policy.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid evidence.
