#include "structured_chance_design.h"

structuredchance::Settings makeStructuredChanceSettings() {
    // Change the seed, grid, inherited angle, density range, and quiet-region density.
    return {20260830u, 3, 3, 0.045f, 0.82f, 0.24f, 10, 18, 3};
}

StructuredChancePalette makeStructuredChancePalette() {
    return {8, 12, 30, 29, 187, 255, 255, 68, 164};
}
