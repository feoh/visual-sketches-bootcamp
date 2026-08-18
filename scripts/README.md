# Foundation commands

No Python, Node, CMake, or globally installed Project Generator is used. Supply
the separately installed, pinned framework with `--of-root`/`-OfRoot` or
`OF_ROOT`.

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

## Section exercises

Sections 00–10 use thin platform wrappers around the shared, containment-checked
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

For sections 01–10, substitute the matching two-digit section number in those
command names; tests follow `tests/run-section-NN-tests.sh` or `.ps1`.

Use `--of-root`/`-OfRoot` or `OF_ROOT` exactly as for the foundation. Generated
native metadata and build products remain ignored.
