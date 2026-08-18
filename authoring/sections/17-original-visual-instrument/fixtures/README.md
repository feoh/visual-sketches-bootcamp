# Section 17 evaluator fixture contract

Creator: Visual Sketches Bootcamp contributors. Origin: original, hand-authored
course evaluator data. License: CC0-1.0; see
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
normalized events into the learner's model twice and compare declared
state/geometry checkpoints.

## `frame-times.tsv`

Reset accumulator and step count at each new partition. Use fixed step `0.01`
seconds, maximum accepted frame delta `0.05` seconds, and at most five catch-up
steps per frame. Compare accumulator values with absolute tolerance `0.000001`.
Partitions A and B represent equal elapsed accepted time and both produce ten
steps. `SPIKE` proves `0.2` is clamped before stepping and produces five steps.
A project may add different production constants but must retain this fixture
as a small accumulator-unit contract.

## `failure-cases.tsv`

Each row specifies an abstract failure and observable minimum outcome. Replace
placeholder input with a project-relevant path, adapter, or parser case. If a
capstone truly has no asset or device adapter, report `NOT_APPLICABLE` and the
architectural reason; do not claim a fake passing exercise. Status text must be
inspectable and visible in the graphical adapter.
