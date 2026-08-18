#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace flow {

struct Vec2 {
    float x;
    float y;
};

struct Color {
    int r;
    int g;
    int b;
};

struct Bounds {
    float width;
    float height;
};

struct Design {
    float fixed_step;
    float maximum_frame_time;
    std::size_t maximum_catch_up_steps;
    std::size_t grid_columns;
    std::size_t grid_rows;
    std::size_t particle_count;
    std::size_t maximum_history;
    float particle_radius;
    float advection_speed;
    float spatial_scale;
    float temporal_scale;
    float field_time_rate;
    Color background;
    Color trail_color;
    Color head_color;
};

struct ScalarGrid {
    std::size_t columns = 0;
    std::size_t rows = 0;
    std::vector<float> values;
};

struct Particle {
    Vec2 position;
    std::vector<Vec2> history;
};

struct System {
    ScalarGrid field;
    std::vector<Particle> particles;
    Vec2 field_offset{0.0f, 0.0f};
    float field_time = 0.0f;
    float accumulator = 0.0f;
    float dropped_time = 0.0f;
    std::uint32_t seed = 1u;
    std::size_t simulated_steps = 0;
    bool paused = false;
    bool reduced_motion = false;
    bool time_frozen = false;
};

struct FrameResult {
    std::size_t simulated_steps;
    float dropped_time;
};

bool designIsValid(const Design& design);
bool boundsAreUsable(Bounds bounds, float radius);
bool gridDimensionsValid(std::size_t columns, std::size_t rows);
bool gridIndex(std::size_t columns,
               std::size_t rows,
               std::size_t column,
               std::size_t row,
               std::size_t& index);
std::uint32_t latticeHash(std::int32_t x,
                          std::int32_t y,
                          std::int32_t z,
                          std::uint32_t seed);
float hashToUnit(std::uint32_t hash);
float smoothInterpolation(float amount);
bool valueNoise(float x,
                float y,
                float z,
                std::uint32_t seed,
                float& value);
bool generateField(const Design& design,
                   Vec2 offset,
                   float field_time,
                   std::uint32_t seed,
                   ScalarGrid& field);
bool sampleGrid(const ScalarGrid& field,
                float normalized_x,
                float normalized_y,
                float& value);
Vec2 directionFromScalar(float value);
bool sampleDirection(const ScalarGrid& field,
                     Vec2 position,
                     Bounds bounds,
                     Vec2& direction);
bool advanceParticle(Particle& particle,
                     const ScalarGrid& field,
                     float dt,
                     const Design& design,
                     Bounds bounds,
                     bool reduced_motion);
FrameResult advanceFrame(System& system,
                         float frame_dt,
                         const Design& design,
                         Bounds bounds);
bool setFieldOffset(System& system, Vec2 offset);
void setPaused(System& system, bool paused);
void setReducedMotion(System& system, bool reduced_motion);
void setTimeFrozen(System& system, bool frozen);
void reset(System& system,
           Bounds bounds,
           const Design& design,
           std::uint32_t seed,
           Vec2 offset = {0.0f, 0.0f});
bool finiteState(const System& system,
                 const Design& design,
                 Bounds bounds);

}  // namespace flow
