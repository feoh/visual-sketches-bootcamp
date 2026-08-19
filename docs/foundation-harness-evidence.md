---
title: Foundation harness validation evidence
weight: 38
draft: false
---

# Foundation harness validation evidence

This ledger separates implementation-time validation from support claims. A
working-tree result without a repository commit and retained native CI logs is
not `build-proven` or `unit-proven` evidence under the platform matrix.

## 2026-08-18 — Three-platform commit-addressed proof

GitHub Actions run [32094195792] validated commit
[`f3c42f4`](https://github.com/feoh/visual-sketches-bootcamp/commit/f3c42f4a2eaa990a2a963d57d67f74a53d402c8b) with all six required statuses:

- `linux-build` and `linux-unit` on `ubuntu24` image
  `20260810.271.1`, Ubuntu 24.04 x86-64, GCC/G++ 13.3.0;
- `macos-build` and `macos-unit` on `macos15` image
  `20260727.0256.1`, macOS 15.7.7 arm64, Xcode 16.4, SDK 15.5, and Apple
  Clang 17.0.0; and
- `windows-build` and `windows-unit` on `win22` image
  `20260802.262.1`, Windows Server build 20348.5386, Visual Studio
  17.14.37516.0/MSBuild 17.14.51, toolset v143, and SDK 10.0.26100.0.

Every build status checksum-verified a fresh official openFrameworks 0.12.1
archive, ran doctor and clean Project Generator 0.103.0 generation, built both
tracked projects in Debug and Release, and verified tracked inputs were
unchanged. Every unit status independently repeated setup and generation, built
the Release unit product, executed it from `bin`, observed process status zero,
and required exactly one `12/12 tests passed` summary. The successful run is
`build-proven` and `unit-proven` evidence for all three selected lanes.

No CI job launched the windowed product or compared rendered pixels. Graphical
runtime and visual appearance therefore remain separate manual evidence, and
the render-baseline policy remains deliberately tolerant and human-reviewed.

[32094195792]: https://github.com/feoh/visual-sketches-bootcamp/actions/runs/32094195792

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
