#include "gesture_model.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace gesture {
namespace {
bool finite(float value) { return std::isfinite(value); }
bool finite(Vec2 value) { return finite(value.x) && finite(value.y); }
bool colorValid(Color color) {
    return color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
bool asFiniteFloat(double value, float& result) {
    if (!std::isfinite(value) || std::fabs(value) > std::numeric_limits<float>::max()) return false;
    result = static_cast<float>(value);
    return std::isfinite(result);
}
bool checkedDistance(Vec2 a, Vec2 b, float& result) {
    if (!finite(a) || !finite(b)) return false;
    return asFiniteFloat(std::hypot(static_cast<double>(a.x) - b.x,
                                    static_cast<double>(a.y) - b.y), result);
}
bool samePoint(Vec2 a, Vec2 b) { return a.x == b.x && a.y == b.y; }
bool rebuildDerived(Gesture& gesture, const Design& design) {
    std::vector<Sample> rebuilt;
    rebuilt.reserve(gesture.samples.capacity());
    for (const Sample& source : gesture.samples) {
        Sample next{source.raw, source.raw, source.time_seconds, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f};
        if (!rebuilt.empty()) {
            const Sample& previous = rebuilt.back();
            const float dt = source.time_seconds - previous.time_seconds;
            next.position = exponentialSmooth(previous.position, source.raw, design.smoothing_k, dt);
            if (!finite(next.position)) return false;
            next.velocity = guardedVelocity(previous.position, next.position, dt);
            if (!finite(next.velocity) || !checkedDistance({0.0f, 0.0f}, next.velocity, next.speed)) return false;
            float segment_length = 0.0f;
            if (!checkedDistance(previous.position, next.position, segment_length) ||
                !asFiniteFloat(static_cast<double>(previous.arc_length) + segment_length,
                               next.arc_length)) return false;
            if (!finite(dt) || dt <= 0.0f) next.time_seconds = previous.time_seconds;
        }
        if (rebuilt.size() >= 2) {
            next.signed_turn_radians = signedTurningAngle(
                rebuilt[rebuilt.size() - 2].position, rebuilt.back().position, next.position);
            if (!finite(next.signed_turn_radians)) return false;
        }
        rebuilt.push_back(next);
    }
    gesture.samples.swap(rebuilt);
    return true;
}
}
float distance(Vec2 a, Vec2 b) {
    float result = 0.0f;
    return checkedDistance(a, b, result) ? result : 0.0f;
}
Vec2 exponentialSmooth(Vec2 current, Vec2 target, float smoothing_k, float dt) {
    if (!finite(current) || !finite(target) || !finite(smoothing_k) || smoothing_k < 0.0f ||
        !finite(dt) || dt <= 0.0f) return current;
    const double alpha = 1.0 - std::exp(-static_cast<double>(smoothing_k) * dt);
    float x = 0.0f, y = 0.0f;
    if (!asFiniteFloat(static_cast<double>(current.x) + alpha *
                       (static_cast<double>(target.x) - current.x), x) ||
        !asFiniteFloat(static_cast<double>(current.y) + alpha *
                       (static_cast<double>(target.y) - current.y), y)) return current;
    return {x, y};
}
Vec2 guardedVelocity(Vec2 previous, Vec2 current, float dt) {
    if (!finite(previous) || !finite(current) || !finite(dt) || dt <= 0.0f) return {0.0f, 0.0f};
    float x = 0.0f, y = 0.0f;
    if (!asFiniteFloat((static_cast<double>(current.x) - previous.x) / dt, x) ||
        !asFiniteFloat((static_cast<double>(current.y) - previous.y) / dt, y)) return {0.0f, 0.0f};
    return {x, y};
}
float signedTurningAngle(Vec2 first, Vec2 middle, Vec2 last) {
    if (!finite(first) || !finite(middle) || !finite(last)) return 0.0f;
    const double incoming_x = static_cast<double>(middle.x) - first.x;
    const double incoming_y = static_cast<double>(middle.y) - first.y;
    const double outgoing_x = static_cast<double>(last.x) - middle.x;
    const double outgoing_y = static_cast<double>(last.y) - middle.y;
    const double incoming_length = std::hypot(incoming_x, incoming_y);
    const double outgoing_length = std::hypot(outgoing_x, outgoing_y);
    if (!std::isfinite(incoming_length) || !std::isfinite(outgoing_length) ||
        incoming_length <= 0.00001 || outgoing_length <= 0.00001) return 0.0f;
    const double cross = incoming_x * outgoing_y - incoming_y * outgoing_x;
    const double dot = incoming_x * outgoing_x + incoming_y * outgoing_y;
    const double angle = std::atan2(cross, dot);
    return std::isfinite(angle) ? static_cast<float>(angle) : 0.0f;
}
std::vector<Vec2> uniformResample(const std::vector<Vec2>& points, float spacing) {
    std::vector<Vec2> clean;
    if (points.size() > maximum_resampled_points) return clean;
    clean.reserve(points.size());
    for (const Vec2 point : points) {
        if (!finite(point)) continue;
        if (clean.empty() || !samePoint(clean.back(), point)) clean.push_back(point);
    }
    if (clean.size() < 2) return clean;
    if (!finite(spacing) || spacing < minimum_resample_spacing) return {};
    std::vector<double> cumulative(clean.size(), 0.0);
    for (std::size_t i = 1; i < clean.size(); ++i) {
        const double segment = std::hypot(static_cast<double>(clean[i].x) - clean[i - 1].x,
                                          static_cast<double>(clean[i].y) - clean[i - 1].y);
        cumulative[i] = cumulative[i - 1] + segment;
        if (!std::isfinite(cumulative[i])) return {};
    }
    const double total = cumulative.back();
    if (total == 0.0) return {clean.front()};
    const double interior_count = std::ceil(total / spacing) - 1.0;
    if (!std::isfinite(interior_count) || interior_count > maximum_resampled_points - 2) return {};
    const std::size_t expected = static_cast<std::size_t>(interior_count) + 2;
    std::vector<Vec2> result;
    result.reserve(expected);
    result.push_back(clean.front());
    std::size_t segment = 1;
    for (std::size_t i = 1; i <= static_cast<std::size_t>(interior_count); ++i) {
        const double target = static_cast<double>(spacing) * static_cast<double>(i);
        if (target >= total) break;
        while (segment < cumulative.size() - 1 && cumulative[segment] < target) ++segment;
        const double segment_length = cumulative[segment] - cumulative[segment - 1];
        if (!(segment_length > 0.0) || !std::isfinite(segment_length)) return {};
        const double amount = (target - cumulative[segment - 1]) / segment_length;
        float x = 0.0f, y = 0.0f;
        if (!asFiniteFloat(static_cast<double>(clean[segment - 1].x) + amount *
                           (static_cast<double>(clean[segment].x) - clean[segment - 1].x), x) ||
            !asFiniteFloat(static_cast<double>(clean[segment - 1].y) + amount *
                           (static_cast<double>(clean[segment].y) - clean[segment - 1].y), y)) return {};
        result.push_back({x, y});
    }
    if (!samePoint(result.back(), clean.back())) result.push_back(clean.back());
    return result;
}
bool designIsValid(const Design& design) {
    return finite(design.minimum_distance) && design.minimum_distance >= 0.0f &&
           finite(design.smoothing_k) && design.smoothing_k >= 0.0f &&
           finite(design.resample_spacing) &&
           design.resample_spacing >= minimum_resample_spacing &&
           finite(design.minimum_width) && design.minimum_width > 0.0f &&
           finite(design.maximum_width) && design.maximum_width >= design.minimum_width &&
           design.maximum_samples >= 3 && design.maximum_samples <= maximum_resampled_points &&
           colorValid(design.slow_color) && colorValid(design.fast_color) &&
           colorValid(design.background);
}
bool addSample(Gesture& gesture, Vec2 raw, float time_seconds, const Design& design) {
    if (!designIsValid(design) || !finite(raw) || !finite(time_seconds)) return false;
    if (!gesture.samples.empty()) {
        float raw_distance = 0.0f;
        if (!checkedDistance(gesture.samples.back().raw, raw, raw_distance) ||
            raw_distance < design.minimum_distance) return false;
    }
    Gesture candidate = gesture;
    candidate.samples.reserve(std::max(gesture.samples.capacity(), gesture.samples.size() + 1));
    candidate.samples.push_back({raw, raw, time_seconds, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f});
    if (candidate.samples.size() > design.maximum_samples) {
        const std::size_t excess = candidate.samples.size() - design.maximum_samples;
        candidate.samples.erase(candidate.samples.begin(),
                                candidate.samples.begin() + static_cast<std::ptrdiff_t>(excess));
    }
    if (!rebuildDerived(candidate, design)) return false;
    gesture.samples.swap(candidate.samples);
    return true;
}
void clear(Gesture& gesture) { gesture.samples.clear(); }
Style styleForSample(const Sample& sample, const Design& design) {
    const float speed_amount = std::clamp(sample.speed / 240.0f, 0.0f, 1.0f);
    const float turn_amount = std::clamp(std::fabs(sample.signed_turn_radians) / pi, 0.0f, 1.0f);
    const float width = design.maximum_width + speed_amount * (design.minimum_width - design.maximum_width);
    const auto channel = [turn_amount](int low, int high) {
        return static_cast<int>(std::lround(low + turn_amount * (high - low)));
    };
    return {width, {channel(design.slow_color.r, design.fast_color.r),
                    channel(design.slow_color.g, design.fast_color.g),
                    channel(design.slow_color.b, design.fast_color.b)}};
}
bool facetTriangle(Vec2 start, Vec2 end, float width, Triangle& triangle) {
    float length = 0.0f;
    if (!finite(start) || !finite(end) || !finite(width) || width <= 0.0f ||
        !checkedDistance(start, end, length) || length <= 0.0001f) return false;
    const double half_width = static_cast<double>(width) * 0.5;
    const double nx = -(static_cast<double>(end.y) - start.y) / length * half_width;
    const double ny = (static_cast<double>(end.x) - start.x) / length * half_width;
    if (!asFiniteFloat(static_cast<double>(start.x) - nx, triangle.first.x) ||
        !asFiniteFloat(static_cast<double>(start.y) - ny, triangle.first.y) ||
        !asFiniteFloat(static_cast<double>(start.x) + nx, triangle.second.x) ||
        !asFiniteFloat(static_cast<double>(start.y) + ny, triangle.second.y)) return false;
    triangle.tip = end;
    return true;
}
bool finiteAndStrokeAwareInBounds(const Gesture& gesture, int width, int height,
                                  float maximum_stroke_width) {
    if (width <= 0 || height <= 0 || !finite(maximum_stroke_width) || maximum_stroke_width <= 0.0f)
        return false;
    const float radius = maximum_stroke_width * 0.5f;
    for (const Sample& sample : gesture.samples) {
        if (!finite(sample.raw) || !finite(sample.position) || !finite(sample.velocity) ||
            !finite(sample.speed) || !finite(sample.signed_turn_radians) ||
            !finite(sample.arc_length) || sample.position.x - radius < 0.0f ||
            sample.position.y - radius < 0.0f ||
            sample.position.x + radius > static_cast<float>(width) ||
            sample.position.y + radius > static_cast<float>(height)) return false;
    }
    return true;
}
}  // namespace gesture
