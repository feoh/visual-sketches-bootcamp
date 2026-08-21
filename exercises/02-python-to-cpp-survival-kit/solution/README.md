# Explained reference solution

## What this example tries

The reference chooses eleven marks with size increasing away from the center.
`makeMarkFamily()` still owns repeatable placement; `ofApp::draw()` iterates over returned
values. Each value becomes two tip-to-tip triangles, and a separate indexed loop links
adjacent centers. Alternating shape fill plus the continuous line makes the grouping
readable without color alone. This linked-hourglass treatment intentionally differs from
the starter's isolated circle and vertical tick; learners should choose a third
treatment.
