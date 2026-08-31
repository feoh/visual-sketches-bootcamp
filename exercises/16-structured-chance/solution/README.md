# Section 16 reference solution: quiet windows

This answer uses a 4 × 3 grid, lower density, a steeper dominant angle, amber and
violet families, wider seams, and a thinner quiet-region core. The same shared
model supplies exact seams, bounded jitter, local clipping, and one quiet panel.

The visual result comes from relationships rather than independent decoration:
regions inherit direction and palette role; strokes vary only spacing and width.
The renderer stacks one faint wide line beneath a narrow core. `G` removes the
soft layer while preserving the composition.

Build with `scripts/section-16.sh generate --project solution` and
`scripts/section-16.sh build --project solution --configuration Release` (or
the matching PowerShell commands). Compare structure across many seeds rather
than copying this palette or layout.
