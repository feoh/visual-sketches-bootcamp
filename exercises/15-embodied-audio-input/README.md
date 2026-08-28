# Exercise 15: a clear amplitude instrument

Complete the [Lesson and Practice](../../authoring/sections/15-embodied-audio-input/index.md#lesson)
before starting this problem. This page is the authoritative exercise brief.

## The short version

You will turn one loudness number from 0 to 1 into visible size and repetition. The
required recorded and keyboard routes work without a microphone. Live input is optional.

Smoothing blends old and new levels; a dead zone removes very quiet room noise. Only the
latest loudness number crosses from the audio callback. No sound or loudness history is
saved.

Build one visual instrument with a relationship you can state in one sentence: **louder
amplitude makes ______**. Recorded scalar amplitude, optional live RMS, and keyboard
fallback must enter the same repeatable adapter. Do not add a camera, spectrum survey,
recording, speech analysis, or 3D lane.

## What you choose

Edit `starter/src/design/audio_instrument_design.cpp` to own smoothing, dead zone, radius range, repeated-mark range, and
palette. Edit `starter/src/ofApp.cpp` to replace the starter sun with a materially different
visual grammar. Keep one monotonic size/spacing/count/weight mapping, visible source and
activity state, a written cue, and limited geometry.

The solution is not a target: it uses a dark horizontal woven horizon, outlined
diamonds, an arrow head, slower smoothing, a smaller dead zone, a wider range, and more
marks. Change composition and geometry—not only colors.

## Consent, device selection, retention, and fallback

The sketch starts in `NO DEVICE: KEYBOARD FALLBACK`; Up/Down drives the full instrument. F replays the
synthetic scalar fixture. Neither route requires permission. The fixture is not sound
and identifies no person.

L is optional and is the only route that requests the operating system's selected
default microphone. Before L, tell nearby people, obtain consent, and select the
intended input in OS sound settings. Verify the visible `LIVE MICROPHONE` label. N or exit
stops/closes the stream. If setup fails, remain in no-device mode and use Up/Down or F.
The callback computes one limited RMS scalar and retains no samples. Do not log or save
waveform, speech, device identity, timestamps, or derived histories.

## Controls

- Up/Down: adjust keyboard amplitude and switch explicitly to no-device mode;
- F: replay recorded amplitude values;
- L: opt in to the selected default microphone, if available;
- N: stop live input and enter no-device mode;
- P: pause consumption;
- R: reset/replay the current source; and
- M: reduce repeated marks while preserving the meter/primary geometry.

Every action and source has an on-screen text route. Sound and color are never the only
cues.

## What the tests check

You do not need to memorize this list. Use it to understand a failure message and to see
which parts the computer can check for you.

Keep declarations in `shared/audio_input_model.h` and `makeAudioInstrumentDesign()`. The core accepts finite
nonnegative normalized amplitudes, clamps values above one, applies `s_new = s_old + alpha * (raw - s_old)`,
applies a `<=` dead zone, then maps to limited normalized level, radius, ray
count, and active state. It exposes accepted, rejected, and dropped counts. Batch work
is capped at 256, fixtures at 4,096, callback frames at 4,096, and geometry at 128
marks. Replay and invalid input do not depend on a device, clock, window, or pixels.

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

The course supplies checked openFrameworks 0.12.1 commands for Linux x86-64, macOS
arm64, and Windows Visual Studio 2022 x64. On another system, you may need to adapt the
build steps. After compiling, open the app to check its picture, permissions, selected
device, and optional microphone behavior.

## Notes for future you

If it will help later, leave a short privacy note saying whether you used the bundled
recording, keyboard controls, or a live microphone, and that the app stores levels
rather than sound. Then answer whichever of these helps you remember the sketch:

- How does smoothing stop one loud sample from causing a harsh jump?
- Which visible change represents quiet versus loud input?
- What can someone use when no microphone is available?

If you enabled live input, also record how `L`, `N`, reset, and exit affect the device so
you do not have to rediscover the consent and stop behavior later.

## Reference solution

The [explained solution](solution/README.md) documents a visually divergent answer. Preserve the repeatable and
privacy rules, not its image.

## Check the experience yourself

- Try the sketch without instructions for a moment: is the main input-to-picture
  relationship discoverable?
- F and Up/Down work with no device, and source/activity are visible in text/shape.
- Live input was never automatic; consent and OS default selection preceded L.
- N and exit stop live input; no sound or identifying metadata is retained.
- Meter/size/count/text duplicate meaning without hearing or color.
- P, R, and M remain discoverable; no flicker or flashing appears.
- Resize and small windows keep work limited; text crowding is noted honestly.
- Mapping, geometry, composition, response, and palette differ from both examples.
- Capture alt text names source, activity, quiet/loud geometry, density, meter, and
  palette roles.
- Credit reused references, code, and assets.
