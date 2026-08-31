#include "population_design.h"

lightpopulation::Settings makePopulationSettings() {
    // Count is capped by the model. Change clusters before adding another behavior.
    return {20260831u, 6, 720, 190.0f, 2.3f, 9.0f, 92.0f};
}

PopulationPalette makePopulationPalette() {
    return {255, 112, 58, 63, 174, 255};
}
