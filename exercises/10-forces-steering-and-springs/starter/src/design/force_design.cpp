#include "force_design.h"

forces::Design makeForceDesign() {
    // Own the force caps, behavior feel, spacing, response, and palette.
    return {
        1.0f / 60.0f,
        0.25f,
        8,
        7,
        7.0f,
        1.0f,
        180.0f,
        220.0f,
        1.4f,
        90.0f,
        9000.0f,
        6200.0f,
        24.0f,
        38.0f,
        18.0f,
        2.8f,
        0.72f,
        {246, 241, 225},
        {30, 112, 126},
        {207, 72, 64}
    };
}
