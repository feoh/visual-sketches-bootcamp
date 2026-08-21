# Exercise: repair and write the first C++ test

## The short version

You will repair one small C++ test, then write a second test of your own. The goal is
not to become a testing expert. It is to learn how a tiny test can tell you, quickly and
specifically, when a calculation changes.

A test makes inputs, calls a function, and checks the answer. The supplied runner
handles the build details.

The small runner checks your calculation without opening a graphics window, so feedback
is quick. The lesson also shows how the same input, function call, and expected answer
fit into an openFrameworks `ofxTest`.

## 1. Repair one assertion

Follow the lesson's exact temporary patch in `foundation/unit/src/main.cpp`: make the seeded-state
bounds assertion wrong with `position.x < 0.0f`, run the native suite, read the failure, and
restore the correct `position.x >= 0.0f`. Finish with the repaired check in place.

## 2. Write one known-answer test

Edit only `starter/learner_known_case.cpp`. Choose a seed and window size. Immediately
after the single call that creates `actual`, temporarily print:

```cpp
std::cerr << actual.position.x << ' ' << actual.position.y << ' '
          << actual.velocity.x << ' ' << actual.velocity.y << '\n';
```

Add `#include <iostream>`, run the starter, and label the four numbers. Check that the
position is inside the window and the speed seems reasonable, then run again to confirm
the same seed repeats. Copy those reviewed numbers into the expected-value slots and
remove the temporary print and include.

Do not call the model a second time to create the expected answer. If the model contains
a mistake, both calls could agree on the same wrong answer.

```sh
for variant in starter solution; do
  CXX=g++ tests/run-first-cpp-test.sh "$variant"
  CXX=clang++ tests/run-first-cpp-test.sh "$variant"
done
```

In Windows Developer PowerShell:

```powershell
foreach ($variant in @("starter", "solution")) {
    .\tests\run-first-cpp-test.ps1 -Variant $variant
}
```

The runner tries your test with the real model and then with a deliberately wrong one.
The real model should pass. The wrong one should fail with a message showing which
number differed. `solution/` uses a different reviewed seed; read it after completing
yours if you want another example.

## Native harness commands

With openFrameworks 0.12.1 configured:

```sh
scripts/foundation.sh generate --project unit
scripts/foundation.sh build --project unit --configuration Release
scripts/foundation.sh test --project unit --configuration Release
```

On Windows Developer PowerShell:

```powershell
.\scripts\foundation.ps1 generate -Project unit
.\scripts\foundation.ps1 build -Project unit -Configuration Release
.\scripts\foundation.ps1 test -Project unit -Configuration Release
```

These commands run the larger foundation tests. This exercise adds no drawing window or
screenshot comparison; it checks numbers only.
