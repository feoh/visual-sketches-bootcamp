#include "traveler_model.h"

#include <algorithm>
#include <cmath>

namespace traveler {
namespace {
float positive(float value) { return std::isfinite(value) && value > 0.0f ? value : 1.0f; }
float wrap(float value, float radius, float extent) {
    const float period = positive(extent) + 2.0f * radius;
    float result = std::fmod(value + radius, period);
    if (result < 0.0f) result += period;
    return result - radius;
}
bool validColor(Color color) {
    return color.red >= 0 && color.red <= 255 && color.green >= 0 && color.green <= 255 &&
           color.blue >= 0 && color.blue <= 255;
}
Vec2 desiredVelocity(const State& state, const Design& design, const Input& input) {
    Vec2 direction = input.keyboard_direction;
    if (input.pointer_active && std::isfinite(input.pointer.x) && std::isfinite(input.pointer.y)) {
        direction = {input.pointer.x - state.position.x, input.pointer.y - state.position.y};
    }
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (!std::isfinite(length) || length <= 0.0001f) return {0.0f, 0.0f};
    const float motion_scale = state.reduced_motion ? 0.25f : 1.0f;
    const float speed = design.speed_pixels_per_second * motion_scale;
    return {direction.x / length * speed, direction.y / length * speed};
}
void step(State& state, const Design& design, const Input& input, float dt, Viewport viewport) {
    state.velocity = desiredVelocity(state, design, input);
    state.position.x = wrap(state.position.x + stepDistance(state.velocity.x, dt),
                            design.radius_pixels, viewport.width);
    state.position.y = wrap(state.position.y + stepDistance(state.velocity.y, dt),
                            design.radius_pixels, viewport.height);
}
}

State makeState(const Design& design, Viewport viewport) {
    State state{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, false, false};
    reset(state, design, viewport);
    return state;
}
void reset(State& state, const Design& design, Viewport viewport) {
    state.position = {std::clamp(design.normalized_start.x, 0.0f, 1.0f) * positive(viewport.width),
                      std::clamp(design.normalized_start.y, 0.0f, 1.0f) * positive(viewport.height)};
    state.velocity = {0.0f, 0.0f};
    state.accumulator_seconds = 0.0f;
    state.paused = false;
}
void togglePause(State& state) { state.paused = !state.paused; state.accumulator_seconds = 0.0f; }
void toggleReducedMotion(State& state) { state.reduced_motion = !state.reduced_motion; }
void advanceVariable(State& state, const Design& design, const Input& input,
                     float frame_seconds, Viewport viewport) {
    if (state.paused || !std::isfinite(frame_seconds) || frame_seconds <= 0.0f) return;
    step(state, design, input, std::min(frame_seconds, maximumFrameSeconds()), viewport);
}
void advanceFrame(State& state, const Design& design, const Input& input,
                  float frame_seconds, Viewport viewport) {
    if (state.paused) {
        state.accumulator_seconds = 0.0f;
        return;
    }
    if (!std::isfinite(frame_seconds) || frame_seconds <= 0.0f) return;
    state.accumulator_seconds += std::min(frame_seconds, maximumFrameSeconds());
    const float fixed = fixedStepSeconds();
    while (state.accumulator_seconds + 0.0000001f >= fixed) {
        step(state, design, input, fixed, viewport);
        state.accumulator_seconds -= fixed;
    }
    if (state.accumulator_seconds < 0.0f) state.accumulator_seconds = 0.0f;
}
bool designIsValid(const Design& design) {
    const bool colors = validColor(design.mark_color) && validColor(design.trail_color) &&
                        validColor(design.background_color);
    const bool distinct = (design.mark_color.red != design.background_color.red ||
                           design.mark_color.green != design.background_color.green ||
                           design.mark_color.blue != design.background_color.blue);
    return std::isfinite(design.normalized_start.x) && design.normalized_start.x >= 0.0f &&
           design.normalized_start.x <= 1.0f && std::isfinite(design.normalized_start.y) &&
           design.normalized_start.y >= 0.0f && design.normalized_start.y <= 1.0f &&
           std::isfinite(design.speed_pixels_per_second) && design.speed_pixels_per_second >= 30.0f &&
           design.speed_pixels_per_second <= 500.0f && std::isfinite(design.radius_pixels) &&
           design.radius_pixels >= 4.0f && design.radius_pixels <= 80.0f && colors && distinct;
}
bool stateIsFiniteAndWrapped(const State& state, const Design& design, Viewport viewport) {
    const float width = positive(viewport.width), height = positive(viewport.height);
    return std::isfinite(state.position.x) && std::isfinite(state.position.y) &&
           std::isfinite(state.velocity.x) && std::isfinite(state.velocity.y) &&
           std::isfinite(state.accumulator_seconds) && state.accumulator_seconds >= 0.0f &&
           state.accumulator_seconds < fixedStepSeconds() + 0.00001f &&
           state.position.x >= -design.radius_pixels && state.position.x < width + design.radius_pixels &&
           state.position.y >= -design.radius_pixels && state.position.y < height + design.radius_pixels;
}
}  // namespace traveler
