# Exercise: repair and write the first C++ test

This exercise uses the already-proven `foundation/unit` no-window executable rather than owning another generated openFrameworks project. The small public contract compiles the learner-owned known-case test without openFrameworks so feedback is quick; the lesson then shows how the same arrange/act/assert values fit an `ofxTest` assertion.

## 1. Repair one assertion

Follow the lesson's exact temporary patch in `foundation/unit/src/main.cpp`: make the seeded-state bounds assertion wrong with `position.x < 0.0f`, run the native suite, read the failure, and restore the correct `position.x >= 0.0f`. The tracked repository remains green.

## 2. Author one known case

Edit only `starter/learner_known_case.cpp`. Choose a seed and viewport, then temporarily print `actual.position.x`, `actual.position.y`, `actual.velocity.x`, and `actual.velocity.y` immediately after the single factory call. Run the starter, label and review all four values for finiteness, in-viewport position, and plausible velocity, then rerun to confirm exact repetition. Freeze those reviewed values as the oracle and remove the temporary output. Do not compute expectations by calling the injected model factory during arrangement; that would repeat the implementation instead of recording an independent oracle. Keep the factory call in the act step and comparisons in the assert step.

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

The public contract passes the real model factory and then the same controlled wrong factory into each variant. It requires the first to pass and the mutation to fail with a diagnostic, without containing the explained solution's seed or coordinates. `solution/` demonstrates a different reviewed seed; read it only after completing yours.

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

The foundation evidence ledger records the native Linux, macOS, and Windows unit statuses. This exercise adds no renderer, screenshot, pixel threshold, or generated metadata.
