# Section 14 fixtures

`seed-mask.png` is an original 136×48 grayscale mask spelling “SHAPE” in a
hand-built 5×7 block alphabet with three circular cuts. It was created for this
repository by the course author and is licensed CC0-1.0. The identical copies in
each app's `bin/data/` directory are runtime assets.

`mask-oracle.txt` is CC0-1.0 test data. It specifies independent known counts,
bounds, and centroids for tiny masks constructed by the test. Keeping expected
values outside the implementation helps catch self-confirming mistakes.
