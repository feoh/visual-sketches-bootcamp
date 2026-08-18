#include "temporal_history.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace temporal {
namespace {

bool finite(float value) { return std::isfinite(value); }
bool finite(double value) { return std::isfinite(value); }

bool sampleIsFinite(const Sample& sample) {
    return finite(sample.time_seconds) && sample.time_seconds >= 0.0 &&
           finite(sample.position.x) && finite(sample.position.y) &&
           finite(sample.phase);
}

bool shapeIsValid(const History& history) {
    return !history.storage.empty() &&
           history.storage.size() <= maximum_samples &&
           history.next < history.storage.size() &&
           history.count <= history.storage.size();
}

}  // namespace

bool checkedMemoryBytes(std::size_t requested_capacity, std::size_t& bytes) {
    if (requested_capacity == 0 || requested_capacity > maximum_samples ||
        requested_capacity > std::numeric_limits<std::size_t>::max() / sizeof(Sample)) {
        return false;
    }
    const std::size_t requested = requested_capacity * sizeof(Sample);
    if (requested > maximum_history_bytes) return false;
    bytes = requested;
    return true;
}

bool configure(History& history, std::size_t requested_capacity) {
    std::size_t bytes = 0;
    if (!checkedMemoryBytes(requested_capacity, bytes)) return false;
    (void)bytes;
    History next;
    next.storage.resize(requested_capacity);
    history = std::move(next);
    return true;
}

void reset(History& history) {
    if (history.storage.empty()) {
        history = History{};
        return;
    }
    std::fill(history.storage.begin(), history.storage.end(), Sample{});
    history.next = 0;
    history.count = 0;
}

std::size_t capacity(const History& history) { return history.storage.size(); }
std::size_t length(const History& history) { return history.count; }

bool valid(const History& history) {
    std::size_t bytes = 0;
    if (!shapeIsValid(history) ||
        !checkedMemoryBytes(history.storage.size(), bytes)) return false;
    (void)bytes;
    if (history.count == 0) return history.next == 0;

    Sample older;
    if (!atOldestIndex(history, 0, older)) return false;
    for (std::size_t index = 1; index < history.count; ++index) {
        Sample newer;
        if (!atOldestIndex(history, index, newer) ||
            newer.frame <= older.frame ||
            newer.time_seconds < older.time_seconds) return false;
        older = newer;
    }
    return true;
}

bool push(History& history, const Sample& sample) {
    if (!shapeIsValid(history) || !sampleIsFinite(sample)) return false;
    if (history.count > 0) {
        Sample newest;
        if (!atAge(history, 0, newest) || sample.frame <= newest.frame ||
            sample.time_seconds < newest.time_seconds) return false;
    }
    history.storage[history.next] = sample;
    history.next = (history.next + 1) % history.storage.size();
    history.count = std::min(history.count + 1, history.storage.size());
    return true;
}

bool atAge(const History& history, std::size_t age, Sample& sample) {
    if (!shapeIsValid(history) || age >= history.count) return false;
    const std::size_t index =
        (history.next + history.storage.size() - 1 - age) % history.storage.size();
    const Sample candidate = history.storage[index];
    if (!sampleIsFinite(candidate)) return false;
    sample = candidate;
    return true;
}

bool atOldestIndex(const History& history, std::size_t index, Sample& sample) {
    if (!shapeIsValid(history) || index >= history.count) return false;
    return atAge(history, history.count - 1 - index, sample);
}

bool ageFromPosition(float normalized_position,
                     std::size_t history_length,
                     std::size_t& age) {
    if (!finite(normalized_position) || normalized_position < 0.0f ||
        normalized_position > 1.0f || history_length == 0 ||
        history_length > maximum_samples) return false;
    const double scaled = static_cast<double>(normalized_position) *
                          static_cast<double>(history_length - 1);
    const double rounded = std::floor(scaled + 0.5);
    if (!finite(rounded) || rounded > static_cast<double>(history_length - 1)) return false;
    age = static_cast<std::size_t>(rounded);
    return true;
}

bool select(const History& history,
            float normalized_position,
            float decay_rate,
            Sample& sample,
            float& opacity) {
    if (!finite(decay_rate) || decay_rate < 0.0f || decay_rate > 16.0f) return false;
    std::size_t age = 0;
    if (!ageFromPosition(normalized_position, history.count, age)) return false;
    Sample candidate;
    if (!atAge(history, age, candidate)) return false;
    const double denominator = history.count > 1 ?
        static_cast<double>(history.count - 1) : 1.0;
    const double value = std::exp(-static_cast<double>(decay_rate) * age / denominator);
    if (!finite(value) || value < 0.0 || value > 1.0) return false;
    sample = candidate;
    opacity = static_cast<float>(value);
    return finite(opacity);
}

bool resize(History& history, std::size_t new_capacity) {
    std::size_t bytes = 0;
    if (!shapeIsValid(history) || !checkedMemoryBytes(new_capacity, bytes)) return false;
    (void)bytes;
    const std::size_t keep = std::min(history.count, new_capacity);
    History next;
    next.storage.resize(new_capacity);
    const std::size_t first_age = keep == 0 ? 0 : keep - 1;
    for (std::size_t offset = 0; offset < keep; ++offset) {
        Sample sample;
        if (!atAge(history, first_age - offset, sample) || !push(next, sample)) return false;
    }
    history = std::move(next);
    return true;
}

bool makeMotionSample(std::uint64_t frame,
                      double time_seconds,
                      float width,
                      float height,
                      Sample& sample) {
    if (!finite(time_seconds) || time_seconds < 0.0 ||
        !finite(width) || !finite(height) || width <= 0.0f || height <= 0.0f ||
        width > 1000000.0f || height > 1000000.0f) return false;
    const double phase = time_seconds * 1.7 + static_cast<double>(frame % 17u) * 0.013;
    const double x = static_cast<double>(width) * (0.5 + 0.34 * std::cos(phase));
    const double y = static_cast<double>(height) * (0.5 + 0.28 * std::sin(phase * 1.37));
    if (!finite(phase) || !finite(x) || !finite(y) ||
        std::fabs(phase) > std::numeric_limits<float>::max() ||
        std::fabs(x) > std::numeric_limits<float>::max() ||
        std::fabs(y) > std::numeric_limits<float>::max()) return false;
    sample = {frame, time_seconds,
              {static_cast<float>(x), static_cast<float>(y)},
              static_cast<float>(phase)};
    return sampleIsFinite(sample);
}

bool sameSample(const Sample& first, const Sample& second, float tolerance) {
    if (!sampleIsFinite(first) || !sampleIsFinite(second) ||
        !finite(tolerance) || tolerance < 0.0f) return false;
    return first.frame == second.frame &&
           std::fabs(first.time_seconds - second.time_seconds) <= tolerance &&
           std::fabs(first.position.x - second.position.x) <= tolerance &&
           std::fabs(first.position.y - second.position.y) <= tolerance &&
           std::fabs(first.phase - second.phase) <= tolerance;
}

}  // namespace temporal
