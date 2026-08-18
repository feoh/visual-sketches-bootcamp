#include "poster_layout.h"

#include <algorithm>
#include <cmath>

namespace responsiveposter {
namespace {
bool finite(float value) { return std::isfinite(value); }
bool colorValid(Color color) {
    return color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
bool colorsDiffer(Color a, Color b) {
    return a.r != b.r || a.g != b.g || a.b != b.b;
}
}  // namespace

float clamp01(float value) {
    if (!finite(value)) return 0.0f;
    return std::clamp(value, 0.0f, 1.0f);
}

float normalizeClamped(float value, float input_min, float input_max) {
    if (!finite(value) || !finite(input_min) || !finite(input_max) || input_max <= input_min)
        return 0.0f;
    return clamp01((value - input_min) / (input_max - input_min));
}

float lerpClamped(float start, float end, float amount) {
    if (!finite(start) || !finite(end)) return 0.0f;
    return start + (end - start) * clamp01(amount);
}

float mapClamped(float value, float input_min, float input_max,
                 float output_min, float output_max) {
    return lerpClamped(output_min, output_max,
                       normalizeClamped(value, input_min, input_max));
}

float smoothstep(float amount) {
    const float t = clamp01(amount);
    return t * t * (3.0f - 2.0f * t);
}

bool designIsValid(const Design& design) {
    return finite(design.focus) && design.focus >= 0.0f && design.focus <= 1.0f &&
           finite(design.vertical_bias) && design.vertical_bias >= 0.2f &&
           design.vertical_bias <= 0.8f && colorValid(design.ink) &&
           colorValid(design.accent) && colorValid(design.background) &&
           colorsDiffer(design.ink, design.background) &&
           colorsDiffer(design.accent, design.background);
}

Layout makeLayout(const Design& design, Viewport viewport) {
    Layout layout{};
    if (!designIsValid(design) || viewport.width < 64 || viewport.height < 64) return layout;

    const float width = static_cast<float>(viewport.width);
    const float height = static_cast<float>(viewport.height);
    const float short_side = std::min(width, height);
    const float scale_amount = normalizeClamped(short_side, 320.0f, 1200.0f);
    const float padding = lerpClamped(16.0f, 64.0f, scale_amount);
    const float available_width = width - 2.0f * padding;
    const float available_height = height - 2.0f * padding;
    constexpr float poster_aspect = 4.0f / 5.0f;
    float panel_width = available_width;
    float panel_height = panel_width / poster_aspect;
    if (panel_height > available_height) {
        panel_height = available_height;
        panel_width = panel_height * poster_aspect;
    }
    layout.panel = {(width - panel_width) * 0.5f, (height - panel_height) * 0.5f,
                    panel_width, panel_height};
    const float eased = smoothstep(scale_amount);
    layout.headline_size = lerpClamped(24.0f, 72.0f, eased);
    layout.focus_radius = lerpClamped(10.0f, 24.0f, eased);
    const float left = layout.panel.x + layout.focus_radius;
    const float right = layout.panel.x + layout.panel.width - layout.focus_radius;
    const float top = layout.panel.y + layout.focus_radius;
    const float bottom = layout.panel.y + layout.panel.height - layout.focus_radius;
    layout.focus_center = {
        lerpClamped(left, right, design.focus),
        std::clamp(layout.panel.y + design.vertical_bias * layout.panel.height, top, bottom)};
    layout.valid = true;
    return layout;
}

float availableFocusDistance(const Layout& layout) {
    if (!layout.valid || !finite(layout.panel.x) || !finite(layout.panel.y) ||
        !finite(layout.panel.width) || !finite(layout.panel.height) ||
        !finite(layout.focus_center.x) || !finite(layout.focus_center.y))
        return 0.0f;
    const float left = layout.focus_center.x - layout.panel.x;
    const float right = layout.panel.x + layout.panel.width - layout.focus_center.x;
    const float top = layout.focus_center.y - layout.panel.y;
    const float bottom = layout.panel.y + layout.panel.height - layout.focus_center.y;
    return std::max(0.0f, std::min({left, right, top, bottom}));
}

bool layoutIsFiniteAndInBounds(const Layout& layout, Viewport viewport) {
    if (!layout.valid) return false;
    const float width = static_cast<float>(viewport.width);
    const float height = static_cast<float>(viewport.height);
    const float epsilon = 0.001f;
    return viewport.width >= 64 && viewport.height >= 64 && finite(layout.panel.x) &&
           finite(layout.panel.y) && finite(layout.panel.width) && finite(layout.panel.height) &&
           finite(layout.focus_center.x) && finite(layout.focus_center.y) &&
           finite(layout.focus_radius) && finite(layout.headline_size) &&
           layout.panel.width > 0.0f && layout.panel.height > 0.0f &&
           layout.panel.x >= -epsilon && layout.panel.y >= -epsilon &&
           layout.panel.x + layout.panel.width <= width + epsilon &&
           layout.panel.y + layout.panel.height <= height + epsilon &&
           layout.focus_radius > 0.0f &&
           layout.focus_center.x - layout.focus_radius >= layout.panel.x - epsilon &&
           layout.focus_center.x + layout.focus_radius <= layout.panel.x + layout.panel.width + epsilon &&
           layout.focus_center.y - layout.focus_radius >= layout.panel.y - epsilon &&
           layout.focus_center.y + layout.focus_radius <= layout.panel.y + layout.panel.height + epsilon;
}
}  // namespace responsiveposter
