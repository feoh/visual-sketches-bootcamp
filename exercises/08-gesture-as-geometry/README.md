# Exercise 08: speed-width and curvature-color gesture

Capture a finite gesture, then turn sampled motion into geometry. Slow samples
become wider and stronger turns shift palette. The standard-library C++17 model
owns filtering, smoothing, velocity, turning angle, arc length, capacity, and
resampling; openFrameworks only captures input and draws inspectable results.

## Learner-owned choices

Edit `starter/src/design/gesture_design.cpp`: own minimum distance, smoothing
rate, resampling spacing, width range, capacity, and three-color palette. Then
replace the starter ribbon in `starter/src/ofApp.cpp` with a geometric language
that differs in structure, not only color. The solution uses narrow triangular
facets, dots, a tighter filter, quicker smoothing, and a night palette.

Drag with a mouse or trackpad. Without a pointer, arrow keys add a path from the
visible square/circle cursor. Press C to clear/reset. Motion is only a direct
result of input: there is no autoplay, flashing, or background motion to reduce.

## Public contract and build

```sh
CXX=g++ tests/run-section-08-tests.sh
CXX=clang++ tests/run-section-08-tests.sh
scripts/section-08.sh generate --project starter
scripts/section-08.sh build --project starter --configuration Release
```

Use `.ps1` wrappers in Windows Developer PowerShell. Keep `makeGestureDesign()`
and `shared/gesture_model.h` declarations. Tests cover strictly parsed fixture
rows, growth/filtering/pruning/capacity, frame-partition smoothing, invalid dt,
velocity, straight/corner/duplicate turning, cumulative length, uniform
resampling edge cases and declared output cap, finite stroke/facet-aware bounds, deterministic replay,
variation, and learner mappings. There is no screenshot or pixel gate. Native
three-platform Debug/Release statuses prove compilation, not graphical launch.

## Manual accessibility and originality review

- Pointer and arrow-key routes both create marks; C clears them.
- No autoplay, flashing, audio-only cue, or hidden reduced-motion animation exists.
- Background/slow and background/turn colors have suitable contrast.
- Width plus geometry conveys speed/turn differences without color alone.
- Resizing clears the gesture and recenters the cursor; capture is suppressed when a dimension is smaller than the full maximum stroke width.
- Marks remain legible near all edges and after window resizing.
- Your geometry, spacing, mapping, and palette differ from starter and solution.
- Capture alt text names direction, speed-width changes, corners, shape, and palette roles.
- Credit any reused visual reference.
