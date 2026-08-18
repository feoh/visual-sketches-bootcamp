#pragma once

namespace responsiveposter {
struct Vec2 { float x; float y; };
struct Rect { float x; float y; float width; float height; };
struct Color { int r; int g; int b; };
struct Viewport { int width; int height; };
struct Design {
    float focus;
    float vertical_bias;
    Color ink;
    Color accent;
    Color background;
};
struct Layout {
    bool valid;
    Rect panel;
    Vec2 focus_center;
    float focus_radius;
    float headline_size;
};

float clamp01(float value);
float normalizeClamped(float value, float input_min, float input_max);
float lerpClamped(float start, float end, float amount);
float mapClamped(float value, float input_min, float input_max,
                 float output_min, float output_max);
float smoothstep(float amount);
bool designIsValid(const Design& design);
Layout makeLayout(const Design& design, Viewport viewport);
float availableFocusDistance(const Layout& layout);
bool layoutIsFiniteAndInBounds(const Layout& layout, Viewport viewport);
}  // namespace responsiveposter
