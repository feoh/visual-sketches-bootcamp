#include "population_design.h"

lightpopulation::Settings makePopulationSettings() {
    // This denser, smaller-grain cloud is deliberately distinct from the starter.
    return {711223u, 9, 1180, 215.0f, 1.5f, 6.5f, 78.0f};
}

PopulationPalette makePopulationPalette() {
    return {255, 179, 157, 37, 141, 177};
}
