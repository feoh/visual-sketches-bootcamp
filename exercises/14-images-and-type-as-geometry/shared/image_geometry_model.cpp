#include "image_geometry_model.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace image_geometry {
namespace {
bool finite(float value) { return std::isfinite(value); }
bool finite(Vec2 value) { return finite(value.x) && finite(value.y); }

bool checkedProduct(std::size_t a, std::size_t b, std::size_t& product) {
    if (a == 0 || b == 0 || a > std::numeric_limits<std::size_t>::max() / b) return false;
    product = a * b;
    return true;
}

bool asFloat(double value, float& output) {
    if (!std::isfinite(value) || std::fabs(value) > std::numeric_limits<float>::max()) return false;
    output = static_cast<float>(value);
    return finite(output);
}
}  // namespace

bool checkedPixelIndex(std::size_t width, std::size_t height,
                       std::size_t channels, std::size_t x, std::size_t y,
                       std::size_t channel, std::size_t& index) {
    if (width == 0 || height == 0 || channels == 0 || channels > 4 ||
        x >= width || y >= height || channel >= channels) return false;
    std::size_t pixel_count = 0;
    std::size_t byte_count = 0;
    if (!checkedProduct(width, height, pixel_count) ||
        pixel_count > maximum_source_pixels ||
        !checkedProduct(pixel_count, channels, byte_count)) return false;
    (void)byte_count;
    if (y > (std::numeric_limits<std::size_t>::max() - x) / width) return false;
    const std::size_t pixel = y * width + x;
    if (pixel > (std::numeric_limits<std::size_t>::max() - channel) / channels) return false;
    index = pixel * channels + channel;
    return true;
}

bool pixelsAreValid(const Pixels& pixels) {
    std::size_t count = 0;
    std::size_t bytes = 0;
    return pixels.width > 0 && pixels.height > 0 &&
           pixels.channels >= 1 && pixels.channels <= 4 &&
           checkedProduct(pixels.width, pixels.height, count) &&
           count <= maximum_source_pixels &&
           checkedProduct(count, pixels.channels, bytes) &&
           pixels.bytes.size() == bytes;
}

bool luminanceAt(const Pixels& pixels, std::size_t x, std::size_t y,
                 std::uint8_t& luminance) {
    if (!pixelsAreValid(pixels)) return false;
    std::size_t first = 0;
    if (!checkedPixelIndex(pixels.width, pixels.height, pixels.channels,
                           x, y, 0, first)) return false;
    if (pixels.channels == 1 || pixels.channels == 2) {
        luminance = pixels.bytes[first];
        return true;
    }
    // Integer Rec. 601 weights: deterministic and independent of color libraries.
    const unsigned red = pixels.bytes[first];
    const unsigned green = pixels.bytes[first + 1];
    const unsigned blue = pixels.bytes[first + 2];
    luminance = static_cast<std::uint8_t>((299u * red + 587u * green + 114u * blue + 500u) / 1000u);
    return true;
}

Status measure(const std::vector<Vec2>& points, Bounds& bounds, Vec2& centroid) {
    if (points.empty()) return Status::no_samples;
    if (points.size() > maximum_geometry_points) return Status::work_limit;
    Bounds next{points.front(), points.front()};
    double sum_x = 0.0;
    double sum_y = 0.0;
    for (Vec2 point : points) {
        if (!finite(point)) return Status::invalid_asset;
        next.minimum.x = std::min(next.minimum.x, point.x);
        next.minimum.y = std::min(next.minimum.y, point.y);
        next.maximum.x = std::max(next.maximum.x, point.x);
        next.maximum.y = std::max(next.maximum.y, point.y);
        sum_x += point.x;
        sum_y += point.y;
    }
    Vec2 center;
    if (!asFloat(sum_x / points.size(), center.x) ||
        !asFloat(sum_y / points.size(), center.y)) return Status::invalid_asset;
    bounds = next;
    centroid = center;
    return Status::ok;
}

Status sampleMask(const Pixels& pixels, const SampleDesign& design,
                  Geometry& output) {
    if (!pixelsAreValid(pixels)) return Status::invalid_asset;
    if (design.step == 0 || design.step > 4096) return Status::invalid_design;
    const std::size_t columns = (pixels.width - 1) / design.step + 1;
    const std::size_t rows = (pixels.height - 1) / design.step + 1;
    std::size_t visits = 0;
    if (!checkedProduct(columns, rows, visits) || visits > maximum_geometry_points)
        return Status::work_limit;

    Geometry next;
    next.points.reserve(visits);
    for (std::size_t y = 0; y < pixels.height; y += design.step) {
        for (std::size_t x = 0; x < pixels.width; x += design.step) {
            std::uint8_t value = 0;
            if (!luminanceAt(pixels, x, y, value)) return Status::invalid_asset;
            const bool selected = design.select_dark ? value <= design.threshold
                                                     : value >= design.threshold;
            if (selected) next.points.push_back({static_cast<float>(x), static_cast<float>(y)});
        }
    }
    const Status status = measure(next.points, next.bounds, next.centroid);
    if (status != Status::ok) return status;
    output = std::move(next);
    return Status::ok;
}

Status transformGeometry(const Geometry& source, const Transform& transform,
                         Geometry& output) {
    if (source.points.empty()) return Status::no_samples;
    if (source.points.size() > maximum_geometry_points) return Status::work_limit;
    if (!finite(source.centroid) || !finite(transform.translation) ||
        !finite(transform.rotation_radians) || !finite(transform.scale) ||
        transform.scale <= 0.0f || transform.scale > 10000.0f)
        return Status::invalid_transform;
    const double cosine = std::cos(transform.rotation_radians);
    const double sine = std::sin(transform.rotation_radians);
    Geometry next;
    next.points.reserve(source.points.size());
    for (Vec2 point : source.points) {
        if (!finite(point)) return Status::invalid_asset;
        const double x = (static_cast<double>(point.x) - source.centroid.x) * transform.scale;
        const double y = (static_cast<double>(point.y) - source.centroid.y) * transform.scale;
        Vec2 transformed;
        if (!asFloat(x * cosine - y * sine + source.centroid.x + transform.translation.x,
                     transformed.x) ||
            !asFloat(x * sine + y * cosine + source.centroid.y + transform.translation.y,
                     transformed.y)) return Status::invalid_transform;
        next.points.push_back(transformed);
    }
    const Status status = measure(next.points, next.bounds, next.centroid);
    if (status != Status::ok) return status;
    output = std::move(next);
    return Status::ok;
}

const char* statusMessage(Status status) {
    switch (status) {
        case Status::ok: return "ok";
        case Status::invalid_asset: return "invalid or incomplete pixel asset";
        case Status::invalid_design: return "sampling step is outside the supported range";
        case Status::work_limit: return "source or sample count exceeds the hard work limit";
        case Status::no_samples: return "threshold selected no pixels";
        case Status::invalid_transform: return "transform is non-finite or outside the supported range";
    }
    return "unknown error";
}

}  // namespace image_geometry
