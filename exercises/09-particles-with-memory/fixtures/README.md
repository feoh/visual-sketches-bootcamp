# Independent oracle

These values use `x=10`, `vx=10`, zero acceleration, drag rate `1`, and two
semi-implicit 0.25-second steps: `v *= exp(-dt)` then `x += v * dt`. They were
calculated independently from the application and are strictly parsed by tests.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid test input.
