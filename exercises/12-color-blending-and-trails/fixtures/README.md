# Independent color/trail oracle

`trail-oracle.txt` is a small human-computable fixture. It pins three palette
samples, straight-alpha source-over red on blue, and two decay steps. The C++
suite parses the file and separately checks malformed row cardinality, so the
fixture gives the tests expected answers that do not come from the model's own control flow.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid test input.
