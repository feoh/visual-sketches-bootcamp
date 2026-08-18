#include "gesture_design.h"
#include <cmath>
gesture::Design makeGestureDesign() {
    // The solution deliberately chooses a tighter, quicker, night-palette system.
    return {5.0f, std::log(2.0f) * 16.0f, 12.0f, 1.5f, 8.0f, 384,
            {236, 200, 92}, {126, 224, 190}, {19, 25, 38}};
}
