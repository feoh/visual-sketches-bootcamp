---
title: Standalone repository and Project Generator architecture
weight: 36
draft: false
---

# Standalone repository and Project Generator architecture

## Decision

`visual_sketches_bootcamp` remains a standalone Git repository. openFrameworks
0.12.1 is installed separately and supplied explicitly as `OF_ROOT`.

This is a deliberate exception to upstream's preferred `OF_ROOT/apps/...`
layout. It keeps curriculum history, exercises, and publication independent of
a 500–750 MB framework distribution while allowing every learner to replace or
reinstall the pinned framework tree without nesting this repository inside it.
The tradeoff is that native project metadata contains machine-local references
to `OF_ROOT`; that metadata is regenerated and is not source of truth.

## Authority and ownership

Project Generator 0.103.0 from the openFrameworks 0.12.1 distribution is the
only authority for Xcode, Visual Studio, Make, and VS Code metadata. Normal
course setup must not hand-edit generated project files.

Tracked canonical inputs for each project are:

- `src/**` — application or test-runner source owned by that project;
- `addons.make` — one addon name per line, with no absolute paths;
- `bin/data/**` — licensed runtime assets and attribution, where needed;
- repository-owned source under `shared/core/**` and test helpers under
  `shared/test-support/**`; and
- the wrapper arguments and generated-file allowlist implemented under
  `scripts/**`.

Generated, machine-local outputs include:

- `Makefile`, `config.make`, `.vscode/**`, and `*.code-workspace`;
- `*.xcodeproj/**`, `Project.xcconfig`, `of.entitlements`,
  `openFrameworks-Info.plist`, and generated schemes;
- `*.sln`, `*.vcxproj`, `*.vcxproj.filters`, `*.vcxproj.user`, and `icon.rc`;
- `obj/**`, compiled binaries, copied runtime libraries, and IDE state.

They are ignored by Git. A fresh clone first runs the generation wrapper, then
opens or builds the resulting native project. If a future openFrameworks change
requires a persistent native customization, it must be represented as a
Project Generator input or a documented, tested overlay; editing a generated
file in place is not an accepted solution.

## Foundation proof layout

The implementation phase will begin with two small projects:

```text
foundation/
├── windowed/
│   ├── src/
│   ├── addons.make
│   └── bin/data/
└── unit/
    ├── src/                 # tracked ofAppNoWindow/ofxUnitTests runner
    ├── addons.make          # contains ofxUnitTests
    └── bin/data/
shared/
├── core/                    # deterministic model, state, geometry, and math
└── test-support/            # tolerances, fixtures, and test-only helpers
scripts/                     # doctor/generate/build/test wrappers
```

Both projects compile `shared/core`. Only the unit project compiles
`shared/test-support`. The windowed project's `ofApp` adapts input and renders;
it does not own a second copy of the deterministic behavior.

Later exercise starters and solutions follow the same contract. Their
project-local `src` contains the adapter/renderer or test entry point; reusable
behavior stays in an explicitly supplied relative source directory.

## OF root discovery and validation

Wrappers use this precedence:

1. an `--of-root` command-line argument;
2. the `OF_ROOT` environment variable; then
3. no fallback.

`PG_OF_PATH` is an upstream Project Generator feature, but course wrappers set
it only from the validated `OF_ROOT`; it is not an independent second setting.
The wrapper must never scan a home directory, choose a rolling `latest`
installation, or silently fall back to another openFrameworks checkout.

Before generation, `doctor` verifies:

- the path exists and contains `addons`, `libs`, and `scripts`;
- `libs/openFrameworks/utils/ofConstants.h` defines version `0.12.1`;
- a packaged command-line Project Generator exists and reports `0.103.0`;
- the requested platform is exactly `linux64`, `osx`, or `vs`;
- every canonical project has `src` and `addons.make`; and
- the external shared-source paths exist with the exact recorded case.

Missing roots, wrong versions, missing generators, and absent source directories
fail before any generated file is deleted.

## Exact Project Generator command contract

Project paths are relative to the repository root. External source paths are
relative to the target project, because Project Generator copies the literal
relative path into generated metadata.

For `foundation/windowed`:

```bash
export PG_OF_PATH="$OF_ROOT"
"$PG" \
  -o"$OF_ROOT" \
  -p"linux64" \
  -s"../../shared/core" \
  foundation/windowed
```

macOS uses the same form with `-p"osx"`. The 0.12.1 command-line tool uses
the same joined short options on Windows; slash-form aliases are not supported:

```powershell
$env:PG_OF_PATH = $env:OF_ROOT
& $PG `
  "-o$env:OF_ROOT" `
  "-pvs" `
  "-s../../shared/core" `
  foundation/windowed
```

For `foundation/unit`, supply their single common `shared` root and keep the test
addon in tracked `addons.make`:

```text
ofxUnitTests
```

```bash
"$PG" \
  -o"$OF_ROOT" \
  -p"$HOST_PLATFORM" \
  -s"../../shared" \
  foundation/unit
```

On Windows the equivalent is:

```powershell
& $PG `
  "-o$env:OF_ROOT" `
  "-pvs" `
  "-s../../shared" `
  foundation/unit
```

The unit project uses one shared root because openFrameworks 0.12.1's GNU Make
rules apply `subst` and a pattern prerequisite to the complete space-separated
`PROJECT_EXTERNAL_SOURCE_PATHS` value; two external roots generate an invalid
`..shared/core` object target. One root still exposes `shared/core` and
`shared/test-support` only to the unit project, remains relative, and works in
all native metadata without a generated-file patch. The windowed project keeps
the narrower `shared/core` root.

The wrapper does not pass `-a`; on regeneration, Project Generator parses the
tracked `addons.make`. Addon membership therefore has one canonical owner.
Paths containing spaces must remain one quoted argument.

The packaged command-line executable locations observed in the official
archives are beneath `projectGenerator`:

- Linux: `projectGenerator/resources/app/app/projectGenerator`;
- Windows: `projectGenerator/resources/app/app/projectGenerator.exe`; and
- macOS: `projectGenerator/projectGenerator.app/Contents/Resources/app/app/projectGenerator`.

The wrapper locates only an allowlisted path within the validated 0.12.1 tree,
runs `--version`, and prints the chosen path. It does not use an unversioned
system `projectGenerator` from `PATH`.

## Clean regeneration, not in-place accumulation

Regeneration means semantic reproducibility, not byte-identical generated
files. Given the same canonical source, addon file, shared-source arguments,
platform, and 0.12.1 framework tree, a clean generation must:

1. produce the expected native project;
2. contain every current project-local and shared `.h`/`.cpp` source exactly
   once in IDE metadata where source lists exist;
3. contain no removed source;
4. build without a manual IDE edit; and
5. leave tracked files unchanged.

The wrapper performs these steps per project and per host platform:

1. run `doctor` without modifying the project;
2. delete only the platform's allowlisted generated files and build products;
3. preserve `src`, `addons.make`, and `bin/data`;
4. invoke Project Generator for exactly one platform;
5. reject any `[ error ]` line in generator output, even if the process returns
   zero;
6. assert the required generated files exist and are non-empty;
7. verify source membership and scan tracked files for an embedded absolute
   `OF_ROOT`.

Compilation is a subsequent explicit wrapper operation; generation never builds
implicitly.

Run clean regeneration after adding, removing, moving, or renaming source; after
changing `addons.make`; after changing the shared-source arguments; or after an
explicit openFrameworks upgrade. Linux Make discovers source recursively at
build time, but that does not remove the regeneration requirement: Xcode and
Visual Studio carry generated source membership.

Do not invoke `-p"osx,linux64,vs"` into one working tree in normal use. Generate
the host's metadata only. Each CI lane starts from a fresh checkout, so one
platform's machine-local paths cannot contaminate another lane.

## Spike evidence and 0.12.1 defects to design around

A disposable Ubuntu 24.04 container was used on 2026-08-17 with the official
Linux release archive and its packaged Project Generator 0.103.0. Exact inputs,
commands, outputs, and limitations are preserved in the
[foundation spike evidence](foundation-spike.md).

The spike proved:

- a project outside `OF_ROOT` can be created with explicit, independent OF and
  project paths;
- `PG_OF_PATH` and `-o` both resolve the external framework root;
- Project Generator accepted `linux64`, `osx`, and `vs` and emitted Make/VS
  Code, Xcode, and Visual Studio metadata from the Linux package; this is
  cross-generation evidence, not target-host generation proof;
- a relative external source argument such as `../shared/core` remained
  relative in `config.make`, Xcode source references, and Visual Studio source
  entries;
- the generated Linux Release build compiled the shared source and linked a
  standalone executable under Ubuntu 24.04.4 with GCC 13.3.0; and
- generated `config.make`, Make, and Xcode configuration embedded the absolute
  `/opt/of` used during the spike, confirming that generated metadata is
  machine-local.

The spike also found behavior the wrappers must not hide:

1. Re-running generation in place duplicated VS Code include entries. Xcode and
   Visual Studio filter files also regenerated random identifiers, so a second
   run was not byte-stable.
2. Generating several platform targets into one project amplified this churn.
3. `-t"unittest"` on the packaged Linux Project Generator attempted to copy a
   missing `.vscode` directory, logged `[ error ]`, omitted required build
   metadata, then still printed `EXIT_OK` and returned success.

Therefore the course tracks its own known-good no-window `src/main.cpp`, empty
`src/ofApp.{h,cpp}` compatibility adapter, and `addons.make`. The adapter
satisfies unconditional source references in the normal 0.12.1 Xcode template
while `main.cpp` owns the test runner. The wrapper performs clean regeneration,
parses generator diagnostics, and validates outputs. It does not depend on the
broken 0.12.1 `unittest` template or trust exit status alone.

## Build and test handoff

Generation and compilation are different gates.

- Linux build: `make -j"$(nproc)" Release` from the generated project.
- macOS build: `xcodebuild` against the generated project, explicit scheme,
  Release configuration, and selected SDK/toolchain.
- Windows build: `msbuild` against the generated project with
  `/p:Configuration=Release`, `/p:Platform=x64`, `/p:PlatformToolset=v143`, and
  the pinned Windows SDK.
- Unit lanes execute the produced no-window binary separately and fail when
  zero tests are discovered.

The implemented products are `bin/<project>_debug` and `bin/<project>` on
Linux, `bin/<project>Debug.app` and `bin/<project>.app` on macOS, and
`bin\\<project>_debug.exe` and `bin\\<project>.exe` on Windows. The generated
Xcode schemes are `<project> Debug` and `<project> Release`. The unit wrapper
runs from `foundation/unit/bin`, propagates the process exit code, and requires
exactly one `12/12 tests passed` summary. See `scripts/README.md` for the separate
doctor, generate, build, and test commands.

## Alternatives rejected

### Put this repository under `OF_ROOT/apps`

This follows upstream's safest relative-path convention, but couples course
history to one bulky framework installation and makes a normal standalone clone
incomplete. Rejected for curriculum distribution.

### Commit generated native projects

Generated metadata contains machine-local OF paths and is not byte-stable under
0.12.1 regeneration. Committing it would create noisy diffs and invite manual
edits. Rejected; clean generation is an explicit setup step.

### Maintain CMake or hand-written source lists beside Project Generator

A second project model would drift from Xcode/Visual Studio/Make and undermine
the required Project Generator workflow. Rejected for the baseline. A future
independent pure-C++ core test build may be added only as an additional fast
check, never as a substitute for native openFrameworks generation and tests.

### Vendor openFrameworks in this repository

This would duplicate hundreds of megabytes, obscure upstream provenance, and
make upgrades difficult to review. Rejected; archive URL, hash, and version
validation provide the dependency boundary.

## Sources

- [openFrameworks 0.12.1 Project Generator documentation][pg-doc]
- [0.12.1 command-line usage][pg-usage]
- [0.12.1 command-line argument and path implementation][pg-main]
- [official create-a-project guide][create-guide]
- [Project Generator commit bundled by openFrameworks 0.12.1][pg-commit]
- [platform matrix and immutable artifacts](platform-matrix.md)
- [dated foundation spike evidence](foundation-spike.md)

[pg-doc]: https://github.com/openframeworks/openFrameworks/blob/0.12.1/docs/projectgenerator.md
[pg-usage]: https://github.com/openframeworks/projectGenerator/blob/74afcd48555ba9b3d0c430b2853ddad306a89b63/commandLine/readme.md
[pg-main]: https://github.com/openframeworks/projectGenerator/blob/74afcd48555ba9b3d0c430b2853ddad306a89b63/commandLine/src/main.cpp
[create-guide]: https://openframeworks.cc/learning/01_basics/create_a_new_project/
[pg-commit]: https://github.com/openframeworks/projectGenerator/tree/74afcd48555ba9b3d0c430b2853ddad306a89b63
