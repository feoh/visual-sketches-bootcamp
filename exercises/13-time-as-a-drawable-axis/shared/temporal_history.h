#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace temporal {

constexpr std::size_t maximum_samples = 65536;
constexpr std::size_t maximum_history_bytes = 8u * 1024u * 1024u;

struct Point {
    float x = 0.0f;
    float y = 0.0f;
};

struct Sample {
    std::uint64_t frame = 0;
    double time_seconds = 0.0;
    Point position{};
    float phase = 0.0f;
};

struct History {
    std::vector<Sample> storage;
    std::size_t next = 0;
    std::size_t count = 0;
};

bool checkedMemoryBytes(std::size_t capacity, std::size_t& bytes);
bool configure(History& history, std::size_t capacity);
void reset(History& history);
std::size_t capacity(const History& history);
std::size_t length(const History& history);
bool valid(const History& history);
bool push(History& history, const Sample& sample);
bool atAge(const History& history, std::size_t age, Sample& sample);
bool atOldestIndex(const History& history, std::size_t index, Sample& sample);
bool ageFromPosition(float normalized_position,
                     std::size_t history_length,
                     std::size_t& age);
bool select(const History& history,
            float normalized_position,
            float decay_rate,
            Sample& sample,
            float& opacity);
bool resize(History& history, std::size_t new_capacity);
bool makeMotionSample(std::uint64_t frame,
                      double time_seconds,
                      float width,
                      float height,
                      Sample& sample);
bool sameSample(const Sample& first, const Sample& second, float tolerance = 1e-5f);

}  // namespace temporal
