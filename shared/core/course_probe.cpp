#include "course_probe.h"

#include <algorithm>
#include <cmath>

namespace course {
namespace {

std::uint32_t nextBits(std::uint32_t& state) {
    // Course-owned mapping: unlike std::uniform_real_distribution, this replay
    // is defined by these integer operations on every supported standard library.
    if (state == 0) {
        state = 0x6d2b79f5U;
    }
    state ^= state << 13U;
    state ^= state >> 17U;
    state ^= state << 5U;
    return state;
}

float unitFloat(std::uint32_t& state) {
    return static_cast<float>(nextBits(state) >> 8U) / 16777216.0f;
}

float wrap(float value, float extent) {
    if (!(extent > 0.0f) || !std::isfinite(value)) {
        return 0.0f;
    }
    value = std::fmod(value, extent);
    return value < 0.0f ? value + extent : value;
}

}  // namespace

Model makeModel(std::uint32_t seed, const Viewport& viewport) {
    std::uint32_t state = seed;
    Model model;
    model.seed = seed;
    model.position = {unitFloat(state) * std::max(0.0f, viewport.width),
                      unitFloat(state) * std::max(0.0f, viewport.height)};
    model.velocity = {(unitFloat(state) * 2.0f - 1.0f) * 80.0f,
                      (unitFloat(state) * 2.0f - 1.0f) * 80.0f};
    return model;
}

void update(Model& model, const StepInput& input, float dt, const Viewport& viewport) {
    // Invalid, non-positive time does not move the model. Positive finite time
    // is capped so a paused debugger or stalled frame cannot create a jump.
    const float safeDt = std::isfinite(dt) && dt > 0.0f ? std::min(dt, 0.25f) : 0.0f;
    model.velocity.x += input.acceleration.x * safeDt;
    model.velocity.y += input.acceleration.y * safeDt;
    model.position.x = wrap(model.position.x + model.velocity.x * safeDt, viewport.width);
    model.position.y = wrap(model.position.y + model.velocity.y * safeDt, viewport.height);
    ++model.stepCount;
}

}  // namespace course
