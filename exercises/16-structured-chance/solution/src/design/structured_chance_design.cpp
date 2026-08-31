#include "structured_chance_design.h"

structuredchance::Settings makeStructuredChanceSettings() {
    // A more vertical, sparser family demonstrates that the model is not one picture.
    return {918273u, 4, 3, 0.032f, 1.34f, 0.42f, 7, 13, 2};
}

StructuredChancePalette makeStructuredChancePalette() {
    return {18, 8, 27, 255, 176, 72, 109, 101, 255};
}
