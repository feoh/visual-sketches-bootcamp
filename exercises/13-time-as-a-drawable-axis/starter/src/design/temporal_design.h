#pragma once

#include <cstddef>

struct TemporalColor {
    int r;
    int g;
    int b;
};

struct TemporalDesign {
    std::size_t history_capacity;
    std::size_t spatial_slices;
    float decay_rate;
    float mark_radius;
    TemporalColor background;
    TemporalColor recent;
    TemporalColor distant;
};

TemporalDesign makeTemporalDesign();
bool temporalDesignIsValid(const TemporalDesign& design);
