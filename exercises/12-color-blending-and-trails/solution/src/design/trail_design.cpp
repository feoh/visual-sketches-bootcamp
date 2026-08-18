#include "trail_design.h"

colortrail::Design makeTrailDesign() {
    return {
        110,
        0.965f,
        0.48f,
        1.0f,
        7.0f,
        {0.025f, 0.035f, 0.085f, 1.0f},
        {
            {{0.12f, 0.38f, 0.95f, 1.0f}, {0.44f, 0.96f, 0.90f, 1.0f}},
            {{0.56f, 0.18f, 0.92f, 1.0f}, {1.0f, 0.67f, 0.18f, 1.0f}}
        }
    };
}
