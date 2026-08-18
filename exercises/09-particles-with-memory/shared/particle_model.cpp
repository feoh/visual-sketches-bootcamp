#include "particle_model.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace particles {
namespace {

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

float unitRandom(std::uint32_t& state) {
    if (state == 0u) {
        state = 0x6d2b79f5u;
    }
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return static_cast<float>(state & 0x00ffffffu) / 16777216.0f;
}

bool checkedFloat(double value, float& output) {
    if (!std::isfinite(value) ||
        std::fabs(value) > std::numeric_limits<float>::max()) {
        return false;
    }
    output = static_cast<float>(value);
    return finite(output);
}

Particle makeParticle(Emitter& emitter, const Design& design) {
    constexpr double tau = 6.28318530717958647692;
    const double random_angle = static_cast<double>(unitRandom(emitter.random_state));
    const double angle = (random_angle * 0.70 + 0.15) * tau;
    const float random_speed = unitRandom(emitter.random_state);
    const float speed = design.launch_speed * (0.75f + 0.5f * random_speed);
    const Vec2 velocity{
        static_cast<float>(std::cos(angle) * speed),
        static_cast<float>(std::sin(angle) * speed)
    };
    return Particle(emitter.origin, velocity, design.lifetime);
}

void reflectAxis(float low,
                 float high,
                 float restitution,
                 float& position,
                 float& velocity) {
    if (position < low) {
        position = low;
        if (velocity < 0.0f) {
            velocity = -velocity * restitution;
        }
    } else if (position > high) {
        position = high;
        if (velocity > 0.0f) {
            velocity = -velocity * restitution;
        }
    }
}

}  // namespace

Particle::Particle(Vec2 initial_position,
                   Vec2 initial_velocity,
                   float lifetime_seconds)
    : position(initial_position),
      velocity(initial_velocity),
      age(0.0f),
      lifetime(lifetime_seconds),
      history{initial_position} {}

bool designIsValid(const Design& design) {
    return finite(design.fixed_step) &&
           design.fixed_step >= 0.00001f &&
           finite(design.maximum_frame_time) &&
           design.maximum_frame_time >= design.fixed_step &&
           design.maximum_frame_time <= 1.0f &&
           design.maximum_catch_up_steps > 0 &&
           design.maximum_catch_up_steps <= 10000 &&
           finite(design.spawn_interval) &&
           design.spawn_interval >= design.fixed_step &&
           design.spawn_interval <= 60.0f &&
           design.maximum_particles > 0 &&
           design.maximum_particles <= 100000 &&
           design.maximum_history > 0 &&
           design.maximum_history <= 100000 &&
           design.maximum_particles <= 1000000 / design.maximum_history &&
           finite(design.lifetime) &&
           design.lifetime > 0.0f &&
           design.lifetime <= 3600.0f &&
           finite(design.drag_rate) &&
           design.drag_rate >= 0.0f &&
           design.drag_rate <= 1000.0f &&
           finite(design.acceleration) &&
           std::fabs(design.acceleration.x) <= 1000000.0f &&
           std::fabs(design.acceleration.y) <= 1000000.0f &&
           finite(design.particle_radius) &&
           design.particle_radius > 0.0f &&
           design.particle_radius <= 100000.0f &&
           finite(design.restitution) &&
           design.restitution >= 0.0f &&
           design.restitution <= 1.0f &&
           finite(design.launch_speed) &&
           design.launch_speed >= 0.0f &&
           design.launch_speed <= 1000000.0f &&
           valid(design.background) &&
           valid(design.young_color) &&
           valid(design.old_color);
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

bool originInBounds(Vec2 origin, Bounds bounds, float radius) {
    if (!boundsAreUsable(bounds, radius) || !finite(origin)) {
        return false;
    }
    const double right = static_cast<double>(bounds.width) - radius;
    const double bottom = static_cast<double>(bounds.height) - radius;
    return static_cast<double>(origin.x) >= radius &&
           static_cast<double>(origin.x) <= right &&
           static_cast<double>(origin.y) >= radius &&
           static_cast<double>(origin.y) <= bottom;
}

float dragMultiplier(float drag_rate, float dt) {
    if (!finite(drag_rate) || drag_rate < 0.0f ||
        !finite(dt) || dt < 0.0f) {
        return 1.0f;
    }
    const double result = std::exp(-static_cast<double>(drag_rate) * dt);
    return std::isfinite(result) ? static_cast<float>(result) : 1.0f;
}

void updateParticle(Particle& particle,
                    float dt,
                    const Design& design,
                    Bounds bounds) {
    if (!designIsValid(design) ||
        !boundsAreUsable(bounds, design.particle_radius) ||
        !finite(dt) || dt <= 0.0f ||
        !finite(particle.position) ||
        !finite(particle.velocity) ||
        !finite(particle.age) ||
        !finite(particle.lifetime)) {
        return;
    }

    // Calculate every next value before committing any mutation.
    const float drag = dragMultiplier(design.drag_rate, dt);
    float next_velocity_x = 0.0f;
    float next_velocity_y = 0.0f;
    float next_position_x = 0.0f;
    float next_position_y = 0.0f;
    float next_age = 0.0f;
    if (!checkedFloat(
            (static_cast<double>(particle.velocity.x) +
             static_cast<double>(design.acceleration.x) * dt) * drag,
            next_velocity_x) ||
        !checkedFloat(
            (static_cast<double>(particle.velocity.y) +
             static_cast<double>(design.acceleration.y) * dt) * drag,
            next_velocity_y) ||
        !checkedFloat(
            static_cast<double>(particle.position.x) +
            static_cast<double>(next_velocity_x) * dt,
            next_position_x) ||
        !checkedFloat(
            static_cast<double>(particle.position.y) +
            static_cast<double>(next_velocity_y) * dt,
            next_position_y) ||
        !checkedFloat(static_cast<double>(particle.age) + dt, next_age)) {
        return;
    }

    particle.velocity = {next_velocity_x, next_velocity_y};
    particle.position = {next_position_x, next_position_y};
    particle.age = next_age;
    reflectAxis(design.particle_radius,
                bounds.width - design.particle_radius,
                design.restitution,
                particle.position.x,
                particle.velocity.x);
    reflectAxis(design.particle_radius,
                bounds.height - design.particle_radius,
                design.restitution,
                particle.position.y,
                particle.velocity.y);
    particle.history.push_back(particle.position);
    if (particle.history.size() > design.maximum_history) {
        const std::size_t excess = particle.history.size() - design.maximum_history;
        particle.history.erase(
            particle.history.begin(),
            particle.history.begin() + static_cast<std::ptrdiff_t>(excess));
    }
}

void removeExpired(std::vector<Particle>& particles) {
    const auto expired = [](const Particle& particle) {
        return !finite(particle.age) ||
               !finite(particle.lifetime) ||
               particle.age >= particle.lifetime;
    };
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), expired),
        particles.end());
}

FrameResult advanceFrame(Emitter& emitter,
                         float frame_dt,
                         const Design& design,
                         Bounds bounds) {
    const FrameResult rejected{0, 0, 0, 0.0f};
    if (!designIsValid(design) ||
        !originInBounds(emitter.origin, bounds, design.particle_radius) ||
        !finite(frame_dt) || frame_dt < 0.0f ||
        !finite(emitter.accumulator) || emitter.accumulator < 0.0f ||
        emitter.accumulator >= design.fixed_step ||
        !finite(emitter.spawn_clock) || emitter.spawn_clock < 0.0f ||
        emitter.spawn_clock >= design.spawn_interval ||
        !finite(emitter.dropped_time) || emitter.dropped_time < 0.0f) {
        return rejected;
    }

    if (emitter.paused) {
        emitter.accumulator = 0.0f;
        return rejected;
    }

    Emitter next = emitter;
    FrameResult result{0, 0, 0, 0.0f};
    const float accepted = std::min(frame_dt, design.maximum_frame_time);
    result.dropped_time = frame_dt - accepted;
    double accumulated = static_cast<double>(next.accumulator) + accepted;
    if (!std::isfinite(accumulated)) {
        return rejected;
    }

    const double tolerance = static_cast<double>(design.fixed_step) * 1e-6;
    const double steps = std::floor((accumulated + tolerance) / design.fixed_step);
    if (!std::isfinite(steps) || steps < 0.0 ||
        steps > static_cast<double>(std::numeric_limits<std::size_t>::max())) {
        return rejected;
    }
    const std::size_t available_steps = static_cast<std::size_t>(steps);
    result.simulated_steps = std::min(available_steps, design.maximum_catch_up_steps);

    for (std::size_t step = 0; step < result.simulated_steps; ++step) {
        next.spawn_clock += design.fixed_step;
        if (next.spawn_clock + design.fixed_step * 1e-4f >= design.spawn_interval) {
            next.spawn_clock = std::fmod(next.spawn_clock, design.spawn_interval);
            if (next.particles.size() < design.maximum_particles) {
                // Spawn before update so every new particle receives this fixed step.
                next.particles.push_back(makeParticle(next, design));
                if (next.total_spawned < std::numeric_limits<std::size_t>::max()) {
                    ++next.total_spawned;
                }
                ++result.spawned;
            }
        }

        for (Particle& particle : next.particles) {
            updateParticle(particle, design.fixed_step, design, bounds);
        }
        const std::size_t size_before_removal = next.particles.size();
        removeExpired(next.particles);
        result.removed += size_before_removal - next.particles.size();
    }

    accumulated -= static_cast<double>(result.simulated_steps) * design.fixed_step;
    if (available_steps > result.simulated_steps) {
        // Drop capped whole steps explicitly; retain only a substep remainder.
        const std::size_t dropped_steps = available_steps - result.simulated_steps;
        const double dropped = static_cast<double>(dropped_steps) * design.fixed_step;
        accumulated -= dropped;
        const double frame_drop = static_cast<double>(result.dropped_time) + dropped;
        if (!checkedFloat(frame_drop, result.dropped_time)) {
            return rejected;
        }
    }

    float next_accumulator = 0.0f;
    float next_dropped_time = 0.0f;
    if (!checkedFloat(std::max(0.0, accumulated), next_accumulator) ||
        !checkedFloat(
            static_cast<double>(next.dropped_time) + result.dropped_time,
            next_dropped_time)) {
        return rejected;
    }
    next.accumulator = next_accumulator;
    next.dropped_time = next_dropped_time;

    emitter = std::move(next);
    return result;
}

void setPaused(Emitter& emitter, bool paused) {
    emitter.paused = paused;
    emitter.accumulator = 0.0f;
}

void reset(Emitter& emitter, Vec2 origin, std::uint32_t seed) {
    emitter = Emitter{};
    emitter.origin = origin;
    emitter.random_state = seed == 0u ? 0x6d2b79f5u : seed;
}

bool finiteState(const Emitter& emitter,
                 const Design& design,
                 Bounds bounds) {
    if (!designIsValid(design) ||
        !originInBounds(emitter.origin, bounds, design.particle_radius) ||
        !finite(emitter.accumulator) || emitter.accumulator < 0.0f ||
        emitter.accumulator >= design.fixed_step ||
        !finite(emitter.spawn_clock) || emitter.spawn_clock < 0.0f ||
        emitter.spawn_clock >= design.spawn_interval ||
        !finite(emitter.dropped_time) || emitter.dropped_time < 0.0f ||
        emitter.particles.size() > design.maximum_particles) {
        return false;
    }

    for (const Particle& particle : emitter.particles) {
        if (!originInBounds(particle.position, bounds, design.particle_radius) ||
            !finite(particle.velocity) ||
            !finite(particle.age) || particle.age < 0.0f ||
            !finite(particle.lifetime) || particle.lifetime <= 0.0f ||
            particle.history.empty() ||
            particle.history.size() > design.maximum_history) {
            return false;
        }
        for (Vec2 point : particle.history) {
            if (!originInBounds(point, bounds, design.particle_radius)) {
                return false;
            }
        }
    }
    return true;
}

Color colorForAge(const Particle& particle, const Design& design) {
    const float amount = particle.lifetime > 0.0f
        ? std::clamp(particle.age / particle.lifetime, 0.0f, 1.0f)
        : 1.0f;
    const auto channel = [amount](int young, int old) {
        return static_cast<int>(std::lround(young + (old - young) * amount));
    };
    return {
        channel(design.young_color.r, design.old_color.r),
        channel(design.young_color.g, design.old_color.g),
        channel(design.young_color.b, design.old_color.b)
    };
}

}  // namespace particles
