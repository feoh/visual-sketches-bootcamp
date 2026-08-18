# Exercise 15: a five-second amplitude instrument

Build one visual instrument with one sentence a new viewer can verify quickly:
**louder amplitude makes ______**. Recorded scalar amplitude, optional live RMS,
and keyboard fallback must enter the same deterministic adapter. Do not add a
camera, spectrum survey, recording, speech analysis, or 3D lane.

## Learner-owned choices

Edit `starter/src/design/audio_instrument_design.cpp` to own smoothing, dead
zone, radius range, repeated-mark range, and palette. Edit
`starter/src/ofApp.cpp` to replace the starter sun with a materially different
visual grammar. Keep one monotonic size/spacing/count/weight mapping, visible
source and activity state, a written cue, and bounded geometry.

The solution is not a target: it uses a dark horizontal woven horizon, outlined
diamonds, an arrow head, slower smoothing, a smaller dead zone, a wider range,
and more marks. Change composition and geometry—not only colors.

## Consent, device selection, retention, and fallback

The sketch starts in `NO DEVICE: KEYBOARD FALLBACK`; Up/Down drives the full
instrument. F replays the synthetic scalar fixture. Neither route requires
permission. The fixture is not sound and identifies no person.

L is optional and is the only route that requests the operating system's
selected default microphone. Before L, tell nearby people, obtain consent, and
select the intended input in OS sound settings. Verify the visible `LIVE
MICROPHONE` label. N or exit stops/closes the stream. If setup fails, remain in
no-device mode and use Up/Down or F. The callback computes one bounded RMS
scalar and retains no samples. Do not log or save waveform, speech, device
identity, timestamps, or derived histories.

## Controls

- Up/Down: adjust keyboard amplitude and switch explicitly to no-device mode;
- F: replay recorded amplitude values;
- L: opt in to the selected default microphone, if available;
- N: stop live input and enter no-device mode;
- P: pause consumption;
- R: reset/replay the current source; and
- M: reduce repeated marks while preserving the meter/primary geometry.

Every action and source has an on-screen text route. Sound and color are never
the only cues.

## Public contract

Keep declarations in `shared/audio_input_model.h` and
`makeAudioInstrumentDesign()`. The core accepts finite nonnegative normalized
amplitudes, clamps values above one, applies
`s_new = s_old + alpha * (raw - s_old)`, applies a `<=` dead zone, then maps to
bounded normalized level, radius, ray count, and active state. It exposes
accepted, rejected, and dropped counts. Batch work is capped at 256, fixtures at
4,096, callback frames at 4,096, and geometry at 128 marks. Replay and invalid
input do not depend on a device, clock, window, or pixels.

Linux x86-64 or macOS arm64:

```sh
CXX=g++ tests/run-section-15-tests.sh
CXX=clang++ tests/run-section-15-tests.sh
scripts/section-15.sh generate --project starter
scripts/section-15.sh build --project starter --configuration Release
```

Windows Visual Studio 2022 x64 Developer PowerShell:

```powershell
.\tests\run-section-15-tests.ps1
.\scripts\section-15.ps1 generate -Project starter
.\scripts\section-15.ps1 build -Project starter -Configuration Release
```

The native wrappers support openFrameworks 0.12.1 on Linux x86-64, macOS arm64,
and Windows Visual Studio 2022 x64. Other hosts are unattested and unsupported.
Compiler/model success does not prove launch, pixels, permissions, device
selection, or microphone operation.

## Required explanation

Explain smoothing visually/numerically/symbolically; exact dead-zone behavior;
scalar-to-geometry mapping; fixture determinism; source labels; invalid-input,
batch, fixture, callback, and geometry caps; why no sound is retained; how
consent/device selection and N/exit work; the explicit fallback; one non-audio,
non-color cue; and which visual choices you own.

## Reference solution

The [explained solution](solution/README.md) documents a visually divergent
answer. Preserve the deterministic and privacy contracts, not its image.

## Manual accessibility, privacy, and originality review

- Ask a new viewer to infer the mapping within five seconds; record their words.
- F and Up/Down work with no device, and source/activity are visible in text/shape.
- Live input was never automatic; consent and OS default selection preceded L.
- N and exit stop live input; no sound or identifying metadata is retained.
- Meter/size/count/text duplicate meaning without hearing or color.
- P, R, and M remain discoverable; no flicker or flashing appears.
- Resize and small windows keep work bounded; text crowding is noted honestly.
- Mapping, geometry, composition, response, and palette differ from both examples.
- Capture alt text names source, activity, quiet/loud geometry, density, meter, and palette roles.
- Credit reused references, code, and assets.
