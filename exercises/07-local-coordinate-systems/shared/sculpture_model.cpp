#include "sculpture_model.h"

#include <algorithm>
#include <cmath>

namespace sculpture {
namespace {
bool finite(float value) { return std::isfinite(value); }
bool colorValid(Color color) {
    return color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
}
Mat3 identity() { return {{{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}}}; }
Mat3 translation(float x, float y) {
    if (!finite(x) || !finite(y)) return identity();
    return {{{1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f}}};
}
Mat3 rotationDegrees(float degrees) {
    if (!finite(degrees)) return identity();
    const float radians = degrees * pi / 180.0f;
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);
    // Positive y points down on screen, so this standard matrix looks clockwise.
    return {{{cosine, -sine, 0.0f, sine, cosine, 0.0f, 0.0f, 0.0f, 1.0f}}};
}
Mat3 scaling(float x, float y) {
    if (!finite(x) || !finite(y)) return identity();
    return {{{x, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 1.0f}}};
}
Mat3 multiply(const Mat3& left, const Mat3& right) {
    Mat3 result{{{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}};
    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 3; ++column) {
            for (int inner = 0; inner < 3; ++inner) {
                result.values[static_cast<std::size_t>(row * 3 + column)] +=
                    left.values[static_cast<std::size_t>(row * 3 + inner)] *
                    right.values[static_cast<std::size_t>(inner * 3 + column)];
            }
        }
    }
    return result;
}
Vec2 transformPoint(const Mat3& transform, Vec2 point) {
    return {
        transform.values[0] * point.x + transform.values[1] * point.y + transform.values[2],
        transform.values[3] * point.x + transform.values[4] * point.y + transform.values[5]};
}
float wrapTime(float time_seconds, float period_seconds) {
    if (!finite(time_seconds) || !finite(period_seconds) || period_seconds <= 0.0f) return 0.0f;
    float wrapped = std::fmod(time_seconds, period_seconds);
    if (wrapped < 0.0f) wrapped += period_seconds;
    return wrapped;
}
bool designIsValid(const Design& design) {
    return finite(design.period_seconds) && design.period_seconds > 0.0f &&
           finite(design.first_length) && design.first_length > 0.0f &&
           finite(design.second_length) && design.second_length > 0.0f &&
           finite(design.ornament_radius) && design.ornament_radius > 0.0f &&
           finite(design.stroke_width) && design.stroke_width > 0.0f &&
           finite(design.root_swing_degrees) && finite(design.child_swing_degrees) &&
           colorValid(design.ink) && colorValid(design.accent) && colorValid(design.background);
}
Scene makeScene(const Design& design, Viewport viewport, float time_seconds) {
    Scene scene{false, 0.0f, 0.0f, 0.0f, 0.0f, identity(), identity(), {}, {}, {}};
    if (!designIsValid(design) || viewport.width < 48 || viewport.height < 48) return scene;
    scene.time_seconds = wrapTime(time_seconds, design.period_seconds);
    const float phase = tau * scene.time_seconds / design.period_seconds;
    scene.root_angle_degrees = design.root_swing_degrees * std::sin(phase);
    scene.child_angle_degrees = design.child_swing_degrees * std::cos(phase);
    const float margin = design.stroke_width * 0.5f + 2.0f;
    const float reach = design.first_length + design.second_length + design.ornament_radius;
    scene.model_scale = (0.5f * static_cast<float>(std::min(viewport.width, viewport.height)) - margin) / reach;
    if (!finite(scene.model_scale) || scene.model_scale <= 0.0f) return scene;
    const Mat3 viewport_transform = multiply(
        translation(static_cast<float>(viewport.width) * 0.5f,
                    static_cast<float>(viewport.height) * 0.5f),
        scaling(scene.model_scale, scene.model_scale));
    scene.root_transform = multiply(viewport_transform, rotationDegrees(scene.root_angle_degrees));
    scene.child_transform = multiply(
        multiply(scene.root_transform, translation(design.first_length, 0.0f)),
        rotationDegrees(scene.child_angle_degrees));
    scene.pivot = transformPoint(scene.root_transform, {0.0f, 0.0f});
    scene.elbow = transformPoint(scene.root_transform, {design.first_length, 0.0f});
    scene.tip = transformPoint(scene.child_transform, {design.second_length, 0.0f});
    scene.valid = sceneIsFiniteAndInBounds(scene, design, viewport);
    return scene;
}
bool sceneIsFiniteAndInBounds(const Scene& scene, const Design& design, Viewport viewport) {
    if (viewport.width < 48 || viewport.height < 48 || !designIsValid(design) ||
        !finite(scene.pivot.x) || !finite(scene.pivot.y) || !finite(scene.elbow.x) ||
        !finite(scene.elbow.y) || !finite(scene.tip.x) || !finite(scene.tip.y) ||
        !finite(scene.model_scale) || scene.model_scale <= 0.0f) return false;
    const float extent = design.ornament_radius * scene.model_scale +
                         design.stroke_width * 0.5f + 2.0f;
    for (const Vec2 anchor : {scene.pivot, scene.elbow, scene.tip}) {
        if (anchor.x - extent < -0.002f || anchor.y - extent < -0.002f ||
            anchor.x + extent > static_cast<float>(viewport.width) + 0.002f ||
            anchor.y + extent > static_cast<float>(viewport.height) + 0.002f) return false;
    }
    return true;
}
}  // namespace sculpture
