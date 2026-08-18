#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace image_geometry {

constexpr std::size_t maximum_source_pixels = 4'194'304;
constexpr std::size_t maximum_geometry_points = 100'000;

struct Vec2 { float x = 0.0f; float y = 0.0f; };
struct Bounds { Vec2 minimum; Vec2 maximum; };

struct Pixels {
    std::size_t width = 0;
    std::size_t height = 0;
    std::size_t channels = 0;
    std::vector<std::uint8_t> bytes;
};

struct SampleDesign {
    std::size_t step = 1;
    std::uint8_t threshold = 127;
    bool select_dark = true;
};

struct Geometry {
    std::vector<Vec2> points;
    Bounds bounds{};
    Vec2 centroid{};
};

struct Transform {
    Vec2 translation{};
    float rotation_radians = 0.0f;
    float scale = 1.0f;
};

enum class Status {
    ok,
    invalid_asset,
    asset_too_large,
    invalid_design,
    work_limit,
    no_samples,
    invalid_transform
};

bool checkedPixelIndex(std::size_t width, std::size_t height,
                       std::size_t channels, std::size_t x, std::size_t y,
                       std::size_t channel, std::size_t& index);
bool pixelsAreValid(const Pixels& pixels);
bool luminanceAt(const Pixels& pixels, std::size_t x, std::size_t y,
                 std::uint8_t& luminance);
Status sampleMask(const Pixels& pixels, const SampleDesign& design,
                  Geometry& output);
Status measure(const std::vector<Vec2>& points, Bounds& bounds, Vec2& centroid);
Status transformGeometry(const Geometry& source, const Transform& transform,
                         Geometry& output);
const char* statusMessage(Status status);

}  // namespace image_geometry
