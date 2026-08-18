#include "audio_input_model.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace embodied {
namespace {

bool finite(float value) { return std::isfinite(value); }

bool increment(std::uint64_t& value) {
    if (value == std::numeric_limits<std::uint64_t>::max()) return false;
    ++value;
    return true;
}

Geometry mapGeometry(float smoothed, const Design& design) {
    const float level = smoothed <= design.dead_zone
        ? 0.0f
        : (smoothed - design.dead_zone) / (1.0f - design.dead_zone);
    const float normalized = std::clamp(level, 0.0f, 1.0f);
    const float ray_span = static_cast<float>(design.maximum_rays - design.minimum_rays);
    const int rays = design.minimum_rays +
        static_cast<int>(std::floor(normalized * ray_span + 0.5f));
    return {
        normalized,
        design.minimum_radius +
            normalized * (design.maximum_radius - design.minimum_radius),
        std::clamp(rays, design.minimum_rays, design.maximum_rays),
        normalized > 0.0f
    };
}

}  // namespace

bool designIsValid(const Design& design) {
    const auto channelValid = [](int value) { return value >= 0 && value <= 255; };
    const auto colorValid = [&](Color color) {
        return channelValid(color.r) && channelValid(color.g) && channelValid(color.b);
    };
    return finite(design.smoothing) && design.smoothing > 0.0f &&
           design.smoothing <= 1.0f && finite(design.dead_zone) &&
           design.dead_zone >= 0.0f && design.dead_zone < 1.0f &&
           finite(design.minimum_radius) && design.minimum_radius >= 0.0f &&
           finite(design.maximum_radius) &&
           design.maximum_radius >= design.minimum_radius &&
           design.maximum_radius <= kMaximumRadius &&
           design.minimum_rays >= 0 &&
           design.maximum_rays >= design.minimum_rays &&
           design.maximum_rays <= 128 && colorValid(design.background) &&
           colorValid(design.quiet_color) && colorValid(design.active_color);
}

void reset(State& state, const Design& design, InputSource source) {
    state = {};
    state.source = source;
    if (designIsValid(design)) state.geometry = mapGeometry(0.0f, design);
}

bool consumeAmplitude(State& state, float amplitude, const Design& design) {
    if (!designIsValid(design) || !finite(amplitude) || amplitude < 0.0f ||
        state.accepted_samples == std::numeric_limits<std::uint64_t>::max()) {
        increment(state.rejected_samples);
        return false;
    }

    const float raw = std::min(amplitude, 1.0f);
    const float smoothed = state.smoothed_amplitude +
        design.smoothing * (raw - state.smoothed_amplitude);
    if (!finite(smoothed)) {
        increment(state.rejected_samples);
        return false;
    }

    state.raw_amplitude = raw;
    state.smoothed_amplitude = smoothed;
    state.geometry = mapGeometry(smoothed, design);
    ++state.accepted_samples;
    return true;
}

std::size_t consumeBatch(State& state,
                         const std::vector<float>& amplitudes,
                         const Design& design) {
    const std::size_t count = std::min(amplitudes.size(), kMaximumBatchSize);
    for (std::size_t index = 0; index < count; ++index) {
        consumeAmplitude(state, amplitudes[index], design);
    }
    const std::size_t dropped = amplitudes.size() - count;
    const auto available = std::numeric_limits<std::uint64_t>::max() -
                           state.dropped_samples;
    state.dropped_samples += std::min<std::uint64_t>(dropped, available);
    return count;
}

bool replayRecorded(State& state,
                    const std::vector<float>& amplitudes,
                    const Design& design) {
    if (!designIsValid(design) || amplitudes.size() > kMaximumFixtureSamples) {
        return false;
    }
    State candidate;
    reset(candidate, design, InputSource::recorded);
    for (float amplitude : amplitudes) {
        if (!consumeAmplitude(candidate, amplitude, design)) return false;
    }
    state = candidate;
    return true;
}

bool setNoDeviceFallback(State& state,
                         float keyboard_amplitude,
                         const Design& design) {
    if (!finite(keyboard_amplitude) || keyboard_amplitude < 0.0f) return false;
    State candidate = state;
    candidate.source = InputSource::no_device;
    if (!consumeAmplitude(candidate, keyboard_amplitude, design)) return false;
    state = candidate;
    return true;
}

const char* compactInputStatus(InputSource source, int detail_level) {
    if (detail_level <= 0) {
        if (source == InputSource::recorded) return "RECORDED | N=FALLBACK";
        if (source == InputSource::live_microphone) return "LIVE MIC | N=STOP";
        return "NO DEVICE | N=FALLBACK";
    }
    if (detail_level == 1) {
        if (source == InputSource::recorded) return "REC | N=FALLBACK";
        if (source == InputSource::live_microphone) return "LIVE | N=STOP";
        return "NO DEV | N=FALLBACK";
    }
    if (detail_level == 2) {
        if (source == InputSource::recorded) return "REC | N";
        if (source == InputSource::live_microphone) return "LIVE | N";
        return "NO DEV | N";
    }
    return "N";
}

const char* compactInputStatusForWidth(InputSource source, int viewport_width) {
    // openFrameworks 0.12.1's built-in bitmap font advances 8 pixels per glyph.
    const int available = std::max(0, viewport_width - 8);
    for (int detail = 0; detail <= 3; ++detail) {
        const char* status = compactInputStatus(source, detail);
        int length = 0;
        while (status[length] != '\0') ++length;
        if (length * 8 <= available) return status;
    }
    return "";
}

}  // namespace embodied
