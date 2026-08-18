#include "constellation_model.h"

#include <algorithm>
#include <cmath>

namespace constellation {
namespace {
constexpr float safe_inset = 12.0f;  // 8 px half-extent + 2 px half-stroke + 2 px margin.
bool finite(float value) { return std::isfinite(value); }
bool finite(Vec2 value) { return finite(value.x) && finite(value.y); }
bool colorValid(Color color) {
    return color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
bool colorsDiffer(Color first, Color second) {
    return first.r != second.r || first.g != second.g || first.b != second.b;
}
Vec2 clampPoint(Vec2 point, Viewport viewport) {
    return {std::clamp(point.x, safe_inset, static_cast<float>(viewport.width) - safe_inset),
            std::clamp(point.y, safe_inset, static_cast<float>(viewport.height) - safe_inset)};
}
bool pointInBounds(Vec2 point, Viewport viewport) {
    const float epsilon = 0.001f;
    return finite(point) && point.x >= safe_inset - epsilon && point.y >= safe_inset - epsilon &&
           point.x <= static_cast<float>(viewport.width) - safe_inset + epsilon &&
           point.y <= static_cast<float>(viewport.height) - safe_inset + epsilon;
}
}  // namespace

Vec2 subtract(Vec2 to, Vec2 from) { return {to.x - from.x, to.y - from.y}; }
Vec2 add(Vec2 first, Vec2 second) { return {first.x + second.x, first.y + second.y}; }
Vec2 scale(Vec2 value, float amount) {
    if (!finite(value) || !finite(amount)) return {};
    return {value.x * amount, value.y * amount};
}
float dot(Vec2 first, Vec2 second) {
    if (!finite(first) || !finite(second)) return 0.0f;
    const float result = first.x * second.x + first.y * second.y;
    return finite(result) ? result : 0.0f;
}
float length(Vec2 value) {
    if (!finite(value)) return 0.0f;
    const float result = std::hypot(value.x, value.y);
    return finite(result) ? result : 0.0f;
}
float distance(Vec2 first, Vec2 second) { return length(subtract(first, second)); }
Vec2 normalizeOrZero(Vec2 value) {
    const float magnitude = length(value);
    if (magnitude <= 0.0f) return {};
    return scale(value, 1.0f / magnitude);
}
Vec2 moveToward(Vec2 start, Vec2 target, float max_step) {
    if (!finite(start)) return {};
    if (!finite(target) || !finite(max_step) || max_step <= 0.0f) return start;
    const Vec2 direction = subtract(target, start);
    return add(start, scale(normalizeOrZero(direction), std::min(length(direction), max_step)));
}

bool designIsValid(const Design& design) {
    return finite(design.anchor_x) && design.anchor_x >= 0.15f && design.anchor_x <= 0.85f &&
           finite(design.anchor_y) && design.anchor_y >= 0.15f && design.anchor_y <= 0.85f &&
           finite(design.reach) && design.reach >= 24.0f && design.reach <= 200.0f &&
           colorValid(design.ink) && colorValid(design.accent) && colorValid(design.background) &&
           colorsDiffer(design.ink, design.background) &&
           colorsDiffer(design.accent, design.background);
}

Scene makeScene(const Design& design, Viewport viewport, Vec2 requested_target) {
    Scene scene{};
    if (!designIsValid(design) || viewport.width < 64 || viewport.height < 64 ||
        !finite(requested_target)) return scene;
    const float usable_width = static_cast<float>(viewport.width) - 2.0f * safe_inset;
    const float usable_height = static_cast<float>(viewport.height) - 2.0f * safe_inset;
    scene.anchor = {safe_inset + design.anchor_x * usable_width,
                    safe_inset + design.anchor_y * usable_height};
    scene.target = clampPoint(requested_target, viewport);
    scene.direction = subtract(scene.target, scene.anchor);
    scene.distance = length(scene.direction);
    scene.unit_direction = normalizeOrZero(scene.direction);
    scene.traveler = moveToward(scene.anchor, scene.target, design.reach);
    const Vec2 perpendicular = {-scene.unit_direction.y, scene.unit_direction.x};
    const float spread = std::min(24.0f, scene.distance * 0.15f);
    scene.constellation_left = clampPoint(add(scene.traveler, scale(perpendicular, spread)), viewport);
    scene.constellation_right = clampPoint(add(scene.traveler, scale(perpendicular, -spread)), viewport);
    scene.valid = true;
    return scene;
}

bool sceneIsFiniteAndInBounds(const Scene& scene, Viewport viewport) {
    if (!scene.valid || viewport.width < 64 || viewport.height < 64 ||
        !finite(scene.direction) || !finite(scene.unit_direction) || !finite(scene.distance)) return false;
    const float unit_length = length(scene.unit_direction);
    const bool unit_policy = scene.distance <= 0.0f ? unit_length == 0.0f :
                                                     std::fabs(unit_length - 1.0f) <= 0.00001f;
    return unit_policy && scene.distance >= 0.0f && pointInBounds(scene.anchor, viewport) &&
           pointInBounds(scene.target, viewport) && pointInBounds(scene.traveler, viewport) &&
           pointInBounds(scene.constellation_left, viewport) &&
           pointInBounds(scene.constellation_right, viewport);
}
}  // namespace constellation
