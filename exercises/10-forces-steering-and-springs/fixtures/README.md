# Section 10 fixtures

`spring-oracle.txt` is an independent hand-calculated oracle. It uses a spring
along +x with rest length 10, stiffness 2, and damping 1. At distance 10 the
force is zero. The model asks for force on the first endpoint and defines its
axis toward the second, so at distance 20 its elastic force along that axis is
`2 * (20 - 10) = 20`; the second receives -20. This is the endpoint-axis form
of conventional restoring notation `F = -k(x - L)`. When the second endpoint
moves toward the first at 10 units/second, the dot
product of relative velocity with the +x spring axis is -10, so damping reduces
the force to `20 + 1 * -10 = 10`.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid evidence.
