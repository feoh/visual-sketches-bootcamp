#include "force_design.h"

forces::Design makeForceDesign() {
    // A taut, slow amber/indigo answer deliberately unlike the starter.
    return {
        1.0f / 60.0f,
        0.25f,
        8,
        9,
        6.0f,
        1.2f,
        145.0f,
        175.0f,
        1.8f,
        120.0f,
        7200.0f,
        4800.0f,
        30.0f,
        30.0f,
        24.0f,
        3.4f,
        0.62f,
        {20, 25, 45},
        {242, 178, 76},
        {111, 185, 175}
    };
}
