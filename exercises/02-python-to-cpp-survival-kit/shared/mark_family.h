#pragma once

#include <vector>

namespace markfamily {
struct Vec2 {
    float x;
    float y;
};

struct Color {
    int r;
    int g;
    int b;
};

struct Viewport {
    int width;
    int height;
};

struct Design {
    int count;
    Vec2 normalized_center;
    float spacing_pixels;
    float base_radius_pixels;
    float radius_step_pixels;
    Color mark_color;
    Color accent_color;
    Color background_color;
};

struct Mark {
    Vec2 center;
    float radius;
    int family_index;
};

bool designIsValid(const Design& design);
std::vector<Mark> makeMarkFamily(const Design& design, Viewport viewport);
bool marksAreFiniteAndInBounds(const std::vector<Mark>& marks, Viewport viewport);
}  // namespace markfamily
