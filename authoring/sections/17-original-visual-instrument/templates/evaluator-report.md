# Deterministic evaluator report

## Environment

- Source/archive identifier:
- OS and architecture:
- Compiler and configuration:
- Test executable and exact command:
- Fixture schema/version:

## Core boundary

- Adapter inputs and normalized outputs:
- Pure model inputs and inspectable outputs:
- Renderer-only responsibilities:
- Seed, initial state, fixed step, clamp, and catch-up cap:
- Tolerance and serialization/comparison method:

## Results

| Contract | Fixture/case | Expected | Actual | Pass/fail |
|---|---|---|---|---|
| replay twice | | equivalent checkpoints | | |
| changed seed | | declared variation, stable caps | | |
| resize | | finite policy-specific geometry | | |
| reset | | fresh-instance checkpoint | | |
| malformed input | | transactional rejection | | |
| missing asset/device | | visible fallback or justified N/A | | |
| frame partitions | | equivalent accepted steps | | |
| frame spike | | clamp/cap and visible counter | | |
| deepening-specific case | | | | |

## Commands and output

```text
Paste exact commands and concise unedited summaries here.
```

## Manual evidence not proved by tests

- Graphical launch and mapping legibility:
- Device behavior, if any:
- Renderer/frame observations and measurement method:
- Accessibility observations:
- Known platform and numeric limitations:
