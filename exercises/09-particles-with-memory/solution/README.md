# Explained solution

## What this example tries

This comparison uses sparse square memory marks and triangular heads rather than the
starter's connected ribbons and circles. Each current head retains a full-radius
triangular outline while its interior triangle grows from 35% to 80% of that radius with
normalized age. The limited, noncolor age cue remains visible when reduced mode hides
history. The solution also changes cadence, lifetime, gravity, drag, bounce, history,
seed, spacing, and palette. Notice how the particle calculations stay separate from
drawing; do not copy this visual system as your own result.
