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
assets listed in the platform matrix. On every invocation they verify SHA-256,
extract into a fresh sibling directory, validate the expected 0.12.1 root, and
only then replace any cached extracted tree. A partial or tampered prior tree is
never reused merely because it exists.

Generation removes only the documented native outputs within each canonical
project and never compiles implicitly. It checks the exact course source
inventory and generated membership before returning success. Test execution
never builds implicitly and requires the exact `12/12 tests passed` contract.
