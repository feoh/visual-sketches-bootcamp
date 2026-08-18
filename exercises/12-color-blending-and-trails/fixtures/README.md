# Independent color/trail oracle

`trail-oracle.txt` is a small human-computable fixture. It pins three palette
samples, straight-alpha source-over red on blue, and two decay steps. The C++
suite parses the file and separately checks malformed row cardinality, so the
fixture is evidence independent from model implementation control flow.
