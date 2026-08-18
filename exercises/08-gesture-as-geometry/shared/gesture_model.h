#pragma once

#include <cstddef>
#include <vector>

namespace gesture {
constexpr float pi = 3.14159265358979323846f;
constexpr float minimum_resample_spacing = 0.001f;
constexpr std::size_t maximum_resampled_points = 100000;
struct Vec2 { float x; float y; };
struct Color { int r; int g; int b; };
struct Design {
    float minimum_distance;
    float smoothing_k;
    float resample_spacing;
    float minimum_width;
    float maximum_width;
    std::size_t maximum_samples;
    Color slow_color;
    Color fast_color;
    Color background;
};
struct Sample {
    Vec2 raw;
    Vec2 position;
    float time_seconds;
    Vec2 velocity;
    float speed;
    float signed_turn_radians;
    float arc_length;
};
struct Gesture { std::vector<Sample> samples; };
struct Style { float width; Color color; };
struct Triangle { Vec2 first; Vec2 second; Vec2 tip; };

float distance(Vec2 a, Vec2 b);
Vec2 exponentialSmooth(Vec2 current, Vec2 target, float smoothing_k, float dt);
Vec2 guardedVelocity(Vec2 previous, Vec2 current, float dt);
float signedTurningAngle(Vec2 first, Vec2 middle, Vec2 last);
std::vector<Vec2> uniformResample(const std::vector<Vec2>& points, float spacing);
bool designIsValid(const Design& design);
bool addSample(Gesture& gesture, Vec2 raw, float time_seconds, const Design& design);
void clear(Gesture& gesture);
Style styleForSample(const Sample& sample, const Design& design);
bool facetTriangle(Vec2 start, Vec2 end, float width, Triangle& triangle);
bool finiteAndStrokeAwareInBounds(const Gesture& gesture, int width, int height,
                                  float maximum_stroke_width);
}  // namespace gesture
