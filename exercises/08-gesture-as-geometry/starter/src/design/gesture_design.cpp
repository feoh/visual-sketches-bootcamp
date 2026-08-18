#include "gesture_design.h"
#include <cmath>
gesture::Design makeGestureDesign() {
    // Own filtering, smoothing, spacing, speed-width range, and curvature palette.
    return {3.0f, std::log(2.0f) * 10.0f, 8.0f, 2.0f, 11.0f, 512,
            {24, 90, 128}, {221, 82, 62}, {244, 239, 222}};
}
