#include "force_model.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace forces {
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

bool checkedFloat(double value, float& output) {
    if (!std::isfinite(value) ||
        std::fabs(value) > std::numeric_limits<float>::max()) {
        return false;
    }
    output = static_cast<float>(value);
    return finite(output);
}

bool limitedVector(double x,
                   double y,
                   float maximum,
                   Vec2& output) {
    if (!std::isfinite(x) || !std::isfinite(y) ||
        !finite(maximum) || maximum < 0.0f) {
        return false;
    }
    const double length = std::hypot(x, y);
    if (!std::isfinite(length)) {
        return false;
    }
    if (length > static_cast<double>(maximum) && length > 0.0) {
        const double factor = static_cast<double>(maximum) / length;
        x *= factor;
        y *= factor;
    }
    float converted_x = 0.0f;
    float converted_y = 0.0f;
    if (!checkedFloat(x, converted_x) || !checkedFloat(y, converted_y)) {
        return false;
    }
    output = {converted_x, converted_y};
    return true;
}

Vec2 saturatedVector(double x, double y) {
    Vec2 output{0.0f, 0.0f};
    limitedVector(x, y, std::numeric_limits<float>::max(), output);
    return output;
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

bool designIsValid(const Design& design) {
    return finite(design.fixed_step) &&
           design.fixed_step >= 0.00001f &&
           finite(design.maximum_frame_time) &&
           design.maximum_frame_time >= design.fixed_step &&
           design.maximum_frame_time <= 1.0f &&
           design.maximum_catch_up_steps > 0 &&
           design.maximum_catch_up_steps <= 10000 &&
           design.agent_count >= 2 &&
           design.agent_count <= 64 &&
           finite(design.agent_radius) &&
           design.agent_radius > 0.0f &&
           design.agent_radius <= 1000.0f &&
           finite(design.mass) && design.mass > 0.0f && design.mass <= 10000.0f &&
           finite(design.maximum_force) &&
           design.maximum_force > 0.0f &&
           design.maximum_force <= 1000000.0f &&
           finite(design.maximum_speed) &&
           design.maximum_speed > 0.0f &&
           design.maximum_speed <= 1000000.0f &&
           finite(design.damping_rate) &&
           design.damping_rate >= 0.0f && design.damping_rate <= 1000.0f &&
           finite(design.arrival_radius) && design.arrival_radius > 0.0f &&
           finite(design.attraction_strength) && design.attraction_strength >= 0.0f &&
           finite(design.repulsion_strength) && design.repulsion_strength >= 0.0f &&
           finite(design.force_softening) && design.force_softening > 0.0f &&
           finite(design.spring_rest_length) && design.spring_rest_length > 0.0f &&
           finite(design.spring_stiffness) && design.spring_stiffness >= 0.0f &&
           finite(design.spring_damping) && design.spring_damping >= 0.0f &&
           finite(design.boundary_restitution) &&
           design.boundary_restitution >= 0.0f &&
           design.boundary_restitution <= 1.0f &&
           valid(design.background) && valid(design.seek_color) &&
           valid(design.spring_color);
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

bool clampPointToBounds(Vec2 input, Bounds bounds, float inset, Vec2& output) {
    if (!finite(input) || !boundsAreUsable(bounds, inset)) {
        return false;
    }
    output = {
        std::clamp(input.x, inset, bounds.width - inset),
        std::clamp(input.y, inset, bounds.height - inset)
    };
    return finite(output);
}

float magnitude(Vec2 value) {
    if (!finite(value)) {
        return std::numeric_limits<float>::infinity();
    }
    const double length = std::hypot(static_cast<double>(value.x),
                                     static_cast<double>(value.y));
    if (length >= std::numeric_limits<float>::max()) {
        return std::numeric_limits<float>::max();
    }
    float result = 0.0f;
    return checkedFloat(length, result)
        ? result
        : std::numeric_limits<float>::infinity();
}

float dot(Vec2 a, Vec2 b) {
    if (!finite(a) || !finite(b)) {
        return 0.0f;
    }
    const double result = static_cast<double>(a.x) * b.x +
                          static_cast<double>(a.y) * b.y;
    const double boundary = std::numeric_limits<float>::max();
    return static_cast<float>(std::clamp(result, -boundary, boundary));
}

Vec2 limit(Vec2 value, float maximum) {
    Vec2 result{0.0f, 0.0f};
    return finite(value) && limitedVector(value.x, value.y, maximum, result)
        ? result
        : Vec2{0.0f, 0.0f};
}

Vec2 seekForce(const Agent& agent, Vec2 target, const Design& design) {
    if (!designIsValid(design) || !finite(agent.position) ||
        !finite(agent.velocity) || !finite(target)) {
        return {0.0f, 0.0f};
    }
    const double offset_x = static_cast<double>(target.x) - agent.position.x;
    const double offset_y = static_cast<double>(target.y) - agent.position.y;
    const double distance = std::hypot(offset_x, offset_y);
    if (!std::isfinite(distance)) {
        return {0.0f, 0.0f};
    }
    double desired_x = 0.0;
    double desired_y = 0.0;
    if (distance > 0.0) {
        const double arrival = distance / design.arrival_radius;
        const double desired_speed = static_cast<double>(design.maximum_speed) *
                                     std::min(1.0, arrival);
        desired_x = offset_x / distance * desired_speed;
        desired_y = offset_y / distance * desired_speed;
    }
    const double response = static_cast<double>(design.mass) /
                            design.fixed_step;
    Vec2 result{0.0f, 0.0f};
    return limitedVector((desired_x - agent.velocity.x) * response,
                         (desired_y - agent.velocity.y) * response,
                         design.maximum_force,
                         result)
        ? result
        : Vec2{0.0f, 0.0f};
}

Vec2 softenedRadialForce(Vec2 from,
                         Vec2 source,
                         float signed_strength,
                         float softening) {
    if (!finite(from) || !finite(source) || !finite(signed_strength) ||
        !finite(softening) || softening <= 0.0f) {
        return {0.0f, 0.0f};
    }
    const double offset_x = static_cast<double>(source.x) - from.x;
    const double offset_y = static_cast<double>(source.y) - from.y;
    const double distance = std::hypot(offset_x, offset_y);
    if (!std::isfinite(distance) || distance == 0.0) {
        return {0.0f, 0.0f};
    }
    const double denominator = distance * distance +
                               static_cast<double>(softening) * softening;
    const double amount = static_cast<double>(signed_strength) / denominator;
    return saturatedVector(offset_x / distance * amount,
                           offset_y / distance * amount);
}

Vec2 springForce(const Agent& first,
                 const Agent& second,
                 float rest_length,
                 float stiffness,
                 float damping) {
    if (!finite(first.position) || !finite(first.velocity) ||
        !finite(second.position) || !finite(second.velocity) ||
        !finite(rest_length) || rest_length < 0.0f ||
        !finite(stiffness) || stiffness < 0.0f ||
        !finite(damping) || damping < 0.0f) {
        return {0.0f, 0.0f};
    }
    const double offset_x = static_cast<double>(second.position.x) -
                            first.position.x;
    const double offset_y = static_cast<double>(second.position.y) -
                            first.position.y;
    const double distance = std::hypot(offset_x, offset_y);
    if (!std::isfinite(distance) || distance == 0.0) {
        return {0.0f, 0.0f};
    }
    const double axis_x = offset_x / distance;
    const double axis_y = offset_y / distance;
    const double relative_x = static_cast<double>(second.velocity.x) -
                              first.velocity.x;
    const double relative_y = static_cast<double>(second.velocity.y) -
                              first.velocity.y;
    const double relative_along_axis = relative_x * axis_x +
                                       relative_y * axis_y;
    const double force_magnitude =
        static_cast<double>(stiffness) * (distance - rest_length) +
        static_cast<double>(damping) * relative_along_axis;
    return saturatedVector(axis_x * force_magnitude,
                           axis_y * force_magnitude);
}

void clearForces(System& system) {
    for (Agent& agent : system.agents) {
        agent.accumulated_force = {0.0f, 0.0f};
    }
}

bool addForce(Agent& agent, Vec2 force) {
    if (!finite(agent.accumulated_force) || !finite(force)) {
        return false;
    }
    Vec2 sum{0.0f, 0.0f};
    if (!limitedVector(
            static_cast<double>(agent.accumulated_force.x) + force.x,
            static_cast<double>(agent.accumulated_force.y) + force.y,
            std::numeric_limits<float>::max(),
            sum)) {
        return false;
    }
    agent.accumulated_force = sum;
    return true;
}

bool composeForces(System& system, const Design& design) {
    if (!designIsValid(design) || !finite(system.target) ||
        !finite(system.anchor) || system.agents.empty() ||
        (system.mode != BehaviorMode::seek &&
         system.mode != BehaviorMode::spring_chain)) {
        return false;
    }
    for (const Agent& agent : system.agents) {
        if (!finite(agent.position) || !finite(agent.velocity) ||
            !finite(agent.accumulated_force)) {
            return false;
        }
    }

    System next = system;
    clearForces(next);
    if (next.mode == BehaviorMode::seek) {
        for (Agent& agent : next.agents) {
            if (!addForce(agent, seekForce(agent, next.target, design)) ||
                !addForce(agent,
                          softenedRadialForce(agent.position,
                                              next.target,
                                              design.attraction_strength,
                                              design.force_softening))) {
                return false;
            }
        }
        for (std::size_t first = 0; first < next.agents.size(); ++first) {
            for (std::size_t second = first + 1; second < next.agents.size(); ++second) {
                const Vec2 repulsion = softenedRadialForce(
                    next.agents[first].position,
                    next.agents[second].position,
                    -design.repulsion_strength,
                    design.force_softening);
                const Vec2 opposite{-repulsion.x, -repulsion.y};
                if (!addForce(next.agents[first], repulsion) ||
                    !addForce(next.agents[second], opposite)) {
                    return false;
                }
            }
        }
        system = std::move(next);
        return true;
    }

    const Agent fixed_anchor{next.anchor, {0.0f, 0.0f}, {0.0f, 0.0f}};
    if (!addForce(next.agents.front(),
                  springForce(next.agents.front(),
                              fixed_anchor,
                              design.spring_rest_length,
                              design.spring_stiffness,
                              design.spring_damping))) {
        return false;
    }
    for (std::size_t index = 0; index + 1 < next.agents.size(); ++index) {
        const Vec2 force = springForce(next.agents[index],
                                       next.agents[index + 1],
                                       design.spring_rest_length,
                                       design.spring_stiffness,
                                       design.spring_damping);
        const Vec2 opposite{-force.x, -force.y};
        if (!addForce(next.agents[index], force) ||
            !addForce(next.agents[index + 1], opposite)) {
            return false;
        }
    }
    system = std::move(next);
    return true;
}

bool integrateAgent(Agent& agent, float dt, const Design& design, Bounds bounds) {
    if (!designIsValid(design) || !boundsAreUsable(bounds, design.agent_radius) ||
        !finite(dt) || dt <= 0.0f || !finite(agent.position) ||
        !finite(agent.velocity) || !finite(agent.accumulated_force)) {
        return false;
    }

    const Vec2 force = limit(agent.accumulated_force, design.maximum_force);
    const double drag = std::exp(-static_cast<double>(design.damping_rate) * dt);
    float velocity_x = 0.0f;
    float velocity_y = 0.0f;
    if (!checkedFloat((static_cast<double>(agent.velocity.x) +
                       static_cast<double>(force.x) / design.mass * dt) * drag,
                      velocity_x) ||
        !checkedFloat((static_cast<double>(agent.velocity.y) +
                       static_cast<double>(force.y) / design.mass * dt) * drag,
                      velocity_y)) {
        return false;
    }
    const Vec2 velocity = limit({velocity_x, velocity_y}, design.maximum_speed);
    float position_x = 0.0f;
    float position_y = 0.0f;
    if (!checkedFloat(static_cast<double>(agent.position.x) + velocity.x * dt,
                      position_x) ||
        !checkedFloat(static_cast<double>(agent.position.y) + velocity.y * dt,
                      position_y)) {
        return false;
    }

    Agent next{{position_x, position_y}, velocity, {0.0f, 0.0f}};
    reflectAxis(design.agent_radius,
                bounds.width - design.agent_radius,
                design.boundary_restitution,
                next.position.x,
                next.velocity.x);
    reflectAxis(design.agent_radius,
                bounds.height - design.agent_radius,
                design.boundary_restitution,
                next.position.y,
                next.velocity.y);
    agent = next;
    return true;
}

FrameResult advanceFrame(System& system,
                         float frame_dt,
                         const Design& design,
                         Bounds bounds) {
    const FrameResult rejected{0, 0.0f};
    if (!finiteState(system, design, bounds) || !finite(frame_dt) || frame_dt < 0.0f) {
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
    const double step_count = std::floor((accumulated + tolerance) / design.fixed_step);
    if (!std::isfinite(step_count) || step_count < 0.0 ||
        step_count > static_cast<double>(std::numeric_limits<std::size_t>::max())) {
        return rejected;
    }
    const std::size_t available = static_cast<std::size_t>(step_count);
    const std::size_t simulated = std::min(available, design.maximum_catch_up_steps);
    if (next.simulated_steps >
        std::numeric_limits<std::size_t>::max() - simulated) {
        return rejected;
    }

    for (std::size_t step = 0; step < simulated; ++step) {
        if (!composeForces(next, design)) {
            return rejected;
        }
        for (Agent& agent : next.agents) {
            if (!integrateAgent(agent, design.fixed_step, design, bounds)) {
                return rejected;
            }
        }
        ++next.simulated_steps;
    }

    accumulated -= static_cast<double>(simulated) * design.fixed_step;
    if (available > simulated) {
        const std::size_t dropped_steps = available - simulated;
        accumulated -= static_cast<double>(dropped_steps) * design.fixed_step;
        if (!checkedFloat(static_cast<double>(dropped) +
                              static_cast<double>(dropped_steps) * design.fixed_step,
                          dropped)) {
            return rejected;
        }
    }
    float next_accumulator = 0.0f;
    float total_dropped = 0.0f;
    if (!checkedFloat(std::max(0.0, accumulated), next_accumulator) ||
        !checkedFloat(static_cast<double>(next.dropped_time) + dropped,
                      total_dropped)) {
        return rejected;
    }
    next.accumulator = next_accumulator;
    next.dropped_time = total_dropped;
    system = std::move(next);
    return {simulated, dropped};
}

void setMode(System& system, BehaviorMode mode) {
    system.mode = mode;
    system.accumulator = 0.0f;
    clearForces(system);
}

void setPaused(System& system, bool paused) {
    system.paused = paused;
    system.accumulator = 0.0f;
    clearForces(system);
}

void reset(System& system,
           Vec2 center,
           BehaviorMode mode,
           const Design& design,
           Bounds bounds) {
    System next;
    if (!designIsValid(design) || !boundsAreUsable(bounds, design.agent_radius) ||
        !finite(center)) {
        system = next;
        return;
    }
    if (!clampPointToBounds(center, bounds, design.agent_radius, center)) {
        system = next;
        return;
    }
    next.target = center;
    next.anchor = center;
    next.mode = mode;
    next.agents.reserve(design.agent_count);
    const double midpoint = static_cast<double>(design.agent_count - 1) * 0.5;
    for (std::size_t index = 0; index < design.agent_count; ++index) {
        const double offset = (static_cast<double>(index) - midpoint) *
                              design.spring_rest_length;
        const double position_x = std::clamp(
            static_cast<double>(center.x) + offset,
            static_cast<double>(design.agent_radius),
            static_cast<double>(bounds.width) - design.agent_radius);
        float converted_x = 0.0f;
        if (!checkedFloat(position_x, converted_x)) {
            system = System{};
            return;
        }
        next.agents.push_back(
            {{converted_x, center.y}, {0.0f, 0.0f}, {0.0f, 0.0f}});
    }
    system = std::move(next);
}

bool finiteState(const System& system, const Design& design, Bounds bounds) {
    Vec2 ignored{0.0f, 0.0f};
    if (!designIsValid(design) || !boundsAreUsable(bounds, design.agent_radius) ||
        !clampPointToBounds(system.target, bounds, design.agent_radius, ignored) ||
        ignored.x != system.target.x || ignored.y != system.target.y ||
        !clampPointToBounds(system.anchor, bounds, design.agent_radius, ignored) ||
        ignored.x != system.anchor.x || ignored.y != system.anchor.y ||
        (system.mode != BehaviorMode::seek &&
         system.mode != BehaviorMode::spring_chain) ||
        !finite(system.accumulator) || system.accumulator < 0.0f ||
        system.accumulator >= design.fixed_step ||
        !finite(system.dropped_time) || system.dropped_time < 0.0f ||
        system.agents.size() != design.agent_count) {
        return false;
    }
    for (const Agent& agent : system.agents) {
        if (!finite(agent.position) || !finite(agent.velocity) ||
            !finite(agent.accumulated_force) ||
            agent.position.x < design.agent_radius ||
            agent.position.x > bounds.width - design.agent_radius ||
            agent.position.y < design.agent_radius ||
            agent.position.y > bounds.height - design.agent_radius ||
            magnitude(agent.velocity) > design.maximum_speed + 0.01f) {
            return false;
        }
    }
    return true;
}

}  // namespace forces
