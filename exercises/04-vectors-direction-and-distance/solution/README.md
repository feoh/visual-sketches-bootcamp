# Explained solution

## What this example tries

This version turns the traveler into the point of a triangular constellation. Two side
marks use the perpendicular direction `(-unit.y, unit.x)`, which is a quarter-turn from the
main arrow. Four marks along the connector make its direction visible.

The side
spread grows with distance but stops at 24 pixels. Every center stays at least 12 pixels
from the window edge. The largest target reaches 10 of those pixels after its outline is
included, leaving a 2-pixel margin.
