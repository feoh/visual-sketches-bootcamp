#include "edition_design.h"

controlledchance::Design makeEditionDesign() {
    // Choose the seed, density, size range, and colors here.
    return {20260818u, 18, 2.5f, 6.0f,
            {31, 43, 61}, {218, 104, 73}, {242, 236, 218}};
}
