#pragma once

#include "population_model.h"

struct PopulationPalette {
    unsigned char warm_r;
    unsigned char warm_g;
    unsigned char warm_b;
    unsigned char cool_r;
    unsigned char cool_g;
    unsigned char cool_b;
};

lightpopulation::Settings makePopulationSettings();
PopulationPalette makePopulationPalette();
