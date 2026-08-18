#include "color_trail_model.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace colortrail {
namespace {

constexpr std::size_t absolute_history_limit = 4096;

bool finite(float value) {
    return std::isfinite(value);
}

bool finite(Vec2 value) {
    return finite(value.x) && finite(value.y);
}

bool unit(float value) {
    return finite(value) && value >= 0.0f && value <= 1.0f;
}

bool checkedFloat(double value, float& output) {
    if (!std::isfinite(value) ||
        std::fabs(value) > std::numeric_limits<float>::max()) {
        return false;
    }
    output = static_cast<float>(value);
    return finite(output);
}

bool checkedLerp(float first, float second, float amount, float& output) {
    return checkedFloat(static_cast<double>(first) +
                            (static_cast<double>(second) - first) * amount,
                        output);
}

}  // namespace

bool colorIsValid(Color color) {
    return unit(color.r) && unit(color.g) && unit(color.b) && unit(color.a);
}

bool paletteIsValid(const Palette& palette) {
    return colorIsValid(palette.first) && colorIsValid(palette.second);
}

bool designIsValid(const Design& design) {
    return design.maximum_history >= 2 &&
           design.maximum_history <= absolute_history_limit &&
           unit(design.alpha_retention) &&
           design.alpha_retention > 0.0f &&
           unit(design.trail_opacity) &&
           design.trail_opacity > 0.0f &&
           finite(design.minimum_radius) && design.minimum_radius > 0.0f &&
           finite(design.maximum_radius) &&
           design.maximum_radius >= design.minimum_radius &&
           design.maximum_radius <= 10000.0f &&
           colorIsValid(design.background) &&
           paletteIsValid(design.palettes[0]) &&
           paletteIsValid(design.palettes[1]);
}

bool sampleIsValid(const TrailSample& sample) {
    return finite(sample.position) &&
           std::fabs(sample.position.x) <= 1000000.0f &&
           std::fabs(sample.position.y) <= 1000000.0f &&
           unit(sample.palette_amount) && sample.palette_index < 2;
}

bool interpolateColor(Color first, Color second, float amount, Color& output) {
    if (!colorIsValid(first) || !colorIsValid(second) || !unit(amount)) {
        return false;
    }
    Color next{};
    if (!checkedLerp(first.r, second.r, amount, next.r) ||
        !checkedLerp(first.g, second.g, amount, next.g) ||
        !checkedLerp(first.b, second.b, amount, next.b) ||
        !checkedLerp(first.a, second.a, amount, next.a) ||
        !colorIsValid(next)) {
        return false;
    }
    output = next;
    return true;
}

bool paletteColor(const Palette& palette, float amount, Color& output) {
    return paletteIsValid(palette) &&
           interpolateColor(palette.first, palette.second, amount, output);
}

bool sourceOver(Color foreground, Color background, Color& output) {
    if (!colorIsValid(foreground) || !colorIsValid(background)) {
        return false;
    }
    const double foreground_alpha = foreground.a;
    const double background_weight = background.a * (1.0 - foreground_alpha);
    const double output_alpha = foreground_alpha + background_weight;
    Color next{};
    if (!checkedFloat(output_alpha, next.a)) {
        return false;
    }
    if (output_alpha == 0.0) {
        next = {0.0f, 0.0f, 0.0f, 0.0f};
    } else if (!checkedFloat((foreground.r * foreground_alpha +
                              background.r * background_weight) / output_alpha,
                             next.r) ||
               !checkedFloat((foreground.g * foreground_alpha +
                              background.g * background_weight) / output_alpha,
                             next.g) ||
               !checkedFloat((foreground.b * foreground_alpha +
                              background.b * background_weight) / output_alpha,
                             next.b)) {
        return false;
    }
    if (!colorIsValid(next)) {
        return false;
    }
    output = next;
    return true;
}

bool decayAlpha(float initial_alpha,
                float retention_per_step,
                std::size_t age_steps,
                float& output) {
    if (!unit(initial_alpha) || !unit(retention_per_step)) {
        return false;
    }
    const double decayed = static_cast<double>(initial_alpha) *
                           std::pow(static_cast<double>(retention_per_step),
                                    static_cast<double>(age_steps));
    float next = 0.0f;
    if (!checkedFloat(decayed, next) || !unit(next)) {
        return false;
    }
    output = next;
    return true;
}

bool appendSample(std::vector<TrailSample>& history,
                  TrailSample sample,
                  std::size_t maximum_history) {
    if (!sampleIsValid(sample) || maximum_history < 2 ||
        maximum_history > absolute_history_limit ||
        history.size() > maximum_history) {
        return false;
    }
    for (const TrailSample& existing : history) {
        if (!sampleIsValid(existing)) {
            return false;
        }
    }
    std::vector<TrailSample> next = history;
    next.push_back(sample);
    if (next.size() > maximum_history) {
        const std::size_t excess = next.size() - maximum_history;
        next.erase(next.begin(),
                   next.begin() + static_cast<std::ptrdiff_t>(excess));
    }
    history = std::move(next);
    return true;
}

bool buildTrailMarks(const std::vector<TrailSample>& history,
                     const Design& design,
                     std::vector<TrailMark>& output) {
    if (!designIsValid(design) || history.size() > design.maximum_history) {
        return false;
    }
    std::vector<TrailMark> next;
    next.reserve(history.size());
    for (std::size_t index = 0; index < history.size(); ++index) {
        const TrailSample& sample = history[index];
        if (!sampleIsValid(sample)) {
            return false;
        }
        const std::size_t age = history.size() - 1 - index;
        Color color{};
        if (!paletteColor(design.palettes[sample.palette_index],
                          sample.palette_amount, color)) {
            return false;
        }
        float alpha = 0.0f;
        if (!decayAlpha(color.a * design.trail_opacity,
                        design.alpha_retention, age, alpha)) {
            return false;
        }
        color.a = alpha;
        const float freshness = history.size() <= 1
                                    ? 1.0f
                                    : static_cast<float>(index) /
                                          static_cast<float>(history.size() - 1);
        float radius = 0.0f;
        if (!checkedLerp(design.minimum_radius, design.maximum_radius,
                         freshness, radius)) {
            return false;
        }
        next.push_back({sample.position, color, radius, age,
                        sample.palette_index});
    }
    if (!marksAreFinite(next, design)) {
        return false;
    }
    output = std::move(next);
    return true;
}

bool marksAreFinite(const std::vector<TrailMark>& marks,
                    const Design& design) {
    if (!designIsValid(design) || marks.size() > design.maximum_history) {
        return false;
    }
    std::size_t previous_age = marks.empty() ? 0 : marks.front().age_steps;
    for (std::size_t index = 0; index < marks.size(); ++index) {
        const TrailMark& mark = marks[index];
        if (!finite(mark.position) || !colorIsValid(mark.color) ||
            !finite(mark.radius) || mark.radius < design.minimum_radius ||
            mark.radius > design.maximum_radius || mark.palette_index >= 2 ||
            (index > 0 && mark.age_steps >= previous_age)) {
            return false;
        }
        previous_age = mark.age_steps;
    }
    return true;
}

}  // namespace colortrail
