# Explained solution

The solution changes start, speed, radius, palette, and geometry while retaining the shared public fixed-step model. `ofApp::update()` passes `ofGetLastFrameTime()` (seconds) and current input into the model; it does not move pixels directly. The model clamps each frame to 0.1 second, accumulates it, and consumes 1/120-second steps. Pointer direction is recomputed at each fixed step; arrow direction is the device-independent fallback.

`draw()` is deliberately distinct from the starter: two triangles form a diamond silhouette and two parallel, non-flashing lines form a geometric trail opposite velocity. Those shape cues remain available without color. The geometry is drawn from model position, velocity, and radius; it does not fork or replace the shared motion model. Pause adds an outer ring, while reduced motion changes trail weight and speed. This is an instructional reference, not a required look.
