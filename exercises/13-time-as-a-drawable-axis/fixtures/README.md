# Temporal oracle

`temporal-oracle.txt` is a small independent, hand-checkable fixture. `motion`
rows pin the origin of the explicit frame/time motion function. `selection`
rows pin round-to-nearest mapping from normalized horizontal position to age,
where age zero is newest. The pure C++ test checks row shape before indexing.
