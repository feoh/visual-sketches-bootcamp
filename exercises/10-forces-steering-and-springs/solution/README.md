# Section 10 reference solution: elastic constellation

This answer changes the starter's count, spacing, force response, speed,
palette, and geometry. Seek uses filled amber triangles and target guides; the
guides show displacement, not the net steering force after velocity, attraction,
and pair repulsion are combined. Spring mode uses open mint squares with exactly one
anchor-to-first link plus adjacent chain links. Its control is a square, matching
the mode cue. Shape, fill, control, and topology distinguish modes without color.
M freezes the deterministic model and removes moving links while current
positions and controls remain visible.

## Why these values

Nine agents make the chain legible without crowding it. A 30-pixel rest length
keeps neighboring squares distinct; stiffness 24 and axial damping 3.4 make the
links recover promptly without prolonged ringing. Mass 1.2, maximum force 145,
maximum speed 175, and global damping 1.8 keep pointer changes responsive but
bounded. Softening 30 prevents near-coincident radial spikes, while attraction
7200 and repulsion 4800 let seek cohere without collapsing every agent onto the
same point. Restitution 0.62 makes edge contact visible without sustaining a
hard bounce. The dark field, amber filled triangles, and mint open squares add
non-color mode cues and readable contrast. Rendered marks reserve their model
radius for stroke/rotation, and controls use a larger clamp inset.

These are exercise-required choices, not uniquely correct values. At exact
coincidence, radial force is zero because there is no direction to normalize.
For a spring axis from the first endpoint to the second, this implementation's
first-endpoint scalar is `k(distance - rest_length)`; this is the same restoring
relationship as conventional `F = -k(x - L)` when `x - L` is defined in the
outward displacement direction. The axial dot product keeps damping parallel
to each link and opposes approach or separation without damping sideways motion.
The model advances at a fixed 1/60-second step, simulates at most eight steps per
render frame, and reports clamped or unsimulated whole-step time as dropped
rather than hiding unbounded catch-up.

A learner should explain and replace count, force/speed caps, damping, arrival,
softening, spring equilibrium, restitution, geometry, mode cue, reduced motion,
and palette rather than treating this design as a target image.

## Build and launch

From the repository root on Linux:

```sh
scripts/section-10.sh generate --project solution
scripts/section-10.sh build --project solution --configuration Release
exercises/10-forces-steering-and-springs/solution/bin/solution
```

On macOS:

```sh
scripts/section-10.sh generate --project solution
scripts/section-10.sh build --project solution --configuration Release
open exercises/10-forces-steering-and-springs/solution/bin/solution.app
```

On Windows Developer PowerShell:

```powershell
.\scripts\section-10.ps1 generate -Project solution
.\scripts\section-10.ps1 build -Project solution -Configuration Release
& .\exercises\10-forces-steering-and-springs\solution\bin\solution.exe
```

Manually inspect both modes, pointer and keyboard controls, pause/reset/still
mode, resize, every boundary, contrast, and originality. Compilation and pure
state tests do not prove those visual claims.
