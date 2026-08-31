#include "structured_chance_model.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

namespace structuredchance {
namespace {
constexpr int max_regions = 64;
constexpr int max_strokes_per_region = 64;

float cross(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool convex(const Quad& quad) {
    const float a = cross(quad.top_left, quad.top_right, quad.bottom_right);
    const float b = cross(quad.top_right, quad.bottom_right, quad.bottom_left);
    const float c = cross(quad.bottom_right, quad.bottom_left, quad.top_left);
    const float d = cross(quad.bottom_left, quad.top_left, quad.top_right);
    return a > 0.0f && b > 0.0f && c > 0.0f && d > 0.0f;
}

float draw(std::mt19937& engine, float low, float high) {
    return std::uniform_real_distribution<float>(low, high)(engine);
}

int drawInt(std::mt19937& engine, int low, int high) {
    return std::uniform_int_distribution<int>(low, high)(engine);
}

bool clipAxis(float origin, float direction, float& enter, float& leave) {
    constexpr float epsilon = 0.000001f;
    if (std::fabs(direction) < epsilon) {
        return origin >= 0.0f && origin <= 1.0f;
    }
    float first = (0.0f - origin) / direction;
    float second = (1.0f - origin) / direction;
    if (first > second) std::swap(first, second);
    enter = std::max(enter, first);
    leave = std::min(leave, second);
    return enter <= leave;
}
}  // namespace

bool finitePoint(Point point) {
    return std::isfinite(point.x) && std::isfinite(point.y);
}

float signedArea(const Quad& quad) {
    const Point points[] = {quad.top_left, quad.top_right, quad.bottom_right, quad.bottom_left};
    float twice_area = 0.0f;
    for (int index = 0; index < 4; ++index) {
        const Point& current = points[index];
        const Point& next = points[(index + 1) % 4];
        twice_area += current.x * next.y - next.x * current.y;
    }
    return twice_area * 0.5f;
}

bool isValid(const Settings& settings) {
    if (settings.rows < 2 || settings.columns < 2 ||
        settings.rows * settings.columns > max_regions) return false;
    if (!std::isfinite(settings.grid_jitter) || settings.grid_jitter < 0.0f ||
        settings.grid_jitter > 0.09f) return false;
    if (!std::isfinite(settings.composition_angle) ||
        !std::isfinite(settings.region_angle_jitter) ||
        settings.region_angle_jitter < 0.0f || settings.region_angle_jitter > 1.0f) return false;
    return settings.quiet_strokes >= 1 && settings.min_strokes >= settings.quiet_strokes &&
           settings.max_strokes >= settings.min_strokes &&
           settings.max_strokes <= max_strokes_per_region;
}

Point bilinearPoint(const Quad& quad, Point local) {
    const float top_x = quad.top_left.x + (quad.top_right.x - quad.top_left.x) * local.x;
    const float top_y = quad.top_left.y + (quad.top_right.y - quad.top_left.y) * local.x;
    const float bottom_x = quad.bottom_left.x + (quad.bottom_right.x - quad.bottom_left.x) * local.x;
    const float bottom_y = quad.bottom_left.y + (quad.bottom_right.y - quad.bottom_left.y) * local.x;
    return {top_x + (bottom_x - top_x) * local.y,
            top_y + (bottom_y - top_y) * local.y};
}

bool clipLineToUnitSquare(Point center, Point direction, Point& a, Point& b) {
    if (!finitePoint(center) || !finitePoint(direction)) return false;
    float enter = -std::numeric_limits<float>::infinity();
    float leave = std::numeric_limits<float>::infinity();
    if (!clipAxis(center.x, direction.x, enter, leave) ||
        !clipAxis(center.y, direction.y, enter, leave) || !std::isfinite(enter) ||
        !std::isfinite(leave) || enter > leave) return false;
    a = {center.x + direction.x * enter, center.y + direction.y * enter};
    b = {center.x + direction.x * leave, center.y + direction.y * leave};
    return finitePoint(a) && finitePoint(b);
}

Composition generateComposition(const Settings& settings) {
    Composition result{settings, -1, {}, {}};
    if (!isValid(settings)) return result;

    std::mt19937 engine(settings.seed);
    const int node_columns = settings.columns + 1;
    result.shared_nodes.reserve(static_cast<std::size_t>((settings.rows + 1) * node_columns));
    for (int row = 0; row <= settings.rows; ++row) {
        for (int column = 0; column <= settings.columns; ++column) {
            float x = static_cast<float>(column) / static_cast<float>(settings.columns);
            float y = static_cast<float>(row) / static_cast<float>(settings.rows);
            if (row > 0 && row < settings.rows && column > 0 && column < settings.columns) {
                x += draw(engine, -settings.grid_jitter, settings.grid_jitter);
                y += draw(engine, -settings.grid_jitter, settings.grid_jitter);
            }
            result.shared_nodes.push_back({x, y});
        }
    }

    const int region_count = settings.rows * settings.columns;
    result.quiet_region_index = drawInt(engine, 0, region_count - 1);
    result.regions.reserve(static_cast<std::size_t>(region_count));
    for (int row = 0; row < settings.rows; ++row) {
        for (int column = 0; column < settings.columns; ++column) {
            const int index = row * settings.columns + column;
            const auto node = [&](int y, int x) -> Point {
                return result.shared_nodes[static_cast<std::size_t>(y * node_columns + x)];
            };
            Region region{};
            region.row = row;
            region.column = column;
            region.family = (row + column + static_cast<int>(settings.seed % 2u)) % 2;
            region.quiet = index == result.quiet_region_index;
            region.angle_radians = settings.composition_angle +
                                   draw(engine, -settings.region_angle_jitter,
                                        settings.region_angle_jitter) +
                                   (region.family == 0 ? -0.12f : 0.12f);
            region.quad = {node(row, column), node(row, column + 1),
                           node(row + 1, column + 1), node(row + 1, column)};
            if (!convex(region.quad)) return Composition{settings, -1, {}, {}};

            const int count = region.quiet
                                  ? settings.quiet_strokes
                                  : drawInt(engine, settings.min_strokes, settings.max_strokes);
            const Point direction{std::cos(region.angle_radians),
                                  std::sin(region.angle_radians)};
            const Point normal{-direction.y, direction.x};
            region.strokes.reserve(static_cast<std::size_t>(count));
            for (int stroke_index = 0; stroke_index < count; ++stroke_index) {
                const float progress = (static_cast<float>(stroke_index) + 0.5f) /
                                       static_cast<float>(count);
                const float offset = (progress - 0.5f) * 1.35f + draw(engine, -0.025f, 0.025f);
                const Point center{0.5f + normal.x * offset, 0.5f + normal.y * offset};
                Point local_a{};
                Point local_b{};
                if (!clipLineToUnitSquare(center, direction, local_a, local_b)) continue;
                region.strokes.push_back({local_a, local_b,
                                          bilinearPoint(region.quad, local_a),
                                          bilinearPoint(region.quad, local_b),
                                          draw(engine, 0.009f, 0.022f),
                                          region.family});
            }
            result.regions.push_back(region);
        }
    }
    return result;
}

}  // namespace structuredchance
