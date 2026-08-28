#include "traveler_design.h"

float traveler::stepDistance(float rate_pixels_per_second, float elapsed_seconds) {
    // TODO: Return the signed distance traveled during this amount of time.
    (void)rate_pixels_per_second;
    (void)elapsed_seconds;
    return 0.0f;
}

traveler::Design makeTravelerDesign() {
    // TODO: Choose a normalized start, speed, radius, and three valid colors.
    // The mark and background colors must differ.
    return {{-1.0f, 2.0f}, 0.0f, 0.0f,
            {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
}
