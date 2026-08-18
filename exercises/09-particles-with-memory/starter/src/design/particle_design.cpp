#include "particle_design.h"

particles::Design makeParticleDesign() {
    // Own cadence, lifetime, forces, radius, memory cap, launch, and palette.
    return {
        1.0f / 60.0f,
        0.25f,
        8,
        0.08f,
        240,
        3.2f,
        0.85f,
        {0.0f, 42.0f},
        4.0f,
        0.78f,
        28,
        92.0f,
        {246, 241, 225},
        {22, 105, 122},
        {207, 72, 64}
    };
}
