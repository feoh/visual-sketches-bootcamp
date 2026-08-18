# Section 05 numerical fixtures

`phase-cases.tsv` contains independently calculated row-major mark oracles for a
3-by-4 field in a 400-by-300 viewport. The fixture reader rejects missing or
extra fields. Values use `amplitude=8`, `frequency=0.5 Hz`, row and column phase
steps of `pi/2`, and radius `4`; the stroke-aware base inset is `15.5` pixels.
These data are numerical expectations, not values emitted by the model and not
a pixel reference.
