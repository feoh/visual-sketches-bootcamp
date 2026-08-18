#pragma once

#include <cstddef>
#include <vector>

namespace colortrail {

struct Vec2 {
    float x;
    float y;
};

// Straight (not premultiplied) RGBA channels in the closed interval [0, 1].
struct Color {
    float r;
    float g;
    float b;
    float a;
};

struct Palette {
    Color first;
    Color second;
};

struct Design {
    std::size_t maximum_history;
    float alpha_retention;
    float trail_opacity;
    float minimum_radius;
    float maximum_radius;
    Color background;
    Palette palettes[2];
};

struct TrailSample {
    Vec2 position;
    float palette_amount;
    std::size_t palette_index;
};

struct TrailMark {
    Vec2 position;
    Color color;
    float radius;
    std::size_t age_steps;
    std::size_t palette_index;
};

bool colorIsValid(Color color);
bool paletteIsValid(const Palette& palette);
bool designIsValid(const Design& design);
bool sampleIsValid(const TrailSample& sample);

bool interpolateColor(Color first, Color second, float amount, Color& output);
bool paletteColor(const Palette& palette, float amount, Color& output);
bool sourceOver(Color foreground, Color background, Color& output);
bool decayAlpha(float initial_alpha,
                float retention_per_step,
                std::size_t age_steps,
                float& output);

bool appendSample(std::vector<TrailSample>& history,
                  TrailSample sample,
                  std::size_t maximum_history);
bool buildTrailMarks(const std::vector<TrailSample>& history,
                     const Design& design,
                     std::vector<TrailMark>& output);
bool marksAreFinite(const std::vector<TrailMark>& marks,
                    const Design& design);

}  // namespace colortrail
