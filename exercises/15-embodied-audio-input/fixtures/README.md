# Recorded amplitude fixture

`amplitude-replay.txt` contains normalized amplitude measurements, not sound.
No voice, waveform, device identifier, timestamp, or other audio is retained.
The columns pin the starter design's smoothing and geometry mapping so a parser
can check every replay step independently. Lines beginning with `#` are
comments. Every data row has exactly seven whitespace-separated fields.

The sequence is course-authored synthetic data (`CC0-1.0`): silence, a value
inside the dead zone, a rise, a peak, a release, and a soft return. It is safe
to redistribute and does not represent a recorded person.
