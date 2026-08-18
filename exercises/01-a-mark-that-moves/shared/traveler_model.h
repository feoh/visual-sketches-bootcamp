#pragma once

#if (defined(_MSVC_LANG) && _MSVC_LANG < 201703L) || \
    (!defined(_MSVC_LANG) && __cplusplus < 201703L)
#error "The section 01 traveler model requires C++17 or newer"
#endif

namespace traveler {

struct Vec2 { float x; float y; };
struct Viewport { float width; float height; };
struct Color { int red; int green; int blue; };
struct Design {
    Vec2 normalized_start;
    float speed_pixels_per_second;
    float radius_pixels;
    Color mark_color;
    Color trail_color;
    Color background_color;
};
struct Input {
    bool pointer_active;
    Vec2 pointer;
    Vec2 keyboard_direction;
};
struct State {
    Vec2 position;
    Vec2 velocity;
    float accumulator_seconds;
    bool paused;
    bool reduced_motion;
};

constexpr float fixedStepSeconds() { return 1.0f / 120.0f; }
constexpr float maximumFrameSeconds() { return 0.1f; }
State makeState(const Design& design, Viewport viewport);
void reset(State& state, const Design& design, Viewport viewport);
void togglePause(State& state);
void toggleReducedMotion(State& state);
void advanceVariable(State& state, const Design& design, const Input& input,
                     float frame_seconds, Viewport viewport);
void advanceFrame(State& state, const Design& design, const Input& input,
                  float frame_seconds, Viewport viewport);
bool designIsValid(const Design& design);
bool stateIsFiniteAndWrapped(const State& state, const Design& design, Viewport viewport);

}  // namespace traveler
