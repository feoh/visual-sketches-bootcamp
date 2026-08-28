#include "traveler_design.h"

float traveler::stepDistance(float rate_pixels_per_second, float elapsed_seconds) {
    return rate_pixels_per_second * elapsed_seconds;
}

traveler::Design makeTravelerDesign() {
    // A bright, compact traveler with a deliberately slower reference pace.
    return {{0.18f, 0.64f}, 112.0f, 14.0f,
            {252, 211, 77}, {87, 196, 229}, {28, 24, 46}};
}
