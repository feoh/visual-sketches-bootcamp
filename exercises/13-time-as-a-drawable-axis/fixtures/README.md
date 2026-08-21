# Temporal oracle

`temporal-oracle.txt` is a small independent, hand-checkable fixture. `motion`
rows pin the origin of the explicit frame/time motion function. `selection`
rows pin round-to-nearest mapping from normalized horizontal position to age,
where age zero is newest. The pure C++ test checks row shape before indexing.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid test input.
