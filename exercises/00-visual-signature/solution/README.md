# One example solution

## What this example tries

This is one possible answer, not a picture you are expected to copy. `signature_design.cpp` uses
all five shape types and places them in two uneven rows. Its three colors repeat, while
the different shapes and positions keep the forms distinguishable without relying only
on color.

The design file stores positions and sizes as percentages. The supplied `buildSignature`
helper turns them into pixels for the current window and keeps each shape inside its
edges. `ofApp` then chooses the matching openFrameworks drawing function for each
shape.

Your answer may repeat shape types, use a symmetrical layout, overlap shapes, or choose
completely different colors. It is successful when it keeps exactly five shapes and
three colors, passes the geometry tests, stays visible after a resize, and looks like
your own design.
