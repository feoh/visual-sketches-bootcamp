#pragma once
#include "image_geometry_model.h"

struct DisplayColor { int r; int g; int b; };
struct ImageGeometryDesign {
    image_geometry::SampleDesign sample;
    float point_radius;
    float motion_amplitude;
    float motion_rate;
    DisplayColor background;
    DisplayColor ink;
};
bool imageGeometryDesignIsValid(const ImageGeometryDesign& design);
ImageGeometryDesign makeImageGeometryDesign();
