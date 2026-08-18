#include "trail_design.h"

colortrail::Design makeTrailDesign() {
    return {
        72,
        0.94f,
        0.72f,
        2.0f,
        11.0f,
        {0.965f, 0.945f, 0.890f, 1.0f},
        {
            {{0.035f, 0.42f, 0.48f, 1.0f}, {0.30f, 0.83f, 0.72f, 1.0f}},
            {{0.60f, 0.09f, 0.34f, 1.0f}, {0.96f, 0.49f, 0.24f, 1.0f}}
        }
    };
}
