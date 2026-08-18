#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace particles {

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
    float spawn_interval;
    std::size_t maximum_particles;
    float lifetime;
    float drag_rate;
    Vec2 acceleration;
    float particle_radius;
    float restitution;
    std::size_t maximum_history;
    float launch_speed;
    Color background;
    Color young_color;
    Color old_color;
};

struct Particle {
    Particle(Vec2 initial_position,
             Vec2 initial_velocity,
             float lifetime_seconds);

    Vec2 position;
    Vec2 velocity;
    float age;
    float lifetime;
    std::vector<Vec2> history;
};

struct Emitter {
    std::vector<Particle> particles;
    Vec2 origin;
    float accumulator = 0.0f;
    float spawn_clock = 0.0f;
    float dropped_time = 0.0f;
    std::uint32_t random_state = 1u;
    std::size_t total_spawned = 0;
    bool paused = false;
};

struct FrameResult {
    std::size_t simulated_steps;
    std::size_t spawned;
    std::size_t removed;
    float dropped_time;
};

bool designIsValid(const Design& design);
bool boundsAreUsable(Bounds bounds, float radius);
bool originInBounds(Vec2 origin, Bounds bounds, float radius);
float dragMultiplier(float drag_rate, float dt);
void updateParticle(Particle& particle,
                    float dt,
                    const Design& design,
                    Bounds bounds);
void removeExpired(std::vector<Particle>& particles);
FrameResult advanceFrame(Emitter& emitter,
                         float frame_dt,
                         const Design& design,
                         Bounds bounds);
void setPaused(Emitter& emitter, bool paused);
void reset(Emitter& emitter, Vec2 origin, std::uint32_t seed);
bool finiteState(const Emitter& emitter,
                 const Design& design,
                 Bounds bounds);
Color colorForAge(const Particle& particle, const Design& design);

}  // namespace particles
