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

// CHANGE YOUR DESIGN HERE: density, darkness threshold, mark size, motion, and colors.
// Keep values within imageGeometryDesignIsValid(); the pure model tests this seam.
ImageGeometryDesign makeImageGeometryDesign() {
    return {{2, 150, true}, 2.2f, 18.0f, 1.2f,
            {244, 239, 222}, {28, 70, 92}};
}
