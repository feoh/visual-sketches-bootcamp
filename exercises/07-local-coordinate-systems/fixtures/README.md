# Transformed-anchor fixture

These three rows were hand-calculated from the documented parent-child chain,
not emitted by `makeScene`: viewport translation, responsive uniform scale,
parent rotation, first-arm translation, then child rotation. Tests parse the
file strictly and compare pivot, elbow, and tip anchors at three phases.
