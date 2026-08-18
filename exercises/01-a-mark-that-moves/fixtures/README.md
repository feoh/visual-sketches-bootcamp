# Section 01 deterministic fixtures

`frame-streams.tsv` supplies the public C++ test's named partition, pause-spike, and boundary scenarios. Each data row has exactly three consumed columns: scenario name, frame count, and duration per frame in seconds (not milliseconds). Lines beginning with `#` are comments.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid evidence.
