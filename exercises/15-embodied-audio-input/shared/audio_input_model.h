#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace embodied {

constexpr std::size_t kMaximumBatchSize = 256;
constexpr std::size_t kMaximumFixtureSamples = 4096;
constexpr float kMaximumRadius = 1'000'000.0f;

enum class InputSource {
    recorded,
    live_microphone,
    no_device
};

struct Color {
    int r;
    int g;
    int b;
};

struct Design {
    float smoothing;
    float dead_zone;
    float minimum_radius;
    float maximum_radius;
    int minimum_rays;
    int maximum_rays;
    Color background;
    Color quiet_color;
    Color active_color;
};

struct Geometry {
    float normalized_level = 0.0f;
    float radius = 0.0f;
    int ray_count = 0;
    bool active = false;
};

struct State {
    InputSource source = InputSource::no_device;
    float raw_amplitude = 0.0f;
    float smoothed_amplitude = 0.0f;
    Geometry geometry{};
    std::uint64_t accepted_samples = 0;
    std::uint64_t rejected_samples = 0;
    std::uint64_t dropped_samples = 0;
};

bool designIsValid(const Design& design);
const char* compactInputStatus(InputSource source, int detail_level);
void reset(State& state, const Design& design, InputSource source);
bool consumeAmplitude(State& state, float amplitude, const Design& design);
std::size_t consumeBatch(State& state,
                         const std::vector<float>& amplitudes,
                         const Design& design);
bool replayRecorded(State& state,
                    const std::vector<float>& amplitudes,
                    const Design& design);
bool setNoDeviceFallback(State& state,
                         float keyboard_amplitude,
                         const Design& design);

}  // namespace embodied
