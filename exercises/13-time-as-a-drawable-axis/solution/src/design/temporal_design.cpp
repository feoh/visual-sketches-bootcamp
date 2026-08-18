#include "temporal_design.h"

#include "temporal_history.h"

#include <cmath>

namespace {
bool colorValid(TemporalColor color) {
    return color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
}

TemporalDesign makeTemporalDesign() {
    return {180, 36, 2.1f, 10.0f,
            {17, 23, 39}, {247, 187, 83}, {115, 224, 190}};
}

bool temporalDesignIsValid(const TemporalDesign& design) {
    std::size_t bytes = 0;
    return temporal::checkedMemoryBytes(design.history_capacity, bytes) &&
           design.spatial_slices >= 2 && design.spatial_slices <= 512 &&
           std::isfinite(design.decay_rate) && design.decay_rate >= 0.0f &&
           design.decay_rate <= 16.0f && std::isfinite(design.mark_radius) &&
           design.mark_radius > 0.0f && design.mark_radius <= 1000.0f &&
           colorValid(design.background) && colorValid(design.recent) &&
           colorValid(design.distant);
}
