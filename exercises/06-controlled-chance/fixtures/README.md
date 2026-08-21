# Portable edition fixture

`portable-edition.txt` is a hand-authored `CONTROLLED_CHANCE_V1` parameter set,
not output copied from `generateEditions()`. It fixes seed metadata, six ordered
editions, four marks per edition, normalized positions, radii, and motif codes.
Tests parse semantic fields, render them without invoking a random distribution,
and require a stable serialize/parse canonical round trip.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid test input.
