#pragma once

#include <array>

namespace sculpture {
constexpr float pi = 3.14159265358979323846f;
constexpr float tau = 2.0f * pi;

struct Vec2 { float x; float y; };
struct Mat3 { std::array<float, 9> values; };
struct Color { int r; int g; int b; };
struct Viewport { int width; int height; };
struct Design {
    float period_seconds;
    float first_length;
    float second_length;
    float ornament_radius;
    float stroke_width;
    float root_swing_degrees;
    float child_swing_degrees;
    Color ink;
    Color accent;
    Color background;
};
struct Scene {
    bool valid;
    float time_seconds;
    float root_angle_degrees;
    float child_angle_degrees;
    float model_scale;
    Mat3 root_transform;
    Mat3 child_transform;
    Vec2 pivot;
    Vec2 elbow;
    Vec2 tip;
};

Mat3 identity();
Mat3 translation(float x, float y);
Mat3 rotationDegrees(float degrees);
// Finite zero scales are allowed (they collapse an axis); non-finite inputs return identity.
Mat3 scaling(float x, float y);
Mat3 multiply(const Mat3& left, const Mat3& right);
Vec2 transformPoint(const Mat3& transform, Vec2 point);
float wrapTime(float time_seconds, float period_seconds);
bool designIsValid(const Design& design);
Scene makeScene(const Design& design, Viewport viewport, float time_seconds);
bool sceneIsFiniteAndInBounds(const Scene& scene, const Design& design,
                              Viewport viewport);
}  // namespace sculpture
