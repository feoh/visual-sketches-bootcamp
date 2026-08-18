#include "force_design.h"
#include "force_model.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
int failures = 0;
void expect(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}
bool near(float a, float b, float tolerance = 0.0005f) {
    return std::isfinite(a) && std::isfinite(b) &&
           std::fabs(a - b) <= tolerance;
}
void expectNear(float actual, float expected, const std::string& message,
                float tolerance = 0.0005f) {
    if (!near(actual, expected, tolerance)) {
        ++failures;
        std::cerr << "FAIL: " << message << " actual=" << actual
                  << " expected=" << expected << '\n';
    }
}
forces::Design known() {
    return {0.01f, 0.25f, 8, 3, 2.0f, 1.0f, 100.0f, 50.0f,
            1.0f, 20.0f, 100.0f, 80.0f, 5.0f, 10.0f, 2.0f,
            1.0f, 0.5f, {245, 245, 240}, {20, 100, 150}, {210, 70, 50}};
}
forces::Agent agent(forces::Vec2 position, forces::Vec2 velocity = {0, 0}) {
    return {position, velocity, {0, 0}};
}
bool sameSystem(const forces::System& first, const forces::System& second) {
    if (first.agents.size() != second.agents.size() ||
        first.target.x != second.target.x || first.target.y != second.target.y ||
        first.anchor.x != second.anchor.x || first.anchor.y != second.anchor.y ||
        first.mode != second.mode || first.accumulator != second.accumulator ||
        first.dropped_time != second.dropped_time ||
        first.simulated_steps != second.simulated_steps ||
        first.paused != second.paused) {
        return false;
    }
    for (std::size_t index = 0; index < first.agents.size(); ++index) {
        const auto& a = first.agents[index];
        const auto& b = second.agents[index];
        if (a.position.x != b.position.x || a.position.y != b.position.y ||
            a.velocity.x != b.velocity.x || a.velocity.y != b.velocity.y ||
            a.accumulated_force.x != b.accumulated_force.x ||
            a.accumulated_force.y != b.accumulated_force.y) {
            return false;
        }
    }
    return true;
}
struct FixtureRow { std::string name; float expected; };
std::vector<FixtureRow> fixture(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "spring oracle opens");
    std::vector<FixtureRow> rows;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        FixtureRow row{};
        std::string extra;
        std::istringstream fields(line);
        const bool exact = static_cast<bool>(fields >> row.name >> row.expected) &&
                           !(fields >> extra);
        expect(exact, "oracle row has exactly two fields");
        if (exact) rows.push_back(row);
    }
    return rows;
}
void oracleCases(const std::vector<FixtureRow>& rows) {
    expect(rows.size() == 3, "oracle has three rows");
    const auto first = agent({0, 0});
    const auto at_rest = agent({10, 0});
    const auto stretched = agent({20, 0});
    const auto damped = agent({20, 0}, {-10, 0});
    if (rows.size() == 3) {
        expectNear(forces::springForce(first, at_rest, 10, 2, 1).x,
                   rows[0].expected, rows[0].name);
        expectNear(forces::springForce(first, stretched, 10, 2, 1).x,
                   rows[1].expected, rows[1].name);
        expectNear(forces::springForce(first, damped, 10, 2, 1).x,
                   rows[2].expected, rows[2].name);
    }
}
void vectorAndLimitCases() {
    expectNear(forces::magnitude({3, 4}), 5, "3-4-5 magnitude");
    expectNear(forces::dot({1, 2}, {3, 4}), 11, "dot product known case");
    const auto limited = forces::limit({30, 40}, 10);
    expectNear(forces::magnitude(limited), 10, "vector limiter caps magnitude");
    expectNear(forces::limit({3, 4}, 10).x, 3, "limiter preserves short vector");
    const float extreme = std::numeric_limits<float>::max();
    expect(forces::magnitude({extreme, extreme}) == extreme,
           "extreme finite magnitude saturates at the documented float boundary");
    expect(forces::dot({extreme, extreme}, {extreme, extreme}) == extreme,
           "extreme finite dot product saturates rather than narrowing out of range");
    const auto extreme_limited = forces::limit({extreme, extreme}, 100.0f);
    expectNear(forces::magnitude(extreme_limited), 100.0f,
               "extreme finite vector limits portably", 0.002f);
}

void seekCases() {
    auto design = known();
    design.maximum_force = 10000.0f;
    const auto outside = forces::seekForce(agent({0, 0}), {30, 40}, design);
    expectNear(outside.x, 3000.0f, "outside arrival seeks in target x direction");
    expectNear(outside.y, 4000.0f, "outside arrival uses full desired speed");
    const auto inside = forces::seekForce(agent({0, 0}), {6, 8}, design);
    expectNear(inside.x, 1500.0f, "inside arrival scales desired x speed");
    expectNear(inside.y, 2000.0f, "inside arrival scales desired y speed");
    expect(forces::magnitude(inside) < forces::magnitude(outside),
           "arrival force is reduced near the target");
    const auto braking = forces::seekForce(agent({5, 5}, {3, 4}), {5, 5}, design);
    expectNear(braking.x, -300.0f, "coincident target brakes velocity x");
    expectNear(braking.y, -400.0f, "coincident target brakes velocity y");
}
void springAndRadialCases() {
    const auto first = agent({0, 0});
    const auto second = agent({6, 8});
    const auto force_first = forces::springForce(first, second, 5, 2, 0);
    const auto force_second = forces::springForce(second, first, 5, 2, 0);
    expectNear(force_first.x, 6, "2D spring force follows axis x");
    expectNear(force_first.y, 8, "2D spring force follows axis y");
    expectNear(force_first.x, -force_second.x,
               "2D spring endpoints receive symmetric x forces");
    expectNear(force_first.y, -force_second.y,
               "2D spring endpoints receive symmetric y forces");
    const auto at_rest = forces::springForce(first, agent({3, 4}), 5, 2, 3);
    expectNear(forces::magnitude(at_rest), 0, "rest length is equilibrium in 2D");
    const auto damped = forces::springForce(
        first, agent({6, 8}, {-3, -4}), 5, 2, 1);
    expectNear(damped.x, 3, "axial approach damping reduces spring x");
    expectNear(damped.y, 4, "axial approach damping reduces spring y");
    const auto sideways = forces::springForce(
        first, agent({6, 8}, {-4, 3}), 5, 2, 1);
    expectNear(sideways.x, force_first.x,
               "velocity perpendicular to spring axis adds no x damping");
    expectNear(sideways.y, force_first.y,
               "velocity perpendicular to spring axis adds no y damping");

    const auto zero = forces::softenedRadialForce({5, 5}, {5, 5}, 100, 4);
    expectNear(forces::magnitude(zero), 0,
               "zero-distance radial force has no arbitrary direction");
    const auto attraction = forces::softenedRadialForce({0, 0}, {3, 4}, 100, 5);
    const auto farther = forces::softenedRadialForce({0, 0}, {6, 8}, 100, 5);
    const auto repulsion = forces::softenedRadialForce({0, 0}, {3, 4}, -100, 5);
    expectNear(attraction.x, 1.2f, "softened radial x follows unit direction");
    expectNear(attraction.y, 1.6f, "softened radial y follows unit direction");
    expectNear(forces::magnitude(attraction), 2.0f,
               "softened radial magnitude uses distance squared plus softening squared");
    expectNear(forces::magnitude(farther), 0.8f,
               "softened radial magnitude decreases with distance");
    expectNear(repulsion.x, -attraction.x, "negative strength reverses x direction");
    expectNear(repulsion.y, -attraction.y, "negative strength reverses y direction");
}

void extremeFiniteForceCases() {
    const float extreme = std::numeric_limits<float>::max();
    const auto radial = forces::softenedRadialForce(
        {-extreme, -extreme}, {extreme, extreme}, extreme,
        std::numeric_limits<float>::denorm_min());
    expect(std::isfinite(radial.x) && std::isfinite(radial.y),
           "extreme finite radial force remains representable");
    const auto spring = forces::springForce(
        agent({-extreme, -extreme}, {-extreme, extreme}),
        agent({extreme, extreme}, {extreme, -extreme}),
        std::numeric_limits<float>::denorm_min(), extreme, extreme);
    expect(std::isfinite(spring.x) && std::isfinite(spring.y),
           "extreme finite spring force saturates representably");

    auto extreme_spacing = known();
    extreme_spacing.spring_rest_length = extreme;
    forces::System spaced;
    forces::reset(spaced, {500, 500}, forces::BehaviorMode::spring_chain,
                  extreme_spacing, {1000, 1000});
    expect(forces::finiteState(spaced, extreme_spacing, {1000, 1000}),
           "extreme finite reset spacing clamps without float overflow");

    auto design = known();
    design.attraction_strength = extreme;
    design.repulsion_strength = extreme;
    design.spring_stiffness = extreme;
    design.spring_damping = extreme;
    forces::System system;
    forces::reset(system, {500, 500}, forces::BehaviorMode::seek,
                  design, {1000, 1000});
    system.target = {extreme, -extreme};
    expect(forces::composeForces(system, design),
           "extreme finite seek composition succeeds with explicit saturation");
    for (const auto& item : system.agents) {
        expect(std::isfinite(item.accumulated_force.x) &&
                   std::isfinite(item.accumulated_force.y),
               "extreme seek accumulator remains finite");
    }
    forces::setMode(system, forces::BehaviorMode::spring_chain);
    system.anchor = {extreme, extreme};
    system.agents[0].position = {-extreme, extreme};
    system.agents[1].position = {extreme, -extreme};
    system.agents[0].velocity = {extreme, -extreme};
    system.agents[1].velocity = {-extreme, extreme};
    expect(forces::composeForces(system, design),
           "extreme finite spring composition succeeds with explicit saturation");
    for (const auto& item : system.agents) {
        expect(std::isfinite(item.accumulated_force.x) &&
                   std::isfinite(item.accumulated_force.y),
               "extreme spring accumulator remains finite");
    }
}
void accumulationAndCapsCases() {
    auto design = known();
    design.damping_rate = 0.0f;
    design.maximum_speed = 1000.0f;
    forces::Agent value = agent({50, 50});
    expect(forces::addForce(value, {60, 0}), "first finite force accumulates");
    expect(forces::addForce(value, {60, 0}), "second finite force accumulates");
    expectNear(value.accumulated_force.x, 120, "small behavior forces accumulate before cap");
    const bool integrated = forces::integrateAgent(value, 1.0f, design, {1000, 1000});
    expect(integrated, "valid accumulated force integrates");
    expectNear(value.velocity.x, 100, "maximum force caps the accumulated sum", 0.001f);
    expectNear(value.accumulated_force.x, 0, "integration clears force for the next step");
    auto speed_design = design;
    speed_design.maximum_force = 1000.0f;
    speed_design.maximum_speed = 50.0f;
    auto fast = agent({50, 50});
    expect(forces::addForce(fast, {1000, 0}), "speed-cap force accumulates");
    expect(forces::integrateAgent(fast, 1.0f, speed_design, {1000, 1000}),
           "speed-cap case integrates");
    expectNear(fast.velocity.x, 50, "maximum speed independently caps velocity", 0.001f);
    auto seeker = agent({10, 10}, {-50, 0});
    expect(forces::magnitude(forces::seekForce(seeker, {900, 10}, design)) <=
               design.maximum_force + 0.001f,
           "seek force respects maximum force");
    const float extreme = std::numeric_limits<float>::max();
    auto saturated = agent({0, 0});
    saturated.accumulated_force = {extreme, extreme};
    expect(forces::addForce(saturated, {extreme, extreme}),
           "finite accumulator overflow saturates explicitly");
    expect(std::isfinite(saturated.accumulated_force.x) &&
               std::isfinite(saturated.accumulated_force.y),
           "saturated accumulator remains finite");
    const auto before_invalid_add = saturated.accumulated_force;
    expect(!forces::addForce(saturated,
                             {std::numeric_limits<float>::quiet_NaN(), 0}),
           "non-finite contribution is reported instead of silently omitted");
    expect(saturated.accumulated_force.x == before_invalid_add.x &&
               saturated.accumulated_force.y == before_invalid_add.y,
           "failed force addition leaves the accumulator unchanged");
}
void modeAndCompositionCases() {
    const auto design = known();
    forces::System system;
    forces::reset(system, {100, 100}, forces::BehaviorMode::seek,
                  design, {300, 200});
    system.accumulator = 0.005f;
    system.agents[0].accumulated_force = {4, 5};
    forces::setMode(system, forces::BehaviorMode::spring_chain);
    expect(system.mode == forces::BehaviorMode::spring_chain,
           "mode state transition is explicit");
    expectNear(system.accumulator, 0, "mode transition clears partial old-mode time");
    expectNear(forces::magnitude(system.agents[0].accumulated_force), 0,
               "mode transition clears old forces");
    expect(forces::composeForces(system, design),
           "spring behavior composition reports success");
    expect(std::isfinite(system.agents[0].accumulated_force.x),
           "spring behavior composition remains finite");
    forces::setMode(system, forces::BehaviorMode::seek);
    expect(forces::composeForces(system, design),
           "seek behavior composition reports success");
    const auto first_composition = system.agents.front().accumulated_force;
    expect(forces::composeForces(system, design),
           "repeated seek composition reports success");
    expectNear(system.agents.front().accumulated_force.x, first_composition.x,
               "each composition clears the previous x force");
    expectNear(system.agents.front().accumulated_force.y, first_composition.y,
               "each composition clears the previous y force");
    for (const auto& item : system.agents) {
        expect(forces::magnitude(item.accumulated_force) < 100000.0f,
               "seek plus pair repulsion composes bounded finite inputs");
    }
    system.mode = static_cast<forces::BehaviorMode>(99);
    const auto before_invalid_mode = system;
    expect(!forces::composeForces(system, design) &&
               sameSystem(system, before_invalid_mode),
           "failed composition reports failure and leaves the system unchanged");
}
void fixedStepPartitionCases() {
    const auto design = known();
    forces::System once;
    forces::System split;
    forces::reset(once, {100, 100}, forces::BehaviorMode::seek,
                  design, {300, 200});
    split = once;
    once.target = split.target = {180, 120};
    const auto one_result = forces::advanceFrame(once, 0.08f, design, {300, 200});
    const auto first = forces::advanceFrame(split, 0.03f, design, {300, 200});
    const auto second = forces::advanceFrame(split, 0.05f, design, {300, 200});
    expect(one_result.simulated_steps == 8 &&
               first.simulated_steps + second.simulated_steps == 8,
           "fixed-step count is independent of render partition");
    for (std::size_t i = 0; i < once.agents.size(); ++i) {
        expectNear(once.agents[i].position.x, split.agents[i].position.x,
                   "partition preserves x", 0.002f);
        expectNear(once.agents[i].position.y, split.agents[i].position.y,
                   "partition preserves y", 0.002f);
    }
}
void boundaryPauseAndDropCases() {
    auto design = known();
    design.damping_rate = 0.0f;
    const forces::Bounds bounds{100, 80};
    auto check = [&](forces::Agent edge,
                     const std::string& name,
                     bool horizontal,
                     bool positive) {
        expect(forces::integrateAgent(edge, 0.1f, design, bounds),
               name + " update succeeds");
        expect(edge.position.x >= 2 && edge.position.x <= 98 &&
                   edge.position.y >= 2 && edge.position.y <= 78,
               name + " remains radius-aware in bounds");
        const float velocity = horizontal ? edge.velocity.x : edge.velocity.y;
        expect(positive ? velocity > 0 : velocity < 0,
               name + " reflects inward");
    };
    check(agent({2.1f, 40}, {-50, 0}), "left", true, true);
    check(agent({97.9f, 40}, {50, 0}), "right", true, false);
    check(agent({50, 2.1f}, {0, -50}), "top", false, true);
    check(agent({50, 77.9f}, {0, 50}), "bottom", false, false);
    auto corner = agent({2.1f, 2.1f}, {-40, -40});
    expect(forces::integrateAgent(corner, 0.1f, design, bounds),
           "corner update succeeds");
    expectNear(corner.position.x, 2, "corner clamps left");
    expectNear(corner.position.y, 2, "corner clamps top");
    expect(corner.velocity.x > 0 && corner.velocity.y > 0,
           "corner reflects both outward components");
    auto exact = agent({2, 2}, {10, -10});
    expect(forces::boundsAreUsable({4, 4}, 2),
           "exact-diameter viewport is usable");
    expect(forces::integrateAgent(exact, 0.1f, design, {4, 4}),
           "exact-diameter update succeeds");
    expectNear(exact.position.x, 2, "exact-diameter x has one legal center");
    expectNear(exact.position.y, 2, "exact-diameter y has one legal center");

    forces::Vec2 clamped{0, 0};
    expect(forces::clampPointToBounds({-5, 100}, bounds, 13, clamped),
           "rendered control inset clamps in usable bounds");
    expectNear(clamped.x, 13, "control inset preserves left stroke extent");
    expectNear(clamped.y, 67, "control inset preserves bottom stroke extent");
    expect(forces::clampPointToBounds({0, 0}, {26, 26}, 13, clamped) &&
               near(clamped.x, 13) && near(clamped.y, 13),
           "exact control diameter has one legal center");
    expect(!forces::clampPointToBounds({0, 0}, {25, 26}, 13, clamped),
           "too-small control viewport rejects instead of clipping");

    design.maximum_catch_up_steps = 3;
    forces::System system;
    forces::reset(system, {50, 50}, forces::BehaviorMode::seek,
                  design, {100, 100});
    const auto capped = forces::advanceFrame(system, 1.0f, design, {100, 100});
    expect(capped.simulated_steps == 3, "catch-up work is capped");
    expectNear(capped.dropped_time, 0.97f,
               "clamped and capped time is reported", 0.001f);
    forces::setPaused(system, true);
    const auto paused = forces::advanceFrame(system, 100.0f, design, {100, 100});
    expect(paused.simulated_steps == 0 && near(system.accumulator, 0),
           "paused frame cannot accumulate resume spike");
    forces::setPaused(system, false);
    expect(forces::advanceFrame(system, 0.01f, design, {100, 100}).simulated_steps == 1,
           "resume consumes only new time");
}
void invalidAndTransactionalCases() {
    const auto design = known();
    forces::System system;
    forces::reset(system, {50, 50}, forces::BehaviorMode::seek,
                  design, {100, 100});
    const auto initial = system;
    const auto tiny = forces::advanceFrame(system, 0.02f, design, {3, 100});
    expect(tiny.simulated_steps == 0 && sameSystem(system, initial),
           "tiny resize rejects with the complete system unchanged");
    const auto nan = forces::advanceFrame(
        system, std::numeric_limits<float>::quiet_NaN(), design, {100, 100});
    expect(nan.simulated_steps == 0 && sameSystem(system, initial),
           "non-finite frame rejects with the complete system unchanged");

    system.dropped_time = std::numeric_limits<float>::max();
    const auto before_drop_overflow = system;
    const auto drop_overflow = forces::advanceFrame(
        system, std::numeric_limits<float>::max(), design, {100, 100});
    expect(drop_overflow.simulated_steps == 0 &&
               sameSystem(system, before_drop_overflow),
           "dropped-time overflow after simulated work rejects transactionally");

    forces::reset(system, {50, 50}, forces::BehaviorMode::seek,
                  design, {100, 100});
    system.simulated_steps = std::numeric_limits<std::size_t>::max() - 1;
    const auto before_step_wrap = system;
    const auto wrapped = forces::advanceFrame(system, 0.02f, design, {100, 100});
    expect(wrapped.simulated_steps == 0 && sameSystem(system, before_step_wrap),
           "simulated-step accounting refuses size_t wrap transactionally");
    const auto final_step = forces::advanceFrame(system, 0.01f, design, {100, 100});
    expect(final_step.simulated_steps == 1 &&
               system.simulated_steps == std::numeric_limits<std::size_t>::max(),
           "simulated-step accounting can reach size_t maximum exactly");

    auto extreme_design = design;
    extreme_design.damping_rate = 0.0f;
    auto extreme = agent({50, 50}, {0, 0});
    extreme.accumulated_force = {1, 1};
    const auto extreme_before = extreme;
    expect(!forces::integrateAgent(extreme, std::numeric_limits<float>::max(),
                                   extreme_design, {100, 100}),
           "overflowing position update is rejected");
    expectNear(extreme.position.x, extreme_before.position.x,
               "failed update preserves position");
    expectNear(extreme.accumulated_force.x, extreme_before.accumulated_force.x,
               "failed update preserves accumulated force");
}
void resetAndDesignHookCases() {
    const auto design = known();
    forces::System system;
    forces::reset(system, {150, 100}, forces::BehaviorMode::spring_chain,
                  design, {300, 200});
    expect(system.agents.size() == design.agent_count,
           "reset constructs bounded configured count");
    expect(forces::finiteState(system, design, {300, 200}),
           "reset state is finite and radius-aware in bounds");
    auto customization = makeForceDesign();
    expect(forces::designIsValid(customization),
           "starter customization hook returns a valid design");
    expect(customization.agent_count >= 4 &&
               customization.spring_rest_length >= 20.0f &&
               customization.seek_color.r != customization.spring_color.r,
           "starter design hook exposes viable count, spacing, and mode palette");
}
}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "usage supplies spring oracle");
    if (argc == 2) oracleCases(fixture(argv[1]));
    vectorAndLimitCases();
    seekCases();
    springAndRadialCases();
    extremeFiniteForceCases();
    accumulationAndCapsCases();
    modeAndCompositionCases();
    fixedStepPartitionCases();
    boundaryPauseAndDropCases();
    invalidAndTransactionalCases();
    resetAndDesignHookCases();
    if (failures != 0) {
        std::cerr << "force_model_test: " << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "force_model_test: spring oracle, seek/arrival/braking, 2D equilibrium/symmetry/damping, portable extreme vector/force arithmetic, softened radial direction/magnitude, accumulation saturation/reset, independent force/speed caps, modes, fixed-step partitions/accounting, four-edge/corner/exact bounds, control inset, pause/drop, full-system transaction, and design-hook readiness passed\n";
    return 0;
}
