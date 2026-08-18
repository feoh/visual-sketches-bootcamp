#pragma once

namespace constellation {
struct Vec2 { float x; float y; };
struct Color { int r; int g; int b; };
struct Viewport { int width; int height; };
struct MotionState {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
};
struct Bounds {
    float left;
    float top;
    float right;
    float bottom;
};
struct Design {
    float anchor_x;
    float anchor_y;
    float reach;
    Color ink;
    Color accent;
    Color background;
};
struct Scene {
    bool valid;
    Vec2 anchor;
    Vec2 target;
    Vec2 direction;
    Vec2 unit_direction;
    float distance;
    Vec2 traveler;
    Vec2 constellation_left;
    Vec2 constellation_right;
};

Vec2 subtract(Vec2 to, Vec2 from);
Vec2 add(Vec2 first, Vec2 second);
Vec2 scale(Vec2 value, float amount);
float dot(Vec2 first, Vec2 second);
float length(Vec2 value);
float distance(Vec2 first, Vec2 second);
Vec2 normalizeOrZero(Vec2 value);
Vec2 moveToward(Vec2 start, Vec2 target, float max_step);
Vec2 seekAcceleration(Vec2 position, Vec2 target, float max_acceleration);
MotionState integrateFixed(MotionState state, float dt);
MotionState stepSeek(MotionState state, Vec2 target, float max_acceleration,
                     float max_speed, float dt);
Vec2 orbitPoint(Vec2 center, float radius, float phase_radians);
MotionState stepBounce(MotionState state, Bounds bounds, float dt);
bool designIsValid(const Design& design);
Scene makeScene(const Design& design, Viewport viewport, Vec2 requested_target);
bool sceneIsFiniteAndInBounds(const Scene& scene, Viewport viewport);
}  // namespace constellation
