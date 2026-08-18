# Section 11 independent oracle

`flow-oracle.txt` is plain text so the test executable does not manufacture its
own expected values. Three rows pin the credited, course-pinned 32-bit lattice hash for
positive, zero, and negative coordinates. A fourth row hand-calculates the
center of a 2-by-2 bilinear grid whose values are 0, 1, 1, 0. The final row maps
scalar 0.25 to angle pi/2 and direction (0, 1).

The exact integer hash is part of this section's pinned implementation. Derived
floating-point noise, interpolation, and vectors are compared with explicit
tolerances; the fixture does not promise byte-identical rendering or results
under arbitrary compilers and floating-point modes.

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course fixture data, independently calculated where the description says so.
License: CC0-1.0. The public test rejects a missing file, malformed row, wrong
field count, or incomplete fixture before treating its values as valid evidence.
