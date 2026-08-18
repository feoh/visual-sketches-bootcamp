#pragma once

#include <cstddef>
#include <vector>

namespace forces {

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

enum class BehaviorMode {
    seek,
    spring_chain
};

struct Design {
    float fixed_step;
    float maximum_frame_time;
    std::size_t maximum_catch_up_steps;
    std::size_t agent_count;
    float agent_radius;
    float mass;
    float maximum_force;
    float maximum_speed;
    float damping_rate;
    float arrival_radius;
    float attraction_strength;
    float repulsion_strength;
    float force_softening;
    float spring_rest_length;
    float spring_stiffness;
    float spring_damping;
    float boundary_restitution;
    Color background;
    Color seek_color;
    Color spring_color;
};

struct Agent {
    Vec2 position;
    Vec2 velocity;
    Vec2 accumulated_force;
};

struct System {
    std::vector<Agent> agents;
    Vec2 target{0.0f, 0.0f};
    Vec2 anchor{0.0f, 0.0f};
    BehaviorMode mode = BehaviorMode::seek;
    float accumulator = 0.0f;
    float dropped_time = 0.0f;
    std::size_t simulated_steps = 0;
    bool paused = false;
};

struct FrameResult {
    std::size_t simulated_steps;
    float dropped_time;
};

bool designIsValid(const Design& design);
bool boundsAreUsable(Bounds bounds, float radius);
bool clampPointToBounds(Vec2 input, Bounds bounds, float inset, Vec2& output);
bool finiteState(const System& system, const Design& design, Bounds bounds);
float magnitude(Vec2 value);
float dot(Vec2 a, Vec2 b);
Vec2 limit(Vec2 value, float maximum);
Vec2 seekForce(const Agent& agent, Vec2 target, const Design& design);
Vec2 softenedRadialForce(Vec2 from,
                         Vec2 source,
                         float signed_strength,
                         float softening);
Vec2 springForce(const Agent& first,
                 const Agent& second,
                 float rest_length,
                 float stiffness,
                 float damping);
void clearForces(System& system);
bool addForce(Agent& agent, Vec2 force);
bool composeForces(System& system, const Design& design);
bool integrateAgent(Agent& agent, float dt, const Design& design, Bounds bounds);
FrameResult advanceFrame(System& system,
                         float frame_dt,
                         const Design& design,
                         Bounds bounds);
void setMode(System& system, BehaviorMode mode);
void setPaused(System& system, bool paused);
void reset(System& system,
           Vec2 center,
           BehaviorMode mode,
           const Design& design,
           Bounds bounds);

}  // namespace forces
