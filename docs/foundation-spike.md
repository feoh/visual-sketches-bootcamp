---
title: Foundation spike evidence
weight: 37
draft: false
---

# Foundation spike evidence

This ledger records exploratory evidence used to make the specification
decisions. It is not the implementation-phase support ledger for the future
`foundation/windowed` and `foundation/unit` projects.

## 2026-08-17 — Linux standalone Project Generator spike

### Inputs

| Field | Value |
|---|---|
| Course repository | `feoh/visual-sketches-bootcamp` at `67afda4084e709e13016b41c53e919b35801c68a` before these specification edits |
| Fixture | Disposable `/tmp/pg-spike`; not a tracked course project |
| openFrameworks | Official 0.12.1 Linux x86-64 release archive |
| Archive SHA-256 | `d6c1dcab777665b2aa63e5e3d9122cc116f096b3421db3493f795a621b399c63` |
| openFrameworks source | Tag commit `1019e6a783a309dac8d8d61cb990967bec5ce8f9` |
| Project Generator source | Submodule commit `74afcd48555ba9b3d0c430b2853ddad306a89b63` |
| Project Generator binary | Packaged `projectGenerator/resources/app/app/projectGenerator`, version `0.103.0` |
| Container | `ubuntu:24.04` pulled as `sha256:561618e2c15bf2397621dd04f96926663a3b5616c189cf7e38db7e82f5c538ea` |
| Observed OS after package update | Ubuntu 24.04.4 LTS, x86-64 |
| Observed compiler | GCC/G++ 13.3.0 (`13.3.0-6ubuntu2~24.04.1`) |

The Ubuntu package repositories were not snapshot-pinned. The container digest
pins the base filesystem, not the packages installed by the upstream dependency
script. This evidence establishes an observed compatibility point, not
bit-for-bit reproducibility.

### Fixture

The standalone fixture had this shape:

```text
/work/
├── linux-only/
│   ├── src/main.cpp
│   ├── src/ofApp.cpp
│   ├── src/ofApp.h
│   ├── addons.make
│   └── bin/data/
└── shared/core/
    ├── course_probe.cpp
    └── course_probe.h
```

`OF_ROOT` was mounted separately at `/opt/of`; the project was not under
`/opt/of/apps`.

### Generation command

Inside the container, with the repository-like fixture mounted at `/work`:

```bash
PG=/opt/of/projectGenerator/resources/app/app/projectGenerator
"$PG" \
  -o/opt/of \
  -plinux64 \
  -s../shared/core \
  linux-only
```

Observed output included:

```text
ofPath set and valid using ["/opt/of"]
target platform is: [linux64]
project path is: ["/work/linux-only"]
[addSrcRecursively] using additional source folder ../shared/core
{ "status": "EXIT_OK" }
```

The generated `config.make` contained:

```make
OF_ROOT = /opt/of
PROJECT_EXTERNAL_SOURCE_PATHS = ../shared/core
```

The absolute OF path was machine-local; the course-owned shared source remained
a project-relative path.

### Dependency install and build

The clean Ubuntu container installed the small Project Generator runtime set,
then used the release's official dependency installer before the build:

```bash
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y \
  lsb-release software-properties-common ca-certificates
/opt/of/scripts/linux/ubuntu/install_dependencies.sh -y
cd /work/linux-only
make -j8 Release
```

The build log showed both the project-local source and
`../shared/core/course_probe.cpp` compiled with `-std=c++20`, then linked:

```text
Compiling src/ofApp.cpp
Compiling src/main.cpp
Compiling ../shared/core//course_probe.cpp
Linking bin/linux-only for linux64
compiling done
```

Produced artifact:

```text
/work/linux-only/bin/linux-only: ELF 64-bit LSB pie executable, x86-64
size: approximately 3.4 MiB
```

The command exited successfully. The container had no display server, so the
windowed executable was not launched. No `ofxUnitTests` suite was built or run.
This result is `spike-proven`, not `build-proven` support for repository code.

## Cross-generation and regeneration observations

The packaged Linux Project Generator accepted
`-p"osx,linux64,vs"` against another disposable project and emitted Xcode,
Make/VS Code, and Visual Studio metadata. This proves template availability and
cross-generation only; it does not prove target-host generation or compilation.

The generated files demonstrated why the repository tracks canonical inputs
rather than native metadata:

- `config.make`, `Makefile`, and `Project.xcconfig` embedded `/opt/of`;
- the Xcode project embedded absolute references to `/opt/of/libs/openFrameworks`
  and `/opt/of/addons`;
- relative shared-source references appeared correctly in Xcode and Visual
  Studio metadata; and
- re-running generation changed random Xcode/filter identifiers and duplicated
  VS Code external include entries.

A clean-delete-and-regenerate wrapper is therefore required. Byte-identical
project metadata is not a valid reproducibility test.

## `unittest` template failure

This command was also tried against a fresh disposable target:

```bash
"$PG" \
  -o/opt/of \
  -plinux64 \
  -tunittest \
  -aofxUnitTests \
  -s../shared/core \
  unit-probe
```

Project Generator logged failures copying nonexistent `unittest/.vscode` and
reported `baseProject::create createProjectFile failed`. It still printed a
final `EXIT_OK`; the target had `src/main.cpp` and `addons.make` but lacked
required Make metadata.

The implementation must therefore:

- own the no-window runner source and `addons.make` in this repository;
- generate it with the normal host template rather than `-tunittest`;
- reject `[ error ]` output; and
- validate required output files instead of trusting process status.

## Remaining implementation evidence

The following are deliberately not claimed by this spike:

- generation or compilation on macOS or Windows;
- compilation of tracked `foundation/windowed` or `foundation/unit` projects;
- a graphical launch or bundled-asset rendering check;
- any no-window test build or execution; or
- Debug/Release parity and CI artifact retention.

Those proofs belong to the implementation harness and native CI rather than
this disposable spike. See [foundation harness validation](foundation-harness-evidence.md)
for explicitly pre-commit Linux and cross-generation results; macOS, Windows,
and graphical runtime remain unproven.
