# Section 12 starter: paired ribbon study

The starter emits two deterministic paths around a movable origin. Palette A is
a filled-circle teal ribbon; palette B is an open-square coral ribbon. Both use
normal alpha blending, but geometry preserves identity without hue. Samples are
oldest-to-newest, radius increases toward the newest point, and alpha decays by
the learner-owned retention value.

Pointer/drag and arrows move the shared origin. P pauses, R clears history and
restores phase, and M keeps only the latest mark in each trail. Replace its
design values, path/gesture source, blend treatment, geometry, density, and
composition rather than changing only color.

Build and launch through the section wrapper from the repository root. Run the
pure model suite first. Native compilation does not prove launch, appearance,
accessibility, renderer-state restoration, or originality; complete and record
the manual checklist in the exercise brief.
