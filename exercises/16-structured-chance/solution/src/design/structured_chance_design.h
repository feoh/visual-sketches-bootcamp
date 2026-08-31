#pragma once

#include "structured_chance_model.h"

struct StructuredChancePalette {
    unsigned char background_r;
    unsigned char background_g;
    unsigned char background_b;
    unsigned char first_r;
    unsigned char first_g;
    unsigned char first_b;
    unsigned char second_r;
    unsigned char second_g;
    unsigned char second_b;
};

structuredchance::Settings makeStructuredChanceSettings();
StructuredChancePalette makeStructuredChancePalette();
