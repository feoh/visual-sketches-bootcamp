# Exercise 08: speed-width and curvature-color gesture

Complete the [Lesson and Practice](../../authoring/sections/08-gesture-as-geometry/index.md#lesson)
before starting this problem. This page is the authoritative exercise brief.

## The short version

You will turn a pointer drag into a clean path that can change width, color, and mark
shape. The model removes points that are too close, smooths sudden jumps, and measures
speed and turning. A separate tested helper can create evenly spaced points, but the
starter does not draw those resampled points unless you wire that extension in.

Try the saved gesture first. It gives you a path whose expected numbers you can check
without needing a perfect hand movement.

Capture a finite gesture, then turn sampled motion into geometry. Slow samples become
wider and stronger turns shift palette. The standard-library C++17 model owns filtering,
smoothing, velocity, turning angle, arc length, capacity, and resampling; openFrameworks
only captures input and draws easy to check results.

## What you choose

Edit `starter/src/design/gesture_design.cpp`: choose minimum distance, smoothing rate,
width range, capacity, and a three-color palette. Change resampling spacing only if you
also wire the optional resampling helper into your drawing. Then replace the starter ribbon in
`starter/src/ofApp.cpp` with a geometric language that differs in structure, not only color. The
solution uses narrow triangular facets, dots, a tighter filter, quicker smoothing, and a
night palette.

Drag with a mouse or trackpad. Without a pointer, arrow keys add a path from the visible
square/circle cursor. Press C to clear/reset. Motion is only a direct result of input:
there is no autoplay, flashing, or background motion to reduce.

## What the tests check and build

```sh
CXX=g++ tests/run-section-08-tests.sh
CXX=clang++ tests/run-section-08-tests.sh
scripts/section-08.sh generate --project starter
scripts/section-08.sh build --project starter --configuration Release
```

Use `.ps1` wrappers in Windows Developer PowerShell. Keep `makeGestureDesign()` and
`shared/gesture_model.h` declarations. Tests cover strictly parsed fixture rows,
growth, filtering, pruning, capacity, smoothing with elapsed time split into different
frame sizes, invalid time steps, velocity, straight/corner/duplicate turning,
cumulative length, uniform resampling edge cases and
declared output cap, finite stroke/facet-aware bounds, repeatable replay, variation, and
mappings you choose. There is no screenshot comparison. The course supplies checked
Linux, macOS, and Windows build commands; open the app to check the finished drawing.

## Check the result yourself

- Pointer and arrow-key routes both create marks; C clears them.
- No autoplay, flashing, audio-only cue, or hidden reduced-motion animation exists.
- Background/slow and background/turn colors have suitable contrast.
- Width plus geometry conveys speed/turn differences without color alone.
- Resizing clears the gesture and recenters the cursor; capture is suppressed when a
  dimension is smaller than the full maximum stroke width.
- Marks remain legible near all edges and after window resizing.
- Your geometry, spacing, mapping, and palette differ from starter and solution.
- Capture alt text names direction, speed-width changes, corners, shape, and palette
  roles.
- Credit any reused visual reference.
