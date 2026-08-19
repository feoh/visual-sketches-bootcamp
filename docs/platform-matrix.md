---
title: openFrameworks 0.12.1 platform matrix
weight: 35
draft: false
---

# openFrameworks 0.12.1 platform matrix

This document pins the foundation compatibility bands and release-adjacent CI
anchors. It does not promise bit-identical developer machines, and it does not
turn an upstream package or workflow into a claim that this repository has been
tested on that platform. Exact observed tool versions belong in dated evidence
rows.

## Evidence vocabulary

| Status | Meaning |
|---|---|
| `selected` | The bounded OS/toolchain combination is the implementation target, but this repository has not built there. |
| `cross-generation-observed` | A generator on another host emitted that target's metadata. This proves template availability only. |
| `generation-proven` | The target host generated its native metadata from the tracked out-of-tree project. |
| `spike-proven` | A disposable fixture established an architecture or compatibility fact, but no tracked repository target was tested. |
| `build-proven` | A fresh environment generated and compiled the tracked repository probe. It does not imply that a window was launched. |
| `unit-proven` | The no-window test executable built, ran, discovered tests, and returned success. |
| `unsupported` | The lane was attempted and cannot currently work; the row must state why and what would change that result. |

`skipped` and `failed` are CI results, not support statuses. A skipped or failed
lane must never be presented as supported. Every proof records the repository
commit, command, environment identity, and date.

## Immutable openFrameworks inputs

All lanes use the official [openFrameworks 0.12.1 release][of-release], source
tag `0.12.1` at commit
[`1019e6a783a309dac8d8d61cb990967bec5ce8f9`][of-tag], and the Project
Generator submodule at commit
[`74afcd48555ba9b3d0c430b2853ddad306a89b63`][pg-commit]. The command-line
Project Generator bundled in the Linux release reports version `0.103.0`.

| Lane | Official artifact | Bytes | SHA-256 |
|---|---|---:|---|
| Linux x86-64 | [`of_v0.12.1_linux64_gcc6_release.tar.gz`][linux-asset] | 355,979,203 | `d6c1dcab777665b2aa63e5e3d9122cc116f096b3421db3493f795a621b399c63` |
| macOS | [`of_v0.12.1_osx_release.tar.gz`][macos-asset] | 512,487,086 | `e4a2dd6e75805320aac905f9f82f7b85e67a8b799137d47f35d40f7c4c195c9b` |
| Windows x64 / Visual Studio | [`of_v0.12.1_vs_64_release.zip`][windows-asset] | 752,137,513 | `059f5210dd86beaee9dd17cfe4afe9eb5929376412b9baffda68ccd5d0b4d8a8` |

The Linux and Windows hashes were calculated from downloads of the immutable
GitHub release assets on 2026-08-17. GitHub publishes the macOS digest in the
release API. A setup script must reject a mismatched hash before extraction.
Do not replace these URLs with the rolling openFrameworks download page.

## Selected host matrix

| Lane | Selected compatibility band and evidence anchor | Native project mode | Current evidence | Permitted claim |
|---|---|---|---|---|
| Linux — Ubuntu | x86-64 Ubuntu 24.04 LTS; upstream 0.12.1 dependency script selected through `scripts/setup-linux.sh`; GCC 13 family; GNU Make | The tagged make configuration selects C++20 for GCC 10 or newer. | `build-proven` and `unit-proven` at commit `f3c42f4`: Debug/Release tracked projects and the exact `12/12` Release unit contract passed on Ubuntu 24.04 with GCC/G++ 13.3.0. See [`foundation-harness-evidence.md`](foundation-harness-evidence.md). | Repository native build and deterministic unit support are proven for the selected Ubuntu band. Graphical appearance remains a separate manual claim. |
| Linux — CachyOS | x86-64 CachyOS rolling snapshot; repository pacman/paru package plan; host-native Project Generator rebuild; GCC 16 family; GNU Make; X11/XWayland window backend | The repository wrapper patches the separately downloaded OF 0.12.1 tree for current libstdc++ and dual-backend GLFW, then compiles Debug/Release framework libraries and PG 0.103.0 locally. | `build-proven` and `unit-proven` on 2026-08-18 at the delivery working tree based on `64da096`: foundation doctor/generation, Debug/Release windowed and unit builds, exact `12/12` unit runtime, and all 32 section 00–15 starter/solution projects generated and built in Debug and Release. Every deterministic section/first-C++ test passed with GCC/G++ 16.1.1 and Clang 22.1.8. Five-second foundation/section-00 runtime probes plus participant launch/resize passed with CachyOS kernel package 7.1.6-1 and GLFW 3.5.1. | Native generation/build and deterministic unit support are proven for this exact dated CachyOS snapshot; section-00 graphical startup/resize is manually observed. Other graphical/device behavior remains unobserved. CachyOS is rolling, so do not generalize to an unrecorded future package snapshot. `ofxOpenCv` is outside this course lane. |
| macOS | Apple Silicon; macOS 15 family; Xcode 16 family; macOS 15 SDK family. The release-adjacent reference point was macOS 15.4.1, Xcode 16.0 (`16A242d`), SDK 15.0. | The `osx` Project Generator template writes `CLANG_CXX_LANGUAGE_STANDARD = c++23`. | `build-proven` and `unit-proven` at commit `f3c42f4`: Debug/Release tracked projects and the exact `12/12` Release unit contract passed on macOS 15.7.7 arm64, Xcode 16.4, SDK 15.5. | Repository native build and deterministic unit support are proven for the selected macOS band. Graphical appearance remains a separate manual claim. |
| Windows | x64 Windows Server 2022 build 20348 with floating UBR; Visual Studio 2022 17.x and v143 C++ tools; Windows SDK 10.0.26100.0. The release-adjacent reference point had UBR 3561 and VS 17.13.35825.156. | The tagged solution is Visual Studio 17, uses toolset `v143`, and sets C++ to `stdcpplatest`. | `build-proven` and `unit-proven` at commit `f3c42f4`: Debug/Release tracked projects and the exact `12/12` Release unit contract passed on build 20348.5386, VS 17.14/MSBuild 17.14.51, v143, SDK 10.0.26100.0. | Repository native build and deterministic unit support are proven for the selected Windows band. Graphical appearance remains a separate manual claim. |

The exact macOS and Windows patch versions identify release-adjacent GitHub
runner images for historical context only. Hosted-runner aliases do not pin
those patch images and this repository does not pretend otherwise. CI prints
`ImageVersion` and exact native tool versions, enforces the bounded families
above. The commit-addressed proof rows now record the exact observations from the
first all-green six-status run.

The upstream source also contains MSYS2 packages and workflows. MSYS2 is not a
course baseline: supporting two Windows compiler/runtime families would add
learner setup cost without improving the three-OS requirement.

## Clean-environment setup contracts

These are the commands the implementation wrappers must automate or verify.
They are intentionally explicit about which parts have and have not been run.

### Linux reference lanes

From this repository on Ubuntu or CachyOS:

```bash
scripts/setup-of.sh --platform linux64 --destination "$HOME/openframeworks"
export OF_ROOT="$HOME/openframeworks/of_v0.12.1_linux64_gcc6_release"
scripts/setup-linux.sh install --of-root "$OF_ROOT"
scripts/setup-linux.sh doctor --of-root "$OF_ROOT"
```

`setup-of.sh` performs the immutable download, SHA-256 check, fresh extraction,
and version check described above. `setup-linux.sh` reads `/etc/os-release` and
never runs apt on CachyOS/Arch or pacman on Ubuntu. Its CachyOS path installs the
course's required packages, uses `paru` for FreeImage only when the removed Arch
package is absent, omits unused `ofxOpenCv`/OpenCV setup, rebuilds Project
Generator against host libraries, and applies an idempotent X11 GLFW hint before
rebuilding the framework. openFrameworks 0.12.1's Linux window implementation
uses X11-native APIs, so `xorg-xwayland` is part of the CachyOS contract even
when the desktop session is Wayland.

The [2026-08-17 Ubuntu spike](foundation-spike.md) used a base image with digest
`sha256:561618e2c15bf2397621dd04f96926663a3b5616c189cf7e38db7e82f5c538ea`,
which resolved to Ubuntu 24.04.4 and GCC/G++ 13.3.0 after package installation.
The official installer uses Ubuntu package repositories, so patch-level package
resolution is not immutable. The selected band is Ubuntu 24.04 LTS plus GCC 13;
each CI run must print `lsb_release -ds`, `gcc --version`, `g++ --version`, the
runner image version, and the openFrameworks version macros. A future
binary-reproducible lane would additionally need an Ubuntu package snapshot.

### macOS reference lane

```bash
curl -L --fail -o of_v0.12.1_osx_release.tar.gz \
  https://github.com/openframeworks/openFrameworks/releases/download/0.12.1/of_v0.12.1_osx_release.tar.gz
printf '%s  %s\n' \
  e4a2dd6e75805320aac905f9f82f7b85e67a8b799137d47f35d40f7c4c195c9b \
  of_v0.12.1_osx_release.tar.gz | shasum -a 256 --check
tar -xzf of_v0.12.1_osx_release.tar.gz
export OF_ROOT="$PWD/of_v0.12.1_osx_release"
test "$(sw_vers -productVersion | cut -d. -f1)" = "15"
test "$(uname -m)" = "arm64"
xcodebuild -version | tee /dev/stderr | sed -n '1p' | grep -E '^Xcode 16([.]|$)'
test "$(xcrun --show-sdk-version | cut -d. -f1)" = "15"
xcrun clang++ --version
```

The official setup guide says Xcode 14.1 or newer; this project deliberately
selects the Xcode 16 family instead of treating that broad statement as a tested
range. The release-adjacent Xcode 16.0 (`16A242d`) value is a historical anchor,
not an assertion about the current floating runner. The repository recorded a
passing macOS generation/build/unit proof at commit `f3c42f4`; see the
[foundation evidence ledger](foundation-harness-evidence.md). That historical
proof does not establish current-HEAD CI or graphical launch/appearance.

### Windows reference lane

Install Visual Studio 2022 with the **Desktop development with C++** workload,
the v143 toolset, and Windows SDK 10.0.26100.0. Then, in PowerShell:

```powershell
$asset = "of_v0.12.1_vs_64_release.zip"
Invoke-WebRequest `
  "https://github.com/openframeworks/openFrameworks/releases/download/0.12.1/$asset" `
  -OutFile $asset
if ((Get-FileHash $asset -Algorithm SHA256).Hash.ToLower() -ne `
    "059f5210dd86beaee9dd17cfe4afe9eb5929376412b9baffda68ccd5d0b4d8a8") {
    throw "openFrameworks archive checksum mismatch"
}
Expand-Archive $asset -DestinationPath .
$env:OF_ROOT = (Resolve-Path .\of_v0.12.1_vs_64_release).Path
$nt = Get-ItemProperty `
  "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion"
if ($nt.CurrentBuild -ne "20348") {
    throw "Windows Server 2022 build 20348 is required"
}
Write-Host "observed Windows build=$($nt.CurrentBuild).$($nt.UBR)"
$vs = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" `
  -version "[17.0,18.0)" -products * `
  -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
  -format json | ConvertFrom-Json
if ($vs.installationVersion -notmatch "^17\.") {
    throw "Visual Studio 2022 17.x is required"
}
Write-Host "observed Visual Studio=$($vs.installationVersion)"
if (-not (Test-Path `
    "${env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.26100.0")) {
    throw "Windows SDK 10.0.26100.0 is missing"
}
```

The tagged Visual Studio template requests the latest installed Windows SDK if
none is supplied. Repository CI must pass `WindowsTargetPlatformVersion=10.0.26100.0`
so a changing runner image cannot silently move that SDK baseline. The UBR and
Visual Studio patch float within the bounded Windows Server 2022 / VS 2022
families and are printed on every run. The repository recorded a passing
Windows generation/build/unit proof at commit `f3c42f4`; see the
[foundation evidence ledger](foundation-harness-evidence.md). That historical
proof does not establish current-HEAD CI or graphical launch/appearance.

## Course-owned C++17 contract

Native openFrameworks projects select C++20, C++23, or the latest MSVC mode.
That is not permission for lesson code to depend on those modes. Course-owned
source must compile as a conservative C++17 subset and begins with a compile-time
floor check:

```cpp
#if __cplusplus < 201703L
#error "visual-sketches-bootcamp requires C++17 or newer"
#endif
```

Allowed teaching surface:

- fundamental numeric and Boolean types, functions, `const`, references, and
  scoped lifetime;
- `struct`, small classes, constructors, member initialization, and `enum class`;
- `std::array`, `std::vector`, `std::string`, range `for`, and a small set of
  readable algorithms such as `std::clamp`, `std::min`, `std::max`, and
  erase/remove;
- RAII and standard smart pointers where ownership must be explicit;
- lambdas only when they make a local callback or algorithm clearer; and
- `std::mt19937` with course-owned integer-to-value mapping when replay must be
  identical across standard libraries.

Outside the course contract:

- concepts, ranges, coroutines, modules, and all other C++20/23-only syntax;
- variable-length arrays, compiler extensions, custom template metaprogramming,
  raw owning `new`/`delete`, and platform-specific pragmas in shared code;
- standard-library distribution output as a cross-platform replay format;
- unordered-container iteration order as deterministic output; and
- `std::filesystem` for lesson asset lookup when openFrameworks data-path APIs
  express the intent more portably.

The implementation CI must compile one course-owned source probe through all
three native projects. A newer selected language mode may accept the source, but
review still rejects newer-language dependencies.

## Required proof before a support claim

For each lane, record:

1. archive checksum and extracted `OF_VERSION_MAJOR/MINOR/PATCH`;
2. Project Generator version and host platform;
3. OS image/build, architecture, compiler, IDE/build tool, and SDK;
4. clean generation from a fresh course checkout with no hand edits;
5. Debug and Release build commands and produced executable paths;
6. a window launch/manual visual probe, including one bundled licensed asset,
   where the environment has a display;
7. a no-window test run with discovered test count and process exit code; and
8. the repository commit, date, log/artifact URL, and known limitations.

Compilation and unit execution remain separate statuses:
`linux-build`, `linux-unit`, `macos-build`, `macos-unit`, `windows-build`, and
`windows-unit`. An unavailable unit lane is `unsupported`, not a successful
`*-unit` check.

## Sources

- [openFrameworks 0.12.1 release][of-release]
- [tagged platform workflows][of-workflows]
- [tagged Linux make standard selection][linux-standard]
- [tagged macOS project configuration][macos-standard]
- [tagged Visual Studio template][windows-template]
- [official Xcode setup guide][xcode-setup]
- [official Linux setup guide][linux-setup]
- [official Visual Studio setup guide][vs-setup]
- [release-adjacent macOS 15 runner image][macos-runner]
- [release-adjacent Windows Server 2022 runner image][windows-runner]
- [release-adjacent Ubuntu 24.04 runner image][linux-runner]
- [dated standalone spike evidence](foundation-spike.md)

[of-release]: https://github.com/openframeworks/openFrameworks/releases/tag/0.12.1
[of-tag]: https://github.com/openframeworks/openFrameworks/tree/1019e6a783a309dac8d8d61cb990967bec5ce8f9
[pg-commit]: https://github.com/openframeworks/projectGenerator/tree/74afcd48555ba9b3d0c430b2853ddad306a89b63
[linux-asset]: https://github.com/openframeworks/openFrameworks/releases/download/0.12.1/of_v0.12.1_linux64_gcc6_release.tar.gz
[macos-asset]: https://github.com/openframeworks/openFrameworks/releases/download/0.12.1/of_v0.12.1_osx_release.tar.gz
[windows-asset]: https://github.com/openframeworks/openFrameworks/releases/download/0.12.1/of_v0.12.1_vs_64_release.zip
[of-workflows]: https://github.com/openframeworks/openFrameworks/tree/0.12.1/.github/workflows
[linux-standard]: https://github.com/openframeworks/openFrameworks/blob/0.12.1/libs/openFrameworksCompiled/project/makefileCommon/config.linux.common.mk
[macos-standard]: https://github.com/openframeworks/openFrameworks/blob/0.12.1/scripts/templates/osx/Project.xcconfig
[windows-template]: https://github.com/openframeworks/openFrameworks/blob/0.12.1/scripts/templates/vs/emptyExample.vcxproj
[xcode-setup]: https://openframeworks.cc/setup/xcode/
[linux-setup]: https://openframeworks.cc/setup/linux-install/
[vs-setup]: https://openframeworks.cc/setup/vs/
[macos-runner]: https://github.com/actions/runner-images/blob/macos-15/20250428.1214/images/macos/macos-15-Readme.md
[windows-runner]: https://github.com/actions/runner-images/blob/win22/20250427.1/images/windows/Windows2022-Readme.md
[linux-runner]: https://github.com/actions/runner-images/blob/ubuntu24/20250427.1/images/ubuntu/Ubuntu2404-Readme.md
