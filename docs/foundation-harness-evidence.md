---
title: Foundation harness validation evidence
weight: 38
draft: false
---

# Foundation harness validation evidence

This ledger separates implementation-time validation from support claims. A
working-tree result without a repository commit and retained native CI logs is
not `build-proven` or `unit-proven` evidence under the platform matrix.

## 2026-08-18 — Linux pre-commit harness validation

The uncommitted foundation working tree was mounted at `/work` in the selected
`ubuntu:24.04` container base (`x86_64`). The freshly checksum-verified official
openFrameworks 0.12.1 Linux release was mounted separately at `/opt/of`; its
upstream Ubuntu dependency script installed the native build dependencies.

The following gates passed:

```text
tests/run-core-tests.sh
scripts/foundation.sh doctor
scripts/foundation.sh generate --project all
scripts/foundation.sh build --project all --configuration Release
scripts/foundation.sh test --project unit --configuration Release
scripts/foundation.sh build --project all --configuration Debug
scripts/foundation.sh test --project unit --configuration Debug
```

Observed results:

- the pure course core compiled with `-std=c++17` and exercised deterministic
  replay, approximate diagnostics, and NaN/infinite/negative/zero `dt` policy;
- Project Generator 0.103.0 cleanly generated normal Linux metadata for both
  tracked projects without changing canonical inputs;
- Release and Debug linked both tracked windowed and unit products from the
  shared core;
- both no-window executables returned success with exactly one
  `12/12 tests passed` summary each; and
- a separate setup-wrapper check replaced a deliberately incomplete extracted
  tree only after verifying the Linux archive SHA-256 and validating the fresh
  0.12.1 root.

This is local pre-commit validation only. A subsequent Xvfb smoke launched the
Release windowed executable for five seconds and confirmed that the bundled PPM
asset emitted no load error. That proves process startup and asset lookup in the
Linux container, but it is not a human visual inspection or a pixel baseline and
does not establish graphical-runtime support.

## 2026-08-18 — Cross-generation inspection

The packaged Linux Project Generator was also invoked with the official joined
short options to emit clean `osx` and `vs` metadata in a disposable directory.
Exact course paths, duplicate counts, Xcode source-build references, and Visual
Studio `ClCompile`/`ClInclude` membership were inspected successfully. This is
`cross-generation-observed` only.

No macOS or Windows native generator, compiler, SDK, executable, or graphical
runtime ran in this validation. Those lanes remain `selected` until the six
commit-addressed CI statuses provide native evidence; a future support ledger
must record the exact floating runner image and tool versions printed by each
job.
