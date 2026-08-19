#pragma once

#if __cplusplus < 201703L
#error "visual-sketches-bootcamp requires C++17 or newer"
#endif

#include <cstdint>

namespace course {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Viewport {
    float width = 0.0f;
    float height = 0.0f;
};

struct StepInput {
    Vec2 acceleration;
};

struct Model {
    Vec2 position;
    Vec2 velocity;
    std::uint32_t seed = 0;
    std::uint64_t stepCount = 0;
};

Model makeModel(std::uint32_t seed, const Viewport& viewport);
// Non-finite, negative, and zero dt values advance the step counter but leave
// position and velocity unchanged. Positive finite dt is capped at 0.25 seconds.
void update(Model& model, const StepInput& input, float dt, const Viewport& viewport);

}  // namespace course
