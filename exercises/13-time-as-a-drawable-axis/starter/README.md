# Section 13 starter: time windows

The starter draws filled vertical strips. Each strip's horizontal position
selects one discrete history age; an outlined circle and crossbar show the
selected sample's y position without relying on color. Change parameters in
`src/design/temporal_design.cpp` and replace this strip grammar in
`src/ofApp.cpp`.

Run the pure tests before generating an openFrameworks project. P pauses, R
resets frame/time and history, and M freezes capture for reduced motion. Native
GPU launch, resize, contrast, and originality remain manual checks. The starter
intentionally uses no FBO.
