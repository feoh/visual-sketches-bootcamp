# Deterministic foundation unit suite

This project owns the normal-template `ofAppNoWindow` runner and uses the
`ofxUnitTests` addon shipped with openFrameworks 0.12.1. Empty tracked
`ofApp.{h,cpp}` files satisfy the normal Xcode template's unconditional source
references; `main.cpp` owns execution. The strict wrapper expects exactly
**12** passing tests and propagates the main-loop exit code.
