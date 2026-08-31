# Foundation commands

No Python, Node, CMake, or globally installed Project Generator is used. Supply
the separately installed, pinned framework with `--of-root`/`-OfRoot` or
`OF_ROOT`.

## Linux host setup

Do not invoke an upstream Ubuntu or Arch dependency installer directly. The
repository wrapper detects Ubuntu, CachyOS, or Arch from `/etc/os-release`,
selects the matching package manager, and verifies the resulting host:

```bash
scripts/setup-of.sh --platform linux64 --destination "$HOME/openframeworks"
export OF_ROOT="$HOME/openframeworks/of_v0.12.1_linux64_gcc6_release"
scripts/setup-linux.sh install --of-root "$OF_ROOT"
```

Use `--yes` only when noninteractive package approval is intended. `doctor`
checks without changing packages; `prepare` applies host compatibility after
dependencies are already installed. On CachyOS/Arch, preparation idempotently
rebuilds Project Generator against the host's GLEW/GLFW/Poco/Assimp libraries,
adds the `<algorithm>` include required by current GCC, forces GLFW's X11
backend because openFrameworks 0.12.1 uses X11-native APIs, and recompiles the
framework Debug/Release libraries. The package plan includes `xorg-xwayland`.
It deliberately does not enable `ofxOpenCv`: the upstream 0.12.1 Arch script
only understands OpenCV 3/4 metadata while current CachyOS supplies OpenCV 5,
and no course project uses that addon.

`setup-of.sh` always restores the checksum-verified upstream tree. Run
`setup-linux.sh prepare` again after re-extracting it on CachyOS/Arch. FreeImage,
which openFrameworks 0.12.1 links directly, is installed from the AUR with
`paru` only when absent; without `--yes`, that review remains interactive.

POSIX (Linux/macOS):

```bash
scripts/foundation.sh doctor --of-root /path/to/of_0.12.1
scripts/foundation.sh generate --project all
scripts/foundation.sh build --project all --configuration Debug
scripts/foundation.sh build --project all --configuration Release
scripts/foundation.sh test --project unit --configuration Release
```

Windows PowerShell uses the same separate gates:

```powershell
.\scripts\foundation.ps1 doctor -OfRoot C:\path\to\of_0.12.1
.\scripts\foundation.ps1 generate -Project all
.\scripts\foundation.ps1 build -Project all -Configuration Debug
.\scripts\foundation.ps1 build -Project all -Configuration Release
.\scripts\foundation.ps1 test -Project unit -Configuration Release
```

`setup-of.sh` and `setup-of.ps1` download only the immutable 0.12.1 release
assets listed in the platform matrix. Their default destination is the
non-hidden `openframeworks/` directory: openFrameworks 0.12.1's GNU Make source
discovery filters every path containing a hidden directory component. On every
invocation the wrappers verify SHA-256 with portable output parsing, extract
into a fresh sibling directory, validate the expected 0.12.1 root, and only then
replace any cached extracted tree. A partial or tampered prior tree is never
reused merely because it exists.

Generation removes only the documented native outputs within each canonical
project and never compiles implicitly. It checks the exact course source
inventory and generated membership before returning success. Test execution
never builds implicitly and requires the exact `12/12 tests passed` contract.

## Authoring checks

The dependency-free authoring check validates portable leaf-bundle front
matter, source citations, asset licenses, relative links, and accessible media
companions. When Hugo is installed it also builds the complete fixture in an
isolated temporary site; require that proof explicitly in CI or release checks:

```sh
scripts/check-authoring.sh --require-hugo
```

```powershell
.\scripts\check-authoring.ps1 -RequireHugo
```

Without the require flag, structural checks still run and a missing Hugo binary
is reported as a skip. See [`../authoring/README.md`](../authoring/README.md) for
the source format and separate instructional/synthesis definitions of done.

## Optional publication build

The no-theme Hugo layer mounts the canonical `authoring/sections/` bundles
directly; it does not copy or rewrite lesson prose. Hugo Extended 0.164.0 or
newer is required for the checked publication build:

```sh
scripts/build-site.sh
```

```powershell
./scripts/build-site.ps1
```

Both commands validate authoring and fail on Hugo warnings before replacing the
ignored `.hugo-public/` output directory. They do not deploy or publish it.
For direct Hugo flags, accessibility/release gates, and licensing behavior, see
[`../docs/publication.md`](../docs/publication.md).

## Section exercises

Sections 00–17 use thin platform wrappers around the shared, containment-checked
`course-project` implementation. Their starter and solution projects
use the same pinned root but remain separate Project Generator targets.
Generation and compilation are separate; each renderer-independent C++17 test
is a third command:

```sh
scripts/section-00.sh generate --project starter
scripts/section-00.sh build --project starter --configuration Release
tests/run-section-00-tests.sh
```

```powershell
.\scripts\section-00.ps1 generate -Project starter
.\scripts\section-00.ps1 build -Project starter -Configuration Release
.\tests\run-section-00-tests.ps1
```

For sections 01–17, substitute the matching two-digit section number in those
command names; tests follow `tests/run-section-NN-tests.sh` or `.ps1`.
Sections 00 and 01 now begin intentionally red: their default commands test the
incomplete starter. Pass `solution` to the shell runner or `-Variant solution`
to the PowerShell runner when verifying the supplied reference solution.

Use `--of-root`/`-OfRoot` or `OF_ROOT` exactly as for the foundation. Generated
native metadata and build products remain ignored.

## Synthesis projects

Sections 18–19 intentionally do not add unproven generic Project Generator
wrappers. A learner keeps the checked build boundary by repurposing distinct
existing starter directories on a learner branch: section 08 for gesture
memory, section 05 or 07 for repetition, and section 10 or 11 for particles.
The capstone then evolves one completed study on a separate capstone branch.
See the [section 18 supported bootstrap](../authoring/sections/18-three-sketch-studies/index.md#reuse-three-working-starters)
and [section 19 capstone bootstrap](../authoring/sections/19-original-visual-instrument/index.md#start-from-one-working-study)
for the required baseline test, generation, build, launch, and checkpoint steps.
The selected section's existing shell/PowerShell runner remains the public test
command; synthesis adds its pure model contract and fixtures to that checked
path rather than committing machine-local native metadata.
