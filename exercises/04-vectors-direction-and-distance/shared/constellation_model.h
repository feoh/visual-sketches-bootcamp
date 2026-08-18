#pragma once

namespace constellation {
struct Vec2 { float x; float y; };
struct Color { int r; int g; int b; };
struct Viewport { int width; int height; };
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
bool designIsValid(const Design& design);
Scene makeScene(const Design& design, Viewport viewport, Vec2 requested_target);
bool sceneIsFiniteAndInBounds(const Scene& scene, Viewport viewport);
}  // namespace constellation
