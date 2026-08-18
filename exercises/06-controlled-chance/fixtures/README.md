# Portable edition fixture

`portable-edition.txt` is a hand-authored `CONTROLLED_CHANCE_V1` parameter set,
not output copied from `generateEditions()`. It fixes seed metadata, six ordered
editions, four marks per edition, normalized positions, radii, and motif codes.
Tests parse semantic fields, render them without invoking a random distribution,
and require a stable serialize/parse canonical round trip.
