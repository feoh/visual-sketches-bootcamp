#pragma once

#include <cstdint>
#include <vector>

namespace structuredchance {

struct Point {
    float x;
    float y;
};

struct Quad {
    Point top_left;
    Point top_right;
    Point bottom_right;
    Point bottom_left;
};

struct Stroke {
    Point local_a;
    Point local_b;
    Point a;
    Point b;
    float width;
    int palette_role;
};

struct Region {
    int row;
    int column;
    int family;
    bool quiet;
    float angle_radians;
    Quad quad;
    std::vector<Stroke> strokes;
};

struct Settings {
    std::uint32_t seed;
    int rows;
    int columns;
    float grid_jitter;
    float composition_angle;
    float region_angle_jitter;
    int min_strokes;
    int max_strokes;
    int quiet_strokes;
};

struct Composition {
    Settings settings;
    int quiet_region_index;
    std::vector<Point> shared_nodes;
    std::vector<Region> regions;
};

bool finitePoint(Point point);
float signedArea(const Quad& quad);
bool isValid(const Settings& settings);
Point bilinearPoint(const Quad& quad, Point local);
bool clipLineToUnitSquare(Point center, Point direction, Point& a, Point& b);
Composition generateComposition(const Settings& settings);

}  // namespace structuredchance
