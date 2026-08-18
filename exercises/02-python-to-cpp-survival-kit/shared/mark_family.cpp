#include "mark_family.h"

#include <algorithm>
#include <cmath>

namespace markfamily {
namespace {
bool finite(float value) { return std::isfinite(value); }
bool colorIsValid(Color color) {
    return color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
bool colorsDiffer(Color a, Color b) { return a.r != b.r || a.g != b.g || a.b != b.b; }
}  // namespace

bool designIsValid(const Design& design) {
    return design.count >= 3 && design.count <= 24 && finite(design.normalized_center.x) &&
           finite(design.normalized_center.y) && design.normalized_center.x >= 0.1f &&
           design.normalized_center.x <= 0.9f && design.normalized_center.y >= 0.1f &&
           design.normalized_center.y <= 0.9f && finite(design.spacing_pixels) &&
           design.spacing_pixels >= 8.0f && design.spacing_pixels <= 80.0f &&
           finite(design.base_radius_pixels) && design.base_radius_pixels >= 3.0f &&
           design.base_radius_pixels <= 40.0f && finite(design.radius_step_pixels) &&
           design.radius_step_pixels >= -1.0f && design.radius_step_pixels <= 3.0f &&
           colorIsValid(design.mark_color) && colorIsValid(design.accent_color) &&
           colorIsValid(design.background_color) &&
           colorsDiffer(design.mark_color, design.background_color);
}

std::vector<Mark> makeMarkFamily(const Design& design, Viewport viewport) {
    std::vector<Mark> marks;
    if (!designIsValid(design) || viewport.width < 2 || viewport.height < 2) return marks;

    marks.reserve(static_cast<std::size_t>(design.count));
    const float center_x = design.normalized_center.x * static_cast<float>(viewport.width);
    const float center_y = design.normalized_center.y * static_cast<float>(viewport.height);
    const float requested_half_span = design.spacing_pixels * static_cast<float>(design.count - 1) * 0.5f;
    const float available_half_span = std::max(0.0f, std::min(center_x, static_cast<float>(viewport.width) - center_x));
    const float half_span = std::min(requested_half_span, available_half_span);
    const float step = design.count > 1 ? (2.0f * half_span) / static_cast<float>(design.count - 1) : 0.0f;
    const float maximum_radius = std::max(1.0f, 0.2f * static_cast<float>(std::min(viewport.width, viewport.height)));

    for (int index = 0; index < design.count; ++index) {
        const float centered_index = static_cast<float>(index) - static_cast<float>(design.count - 1) * 0.5f;
        const float radius = std::clamp(design.base_radius_pixels +
                                            std::fabs(centered_index) * design.radius_step_pixels,
                                        1.0f, maximum_radius);
        const float wave = (index % 2 == 0 ? -1.0f : 1.0f) * std::min(design.spacing_pixels * 0.25f,
                                                                      static_cast<float>(viewport.height) * 0.08f);
        marks.push_back({
            {std::clamp(center_x + centered_index * step, radius,
                        std::max(radius, static_cast<float>(viewport.width) - radius)),
             std::clamp(center_y + wave, radius,
                        std::max(radius, static_cast<float>(viewport.height) - radius))},
            radius,
            index,
        });
    }
    return marks;
}

bool marksAreFiniteAndInBounds(const std::vector<Mark>& marks, Viewport viewport) {
    if (viewport.width < 2 || viewport.height < 2) return marks.empty();
    for (const Mark& mark : marks) {
        if (!finite(mark.center.x) || !finite(mark.center.y) || !finite(mark.radius) || mark.radius < 0.0f ||
            mark.center.x - mark.radius < -0.0001f || mark.center.y - mark.radius < -0.0001f ||
            mark.center.x + mark.radius > static_cast<float>(viewport.width) + 0.0001f ||
            mark.center.y + mark.radius > static_cast<float>(viewport.height) + 0.0001f) return false;
    }
    return true;
}
}  // namespace markfamily
