#include "flow_field_model.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace flow {
namespace {

constexpr std::size_t maximum_grid_cells = 65536;
constexpr std::size_t maximum_history_points = 65536;
constexpr double tau = 6.28318530717958647692;

bool finite(float value) {
    return std::isfinite(value);
}

bool finite(Vec2 value) {
    return finite(value.x) && finite(value.y);
}

bool valid(Color color) {
    return color.r >= 0 && color.r <= 255 &&
           color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}

bool checkedFloat(double value, float& output) {
    if (!std::isfinite(value) ||
        std::fabs(value) > std::numeric_limits<float>::max()) {
        return false;
    }
    output = static_cast<float>(value);
    return finite(output);
}

bool checkedIncrease(float current, float increment, float& output) {
    if (!finite(current) || !finite(increment) || increment < 0.0f) {
        return false;
    }
    float next = 0.0f;
    if (!checkedFloat(static_cast<double>(current) + increment, next) ||
        (increment > 0.0f && !(next > current))) {
        return false;
    }
    output = next;
    return true;
}

bool checkedProduct(std::size_t first,
                    std::size_t second,
                    std::size_t maximum,
                    std::size_t& product) {
    if (first == 0 || second == 0 || first > maximum / second) {
        return false;
    }
    product = first * second;
    return product <= maximum;
}

std::uint32_t mix(std::uint32_t hash, std::uint32_t value) {
    // Boost-style hash_combine followed by Chris Wellons's lowbias32
    // finalizer. The lesson source records document provenance and licenses.
    hash ^= value + 0x9e3779b9u + (hash << 6) + (hash >> 2);
    hash ^= hash >> 16;
    hash *= 0x7feb352du;
    hash ^= hash >> 15;
    hash *= 0x846ca68bu;
    hash ^= hash >> 16;
    return hash;
}

bool latticeCoordinate(double value, std::int32_t& coordinate, double& fraction) {
    if (!std::isfinite(value)) {
        return false;
    }
    const double base = std::floor(value);
    if (base < std::numeric_limits<std::int32_t>::min() ||
        base >= std::numeric_limits<std::int32_t>::max()) {
        return false;
    }
    coordinate = static_cast<std::int32_t>(base);
    fraction = value - base;
    return fraction >= 0.0 && fraction <= 1.0;
}

float latticeValue(std::int32_t x,
                   std::int32_t y,
                   std::int32_t z,
                   std::uint32_t seed) {
    return hashToUnit(latticeHash(x, y, z, seed));
}

double lerp(double first, double second, double amount) {
    return first + (second - first) * amount;
}

bool wrapAxis(float low, float high, double value, float& wrapped) {
    if (!std::isfinite(value) || !finite(low) || !finite(high) || high < low) {
        return false;
    }
    const double span = static_cast<double>(high) - low;
    if (span == 0.0) {
        wrapped = low;
        return true;
    }
    double result = value;
    if (result > high) {
        result = static_cast<double>(low) + std::fmod(result - low, span);
    } else if (result < low) {
        const double remainder = std::fmod(static_cast<double>(low) - result, span);
        result = remainder == 0.0 ? high : static_cast<double>(high) - remainder;
    }
    return checkedFloat(result, wrapped);
}

}  // namespace

bool gridDimensionsValid(std::size_t columns, std::size_t rows) {
    std::size_t cells = 0;
    return columns >= 2 && columns <= 256 &&
           rows >= 2 && rows <= 256 &&
           checkedProduct(columns, rows, maximum_grid_cells, cells);
}

bool designIsValid(const Design& design) {
    std::size_t history_points = 0;
    return finite(design.fixed_step) &&
           design.fixed_step >= 0.00001f &&
           finite(design.maximum_frame_time) &&
           design.maximum_frame_time >= design.fixed_step &&
           design.maximum_frame_time <= 1.0f &&
           design.maximum_catch_up_steps > 0 &&
           design.maximum_catch_up_steps <= 10000 &&
           gridDimensionsValid(design.grid_columns, design.grid_rows) &&
           design.particle_count > 0 && design.particle_count <= 1024 &&
           design.maximum_history > 0 && design.maximum_history <= 1024 &&
           checkedProduct(design.particle_count,
                          design.maximum_history,
                          maximum_history_points,
                          history_points) &&
           finite(design.particle_radius) && design.particle_radius > 0.0f &&
           design.particle_radius <= 10000.0f &&
           finite(design.advection_speed) && design.advection_speed > 0.0f &&
           design.advection_speed <= 1000000.0f &&
           finite(design.spatial_scale) && design.spatial_scale > 0.0f &&
           design.spatial_scale <= 16.0f &&
           finite(design.temporal_scale) && design.temporal_scale > 0.0f &&
           design.temporal_scale <= 16.0f &&
           finite(design.field_time_rate) && design.field_time_rate >= 0.0f &&
           design.field_time_rate <= 1000.0f &&
           valid(design.background) && valid(design.trail_color) &&
           valid(design.head_color);
}

bool boundsAreUsable(Bounds bounds, float radius) {
    if (!finite(bounds.width) || !finite(bounds.height) ||
        !finite(radius) || radius <= 0.0f) {
        return false;
    }
    const double diameter = 2.0 * static_cast<double>(radius);
    return static_cast<double>(bounds.width) >= diameter &&
           static_cast<double>(bounds.height) >= diameter;
}

bool gridIndex(std::size_t columns,
               std::size_t rows,
               std::size_t column,
               std::size_t row,
               std::size_t& index) {
    if (!gridDimensionsValid(columns, rows) || column >= columns || row >= rows ||
        row > (std::numeric_limits<std::size_t>::max() - column) / columns) {
        return false;
    }
    index = row * columns + column;
    return true;
}

std::uint32_t latticeHash(std::int32_t x,
                          std::int32_t y,
                          std::int32_t z,
                          std::uint32_t seed) {
    std::uint32_t hash = seed ^ 0xa511e9b3u;
    hash = mix(hash, static_cast<std::uint32_t>(x));
    hash = mix(hash, static_cast<std::uint32_t>(y));
    return mix(hash, static_cast<std::uint32_t>(z));
}

float hashToUnit(std::uint32_t hash) {
    return static_cast<float>(hash & 0x00ffffffu) / 16777215.0f;
}

float smoothInterpolation(float amount) {
    if (!finite(amount)) {
        return 0.0f;
    }
    const float clamped = std::clamp(amount, 0.0f, 1.0f);
    return clamped * clamped * (3.0f - 2.0f * clamped);
}

bool valueNoise(float x,
                float y,
                float z,
                std::uint32_t seed,
                float& value) {
    std::int32_t x0 = 0;
    std::int32_t y0 = 0;
    std::int32_t z0 = 0;
    double fx = 0.0;
    double fy = 0.0;
    double fz = 0.0;
    if (!latticeCoordinate(x, x0, fx) ||
        !latticeCoordinate(y, y0, fy) ||
        !latticeCoordinate(z, z0, fz)) {
        return false;
    }
    const std::int32_t x1 = x0 + 1;
    const std::int32_t y1 = y0 + 1;
    const std::int32_t z1 = z0 + 1;
    const double sx = smoothInterpolation(static_cast<float>(fx));
    const double sy = smoothInterpolation(static_cast<float>(fy));
    const double sz = smoothInterpolation(static_cast<float>(fz));
    const double c000 = latticeValue(x0, y0, z0, seed);
    const double c100 = latticeValue(x1, y0, z0, seed);
    const double c010 = latticeValue(x0, y1, z0, seed);
    const double c110 = latticeValue(x1, y1, z0, seed);
    const double c001 = latticeValue(x0, y0, z1, seed);
    const double c101 = latticeValue(x1, y0, z1, seed);
    const double c011 = latticeValue(x0, y1, z1, seed);
    const double c111 = latticeValue(x1, y1, z1, seed);
    const double lower = lerp(lerp(c000, c100, sx),
                              lerp(c010, c110, sx), sy);
    const double upper = lerp(lerp(c001, c101, sx),
                              lerp(c011, c111, sx), sy);
    return checkedFloat(lerp(lower, upper, sz), value) &&
           value >= 0.0f && value <= 1.0f;
}

bool generateField(const Design& design,
                   Vec2 offset,
                   float field_time,
                   std::uint32_t seed,
                   ScalarGrid& field) {
    if (!designIsValid(design) || !finite(offset) ||
        std::fabs(offset.x) > 10000.0f || std::fabs(offset.y) > 10000.0f ||
        !finite(field_time) || std::fabs(field_time) > 1000000.0f) {
        return false;
    }
    std::size_t count = 0;
    if (!checkedProduct(design.grid_columns,
                        design.grid_rows,
                        maximum_grid_cells,
                        count)) {
        return false;
    }
    ScalarGrid next;
    next.columns = design.grid_columns;
    next.rows = design.grid_rows;
    next.values.reserve(count);
    const double z = static_cast<double>(field_time) * design.temporal_scale;
    float z_value = 0.0f;
    if (!checkedFloat(z, z_value)) {
        return false;
    }
    for (std::size_t row = 0; row < next.rows; ++row) {
        for (std::size_t column = 0; column < next.columns; ++column) {
            float sample_x = 0.0f;
            float sample_y = 0.0f;
            if (!checkedFloat(static_cast<double>(column) * design.spatial_scale +
                                  offset.x,
                              sample_x) ||
                !checkedFloat(static_cast<double>(row) * design.spatial_scale +
                                  offset.y,
                              sample_y)) {
                return false;
            }
            float sample = 0.0f;
            if (!valueNoise(sample_x, sample_y, z_value, seed, sample)) {
                return false;
            }
            next.values.push_back(sample);
        }
    }
    field = std::move(next);
    return true;
}

bool sampleGrid(const ScalarGrid& field,
                float normalized_x,
                float normalized_y,
                float& value) {
    if (!gridDimensionsValid(field.columns, field.rows) ||
        field.values.size() != field.columns * field.rows ||
        !finite(normalized_x) || !finite(normalized_y) ||
        normalized_x < 0.0f || normalized_x > 1.0f ||
        normalized_y < 0.0f || normalized_y > 1.0f) {
        return false;
    }
    const double grid_x = static_cast<double>(normalized_x) * (field.columns - 1);
    const double grid_y = static_cast<double>(normalized_y) * (field.rows - 1);
    const std::size_t x0 = static_cast<std::size_t>(std::floor(grid_x));
    const std::size_t y0 = static_cast<std::size_t>(std::floor(grid_y));
    const std::size_t x1 = std::min(x0 + 1, field.columns - 1);
    const std::size_t y1 = std::min(y0 + 1, field.rows - 1);
    const double tx = grid_x - x0;
    const double ty = grid_y - y0;
    std::size_t i00 = 0;
    std::size_t i10 = 0;
    std::size_t i01 = 0;
    std::size_t i11 = 0;
    if (!gridIndex(field.columns, field.rows, x0, y0, i00) ||
        !gridIndex(field.columns, field.rows, x1, y0, i10) ||
        !gridIndex(field.columns, field.rows, x0, y1, i01) ||
        !gridIndex(field.columns, field.rows, x1, y1, i11)) {
        return false;
    }
    const float samples[] = {field.values[i00], field.values[i10],
                             field.values[i01], field.values[i11]};
    for (float sample : samples) {
        if (!finite(sample) || sample < 0.0f || sample > 1.0f) {
            return false;
        }
    }
    return checkedFloat(lerp(lerp(samples[0], samples[1], tx),
                              lerp(samples[2], samples[3], tx), ty),
                        value);
}

Vec2 directionFromScalar(float value) {
    if (!finite(value)) {
        return {0.0f, 0.0f};
    }
    const double angle = static_cast<double>(std::clamp(value, 0.0f, 1.0f)) * tau;
    return {static_cast<float>(std::cos(angle)),
            static_cast<float>(std::sin(angle))};
}

bool sampleDirection(const ScalarGrid& field,
                     Vec2 position,
                     Bounds bounds,
                     Vec2& direction) {
    if (!finite(position) || !finite(bounds.width) || !finite(bounds.height) ||
        bounds.width <= 0.0f || bounds.height <= 0.0f ||
        position.x < 0.0f || position.x > bounds.width ||
        position.y < 0.0f || position.y > bounds.height) {
        return false;
    }
    float value = 0.0f;
    if (!sampleGrid(field,
                    position.x / bounds.width,
                    position.y / bounds.height,
                    value)) {
        return false;
    }
    direction = directionFromScalar(value);
    return finite(direction);
}

bool advanceParticle(Particle& particle,
                     const ScalarGrid& field,
                     float dt,
                     const Design& design,
                     Bounds bounds,
                     bool reduced_motion) {
    if (!designIsValid(design) ||
        !boundsAreUsable(bounds, design.particle_radius) ||
        !finite(dt) || dt <= 0.0f || !finite(particle.position) ||
        particle.position.x < design.particle_radius ||
        particle.position.x > bounds.width - design.particle_radius ||
        particle.position.y < design.particle_radius ||
        particle.position.y > bounds.height - design.particle_radius ||
        particle.history.empty() || particle.history.size() > design.maximum_history) {
        return false;
    }
    for (Vec2 point : particle.history) {
        if (!finite(point) || point.x < design.particle_radius ||
            point.x > bounds.width - design.particle_radius ||
            point.y < design.particle_radius ||
            point.y > bounds.height - design.particle_radius) {
            return false;
        }
    }
    Vec2 direction{0.0f, 0.0f};
    if (!sampleDirection(field, particle.position, bounds, direction)) {
        return false;
    }
    float next_x = 0.0f;
    float next_y = 0.0f;
    const double raw_x = static_cast<double>(particle.position.x) +
                         static_cast<double>(direction.x) * design.advection_speed * dt;
    const double raw_y = static_cast<double>(particle.position.y) +
                         static_cast<double>(direction.y) * design.advection_speed * dt;
    const bool wrapped = raw_x < design.particle_radius ||
                         raw_x > bounds.width - design.particle_radius ||
                         raw_y < design.particle_radius ||
                         raw_y > bounds.height - design.particle_radius;
    if (!wrapAxis(design.particle_radius,
                  bounds.width - design.particle_radius,
                  raw_x,
                  next_x) ||
        !wrapAxis(design.particle_radius,
                  bounds.height - design.particle_radius,
                  raw_y,
                  next_y)) {
        return false;
    }
    Particle next = particle;
    next.position = {next_x, next_y};
    if (reduced_motion || wrapped) {
        // A toroidal jump is not a drawable local trajectory segment.
        next.history.assign(1, next.position);
    } else {
        next.history.push_back(next.position);
        if (next.history.size() > design.maximum_history) {
            const std::size_t excess = next.history.size() - design.maximum_history;
            next.history.erase(next.history.begin(),
                               next.history.begin() +
                                   static_cast<std::ptrdiff_t>(excess));
        }
    }
    particle = std::move(next);
    return true;
}

FrameResult advanceFrame(System& system,
                         float frame_dt,
                         const Design& design,
                         Bounds bounds) {
    const FrameResult rejected{0, 0.0f};
    if (!finiteState(system, design, bounds) ||
        !finite(frame_dt) || frame_dt < 0.0f) {
        return rejected;
    }
    if (system.paused) {
        system.accumulator = 0.0f;
        return rejected;
    }
    System next = system;
    const float accepted = std::min(frame_dt, design.maximum_frame_time);
    double accumulated = static_cast<double>(next.accumulator) + accepted;
    float dropped = frame_dt - accepted;
    const double tolerance = static_cast<double>(design.fixed_step) * 1e-6;
    const double raw_steps = std::floor((accumulated + tolerance) / design.fixed_step);
    if (!std::isfinite(raw_steps) || raw_steps < 0.0 ||
        raw_steps > static_cast<double>(std::numeric_limits<std::size_t>::max())) {
        return rejected;
    }
    const std::size_t available = static_cast<std::size_t>(raw_steps);
    const std::size_t simulated = std::min(available, design.maximum_catch_up_steps);
    if (next.simulated_steps > std::numeric_limits<std::size_t>::max() - simulated) {
        return rejected;
    }
    for (std::size_t step = 0; step < simulated; ++step) {
        if (!next.time_frozen) {
            float time = 0.0f;
            if (!checkedFloat(static_cast<double>(next.field_time) +
                                  static_cast<double>(design.fixed_step) *
                                      design.field_time_rate,
                              time)) {
                return rejected;
            }
            next.field_time = time;
        }
        if (!generateField(design,
                           next.field_offset,
                           next.field_time,
                           next.seed,
                           next.field)) {
            return rejected;
        }
        for (Particle& particle : next.particles) {
            if (!advanceParticle(particle,
                                 next.field,
                                 design.fixed_step,
                                 design,
                                 bounds,
                                 next.reduced_motion)) {
                return rejected;
            }
        }
        ++next.simulated_steps;
    }
    accumulated -= static_cast<double>(simulated) * design.fixed_step;
    if (available > simulated) {
        const std::size_t dropped_steps = available - simulated;
        accumulated -= static_cast<double>(dropped_steps) * design.fixed_step;
        float dropped_increment = 0.0f;
        float increased_drop = 0.0f;
        if (!checkedFloat(static_cast<double>(dropped_steps) * design.fixed_step,
                          dropped_increment) ||
            !checkedIncrease(dropped, dropped_increment, increased_drop)) {
            return rejected;
        }
        dropped = increased_drop;
    }
    float remainder = 0.0f;
    float total_dropped = 0.0f;
    if (!checkedFloat(std::max(0.0, accumulated), remainder) ||
        !checkedIncrease(next.dropped_time, dropped, total_dropped)) {
        return rejected;
    }
    next.accumulator = remainder;
    next.dropped_time = total_dropped;
    system = std::move(next);
    return {simulated, dropped};
}

bool setFieldOffset(System& system, Vec2 offset) {
    if (!finite(offset) || std::fabs(offset.x) > 10000.0f ||
        std::fabs(offset.y) > 10000.0f) {
        return false;
    }
    system.field_offset = offset;
    system.accumulator = 0.0f;
    return true;
}

void setPaused(System& system, bool paused) {
    system.paused = paused;
    system.accumulator = 0.0f;
}

void setReducedMotion(System& system, bool reduced_motion) {
    system.reduced_motion = reduced_motion;
    system.accumulator = 0.0f;
    if (reduced_motion) {
        for (Particle& particle : system.particles) {
            particle.history.assign(1, particle.position);
        }
    }
}

void setTimeFrozen(System& system, bool frozen) {
    system.time_frozen = frozen;
    system.accumulator = 0.0f;
}

void reset(System& system,
           Bounds bounds,
           const Design& design,
           std::uint32_t seed,
           Vec2 offset) {
    System next;
    if (!designIsValid(design) ||
        !boundsAreUsable(bounds, design.particle_radius) ||
        !finite(offset) || std::fabs(offset.x) > 10000.0f ||
        std::fabs(offset.y) > 10000.0f) {
        system = next;
        return;
    }
    next.seed = seed;
    next.field_offset = offset;
    if (!generateField(design, offset, 0.0f, next.seed, next.field)) {
        system = System{};
        return;
    }
    next.particles.reserve(design.particle_count);
    const double span_x = static_cast<double>(bounds.width) -
                          2.0 * design.particle_radius;
    const double span_y = static_cast<double>(bounds.height) -
                          2.0 * design.particle_radius;
    for (std::size_t index = 0; index < design.particle_count; ++index) {
        const std::uint32_t x_hash = latticeHash(static_cast<std::int32_t>(index),
                                                  17, 31, next.seed);
        const std::uint32_t y_hash = latticeHash(static_cast<std::int32_t>(index),
                                                  43, 59, next.seed);
        float x = 0.0f;
        float y = 0.0f;
        if (!checkedFloat(design.particle_radius + hashToUnit(x_hash) * span_x, x) ||
            !checkedFloat(design.particle_radius + hashToUnit(y_hash) * span_y, y)) {
            system = System{};
            return;
        }
        next.particles.push_back({{x, y}, {{x, y}}});
    }
    system = std::move(next);
}

bool finiteState(const System& system,
                 const Design& design,
                 Bounds bounds) {
    if (!designIsValid(design) ||
        !boundsAreUsable(bounds, design.particle_radius) ||
        system.field.columns != design.grid_columns ||
        system.field.rows != design.grid_rows ||
        system.field.values.size() != design.grid_columns * design.grid_rows ||
        system.particles.size() != design.particle_count ||
        !finite(system.field_offset) ||
        std::fabs(system.field_offset.x) > 10000.0f ||
        std::fabs(system.field_offset.y) > 10000.0f ||
        !finite(system.field_time) || std::fabs(system.field_time) > 1000000.0f ||
        !finite(system.accumulator) || system.accumulator < 0.0f ||
        system.accumulator >= design.fixed_step ||
        !finite(system.dropped_time) || system.dropped_time < 0.0f) {
        return false;
    }
    for (float value : system.field.values) {
        if (!finite(value) || value < 0.0f || value > 1.0f) {
            return false;
        }
    }
    for (const Particle& particle : system.particles) {
        if (!finite(particle.position) ||
            particle.position.x < design.particle_radius ||
            particle.position.x > bounds.width - design.particle_radius ||
            particle.position.y < design.particle_radius ||
            particle.position.y > bounds.height - design.particle_radius ||
            particle.history.empty() ||
            particle.history.size() > design.maximum_history) {
            return false;
        }
        for (Vec2 point : particle.history) {
            if (!finite(point) ||
                point.x < design.particle_radius ||
                point.x > bounds.width - design.particle_radius ||
                point.y < design.particle_radius ||
                point.y > bounds.height - design.particle_radius) {
                return false;
            }
        }
    }
    return true;
}

}  // namespace flow
