# Layout fixtures

Each non-comment TSV row contains exactly 11 whitespace-separated fields: case name, viewport width/height, then independent expected panel x/y/width/height, headline size, focus radius, and focus x/y. Values use the test-owned `knownDesign()` with `focus=0.25` and `vertical_bias=0.65`. That oracle input is defined independently in the test and does not come from the learner-editable starter design. Tests parse and compare every field with documented floating-point tolerances.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid evidence.
