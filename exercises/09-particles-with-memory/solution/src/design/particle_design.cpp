#include "particle_design.h"

particles::Design makeParticleDesign() {
    // Deliberately distinct: sparse long-lived sparks, stronger gravity, dark field.
    return {
        1.0f / 60.0f,
        0.25f,
        8,
        0.14f,
        160,
        4.5f,
        0.38f,
        {0.0f, 68.0f},
        3.0f,
        0.62f,
        18,
        138.0f,
        {17, 22, 35},
        {244, 194, 79},
        {101, 214, 188}
    };
}
