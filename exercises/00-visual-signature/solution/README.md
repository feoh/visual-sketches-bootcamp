# Explained reference solution

This is one answer, not the intended visual target. `signature_design.cpp` uses
five different primitive kinds and distributes them across two uneven rows.
Three colors repeat by index, while changes in silhouette and position carry
meaning without color.

The normalized specifications remain independent of window pixels. The shared
`buildSignature` function converts them only after receiving the current
viewport, clamps each bounding box, and returns renderer-inspectable geometry.
`ofApp` then selects a drawing call by `PrimitiveKind`; it owns openFrameworks
rendering but none of the coordinate policy tested by the standard-library
suite.

A learner solution may repeat kinds, use a symmetric layout, overlap forms, or
choose a radically different palette. It is correct when it retains exactly
five primitives, exactly three palette entries, passes the geometry tests, and
meets the manual authorship checklist.
