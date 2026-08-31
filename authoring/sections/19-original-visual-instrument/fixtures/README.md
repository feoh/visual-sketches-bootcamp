# Section 19 replay test fixtures

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course test data. License: CC0-1.0; see
`../../../../LICENSES/CC0-1.0.txt`. Malformed input must fail before changing
adapter/model state or indexing a row.

The fixtures are UTF-8 tab-separated text with one header row. Parse field
names, not column positions. Reject missing or duplicate headers, extra fields,
non-finite numeric values, decreasing replay ticks, and unsupported events with
an actionable status. Rejection is transactional: the last valid model and
adapter state remains intact. `NA` means the field is intentionally absent, not
zero.

## `replay-events.tsv`

Start with no viewport and no pointer state. Reset records seed 4242. A viewport
event changes the normalization dimensions. Normalize pointer pixels as
`clamp(pixel / dimension, 0, 1)`. The unknown event is rejected without moving
the pointer or replay cursor. After the adapter fixture passes, feed accepted
normalized events into your model twice and compare declared state/geometry
checkpoints.

## `frame-times.tsv`

Reset accumulator and step count at each new partition. Use fixed step `0.01`
seconds, maximum accepted frame delta `0.05` seconds, and at most five catch-up
steps per frame. Compare accumulator values with absolute tolerance `0.000001`.
Partitions A and B represent equal elapsed accepted time and both produce ten
steps. `SPIKE` proves `0.2` is clamped before stepping and produces five steps.
Keep this small common check even if your project uses different production constants.
Add a project-specific case separately when your values differ.

## `failure-cases.tsv`

Each row suggests a failure worth trying. Replace placeholder asset or device input with
a case that matters to your sketch; skip only `missing_asset` or `missing_device` when
that input type does not exist. Keep the malformed-replay, tiny-viewport, and
nonfinite-time cases for every project. Show a short, visible status instead of crashing
or silently continuing.
