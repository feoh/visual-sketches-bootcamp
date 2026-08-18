#include "phase_field_model.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace phasefield {
namespace {
constexpr float stroke_half_width = 1.5f;
constexpr float outer_margin = 2.0f;
bool finite(float value) { return std::isfinite(value); }
bool finite(Vec2 value) { return finite(value.x) && finite(value.y); }
bool colorValid(Color color) {
    return color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
bool colorsDiffer(Color first, Color second) {
    return first.r != second.r || first.g != second.g || first.b != second.b;
}
float baseInset(const Design& design) {
    return design.amplitude + design.mark_radius + stroke_half_width + outer_margin;
}
float markInset(const Design& design) {
    return design.mark_radius + stroke_half_width + outer_margin;
}
float baseGeometryInset(const Design& design) {
    const float crosshair_arm = std::min(3.0f, design.mark_radius);
    return std::max(design.amplitude, crosshair_arm) +
           stroke_half_width + outer_margin;
}
float lerpForIndex(float first, float last, int index, int count) {
    if (count <= 1) return (first + last) * 0.5f;
    const float amount = static_cast<float>(index) / static_cast<float>(count - 1);
    return first + (last - first) * amount;
}
}  // namespace

float degreesToRadians(float degrees) {
    return finite(degrees) ? degrees * pi / 180.0f : 0.0f;
}
float radiansToDegrees(float radians) {
    return finite(radians) ? radians * 180.0f / pi : 0.0f;
}
Vec2 polarToCartesian(Polar polar) {
    if (!finite(polar.radius) || !finite(polar.angle_radians) || polar.radius < 0.0f) return {};
    return {polar.radius * std::cos(polar.angle_radians),
            polar.radius * std::sin(polar.angle_radians)};
}
Polar cartesianToPolar(Vec2 cartesian) {
    if (!finite(cartesian)) return {};
    const float radius = std::hypot(cartesian.x, cartesian.y);
    if (!finite(radius) || radius == 0.0f) return {};  // Zero vector has angle policy 0.
    return {radius, std::atan2(cartesian.y, cartesian.x)};
}
float oscillate(float amplitude, float frequency_hz, float time_seconds,
                float phase_radians) {
    if (!finite(amplitude) || !finite(frequency_hz) || !finite(time_seconds) ||
        !finite(phase_radians) || amplitude < 0.0f || frequency_hz < 0.0f) return 0.0f;
    return amplitude * std::sin(tau * frequency_hz * time_seconds + phase_radians);
}
int flatIndex(int row, int column, int columns) {
    if (row < 0 || column < 0 || columns <= 0 || column >= columns) return -1;
    if (row > (std::numeric_limits<int>::max() - column) / columns) return -1;
    return row * columns + column;
}

bool designIsValid(const Design& design) {
    return design.rows >= 2 && design.rows <= 16 &&
           design.columns >= 2 && design.columns <= 16 &&
           finite(design.amplitude) && design.amplitude >= 0.0f && design.amplitude <= 16.0f &&
           finite(design.frequency_hz) && design.frequency_hz >= 0.05f && design.frequency_hz <= 2.0f &&
           finite(design.row_phase_step) && finite(design.column_phase_step) &&
           std::fabs(design.row_phase_step) <= tau && std::fabs(design.column_phase_step) <= tau &&
           finite(design.mark_radius) && design.mark_radius >= 2.0f && design.mark_radius <= 8.0f &&
           colorValid(design.ink) && colorValid(design.accent) && colorValid(design.background) &&
           colorsDiffer(design.ink, design.background) &&
           colorsDiffer(design.accent, design.background);
}

Scene makeScene(const Design& design, Viewport viewport, float time_seconds) {
    Scene scene{};
    if (!designIsValid(design) || viewport.width < 64 || viewport.height < 64 ||
        !finite(time_seconds)) return scene;
    const float inset = baseInset(design);
    const float right = static_cast<float>(viewport.width) - inset;
    const float bottom = static_cast<float>(viewport.height) - inset;
    if (right < inset || bottom < inset) return scene;
    scene.time_seconds = time_seconds;
    scene.marks.reserve(static_cast<std::size_t>(design.rows * design.columns));
    for (int row = 0; row < design.rows; ++row) {
        for (int column = 0; column < design.columns; ++column) {
            const int index = flatIndex(row, column, design.columns);
            const Vec2 base{lerpForIndex(inset, right, column, design.columns),
                            lerpForIndex(inset, bottom, row, design.rows)};
            const float phase = tau * design.frequency_hz * time_seconds +
                                static_cast<float>(row) * design.row_phase_step +
                                static_cast<float>(column) * design.column_phase_step;
            const Vec2 displacement = polarToCartesian({design.amplitude, phase});
            scene.marks.push_back({row, column, index, base,
                                   {base.x + displacement.x, base.y + displacement.y}, phase});
        }
    }
    scene.valid = true;
    return scene;
}

bool sceneIsFiniteAndInBounds(const Scene& scene, const Design& design,
                              Viewport viewport) {
    if (!scene.valid || !designIsValid(design) || viewport.width < 64 || viewport.height < 64 ||
        !finite(scene.time_seconds) || scene.marks.size() !=
            static_cast<std::size_t>(design.rows * design.columns)) return false;
    const float mark_inset = markInset(design);
    const float base_geometry_inset = baseGeometryInset(design);
    const float epsilon = 0.002f;
    for (std::size_t i = 0; i < scene.marks.size(); ++i) {
        const Mark& mark = scene.marks[i];
        if (mark.row < 0 || mark.row >= design.rows || mark.column < 0 ||
            mark.column >= design.columns || mark.index != static_cast<int>(i) ||
            !finite(mark.base) || !finite(mark.center) || !finite(mark.phase_radians) ||
            mark.base.x < base_geometry_inset - epsilon ||
            mark.base.y < base_geometry_inset - epsilon ||
            mark.base.x > static_cast<float>(viewport.width) - base_geometry_inset + epsilon ||
            mark.base.y > static_cast<float>(viewport.height) - base_geometry_inset + epsilon ||
            mark.center.x < mark_inset - epsilon || mark.center.y < mark_inset - epsilon ||
            mark.center.x > static_cast<float>(viewport.width) - mark_inset + epsilon ||
            mark.center.y > static_cast<float>(viewport.height) - mark_inset + epsilon) return false;
    }
    return true;
}
}  // namespace phasefield
