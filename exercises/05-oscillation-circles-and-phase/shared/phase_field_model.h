#pragma once

#include <vector>

namespace phasefield {
constexpr float pi = 3.14159265358979323846f;
constexpr float tau = 2.0f * pi;

struct Vec2 { float x; float y; };
struct Polar { float radius; float angle_radians; };
struct Color { int r; int g; int b; };
struct Viewport { int width; int height; };
struct Design {
    int rows;
    int columns;
    float amplitude;
    float frequency_hz;
    float row_phase_step;
    float column_phase_step;
    float mark_radius;
    Color ink;
    Color accent;
    Color background;
};
struct Mark {
    int row;
    int column;
    int index;
    Vec2 base;
    Vec2 center;
    float phase_radians;
};
struct Scene {
    bool valid;
    float time_seconds;
    std::vector<Mark> marks;
};

float degreesToRadians(float degrees);
float radiansToDegrees(float radians);
Vec2 polarToCartesian(Polar polar);
Polar cartesianToPolar(Vec2 cartesian);
float oscillate(float amplitude, float frequency_hz, float time_seconds,
                float phase_radians);
int flatIndex(int row, int column, int columns);
bool designIsValid(const Design& design);
Scene makeScene(const Design& design, Viewport viewport, float time_seconds);
bool sceneIsFiniteAndInBounds(const Scene& scene, const Design& design,
                              Viewport viewport);
}  // namespace phasefield
