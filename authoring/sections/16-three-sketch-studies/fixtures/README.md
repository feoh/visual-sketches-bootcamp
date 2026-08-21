# Section 16 study test fixtures

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course test data. License: CC0-1.0; see
`../../../../LICENSES/CC0-1.0.txt`. Malformed input must fail before changing
model state or indexing a row.

These UTF-8, tab-separated fixtures use a header row, decimal points, `1` for
true, and `0` for false. Your fixture reader must reject missing columns, extra columns,
non-finite numbers, non-positive capacities, and non-positive time steps with a
clear diagnostic. Rows are sequential within a file; reset model state before
the first data row. Do not use wall-clock time, framework randomness, or
rendered pixels.

- `gesture-memory.tsv` starts with empty memory. Its first valid candidate is
  accepted. Later candidates are measured from the newest accepted point.
- `repetition-transform.tsv` treats every row independently. Angles are degrees;
  rotation around local origin occurs before translation.
- `repetition-oscillation.tsv` treats every row independently. Phase is radians;
  offset is `amplitude * sin(phase)`.
- `particle-step.tsv` starts at zero position and velocity. Rows use
  semi-implicit Euler and accumulate sequentially.

Compare fixture floats with absolute tolerance `0.0001`. You may add cases or a fixture for a different documented fixed step; preserve these rows
as shared compatibility checks.
