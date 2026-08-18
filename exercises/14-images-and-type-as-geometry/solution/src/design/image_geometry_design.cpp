#include "image_geometry_design.h"

#include <cmath>

namespace {
bool colorIsValid(DisplayColor color) {
    return color.r >= 0 && color.r <= 255 &&
           color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
}

bool imageGeometryDesignIsValid(const ImageGeometryDesign& design) {
    return design.sample.step >= 1 && design.sample.step <= 4096 &&
           std::isfinite(design.point_radius) && design.point_radius > 0.0f &&
           design.point_radius <= 128.0f &&
           std::isfinite(design.motion_amplitude) &&
           design.motion_amplitude >= 0.0f && design.motion_amplitude <= 10000.0f &&
           std::isfinite(design.motion_rate) &&
           std::fabs(design.motion_rate) <= 1000.0f &&
           colorIsValid(design.background) && colorIsValid(design.ink);
}

// A visually divergent choice: sparse bright-space samples become orbiting bars.
ImageGeometryDesign makeImageGeometryDesign() {
    return {{3, 220, false}, 3.4f, 30.0f, 0.7f,
            {18, 24, 38}, {246, 178, 72}};
}
