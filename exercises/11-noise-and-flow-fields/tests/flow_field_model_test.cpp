#include "flow_field_design.h"
#include "flow_field_model.h"

#include <cmath>
#include <cstdint>
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

bool near(float first, float second, float tolerance = 0.0005f) {
    return std::isfinite(first) && std::isfinite(second) &&
           std::fabs(first - second) <= tolerance;
}

void expectNear(float actual,
                float expected,
                const std::string& message,
                float tolerance = 0.0005f) {
    if (!near(actual, expected, tolerance)) {
        ++failures;
        std::cerr << "FAIL: " << message << " actual=" << actual
                  << " expected=" << expected << '\n';
    }
}

flow::Design known() {
    return {0.01f, 0.25f, 8, 3, 3, 4, 5, 2.0f, 10.0f,
            0.25f, 0.5f, 1.0f,
            {245, 240, 224}, {30, 100, 120}, {210, 70, 60}};
}

struct OracleRow {
    std::string name;
    std::string kind;
    std::vector<std::string> fields;
};

std::vector<OracleRow> readOracle(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "flow oracle opens");
    std::vector<OracleRow> rows;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream stream(line);
        OracleRow row;
        expect(static_cast<bool>(stream >> row.name >> row.kind),
               "oracle row has name and kind");
        std::string field;
        while (stream >> field) row.fields.push_back(field);
        rows.push_back(row);
    }
    return rows;
}

void oracleCases(const std::vector<OracleRow>& rows) {
    expect(rows.size() == 5, "oracle has five independent rows");
    for (const auto& row : rows) {
        if (row.kind == "hash") {
            expect(row.fields.size() == 5, row.name + " has hash fields");
            if (row.fields.size() == 5) {
                const auto actual = flow::latticeHash(
                    static_cast<std::int32_t>(std::stol(row.fields[0])),
                    static_cast<std::int32_t>(std::stol(row.fields[1])),
                    static_cast<std::int32_t>(std::stol(row.fields[2])),
                    static_cast<std::uint32_t>(std::stoul(row.fields[3])));
                expect(actual == static_cast<std::uint32_t>(std::stoul(row.fields[4])),
                       row.name + " exact integer hash");
            }
        } else if (row.kind == "sample") {
            expect(row.fields.size() == 3, row.name + " has sample fields");
            if (row.fields.size() == 3) {
                flow::ScalarGrid grid{2, 2, {0.0f, 1.0f, 1.0f, 0.0f}};
                float value = -1.0f;
                expect(flow::sampleGrid(grid,
                                        std::stof(row.fields[0]),
                                        std::stof(row.fields[1]), value),
                       row.name + " samples");
                expectNear(value, std::stof(row.fields[2]), row.name);
            }
        } else if (row.kind == "angle") {
            expect(row.fields.size() == 3, row.name + " has angle fields");
            if (row.fields.size() == 3) {
                const auto direction =
                    flow::directionFromScalar(std::stof(row.fields[0]));
                expectNear(direction.x, std::stof(row.fields[1]), row.name + " x");
                expectNear(direction.y, std::stof(row.fields[2]), row.name + " y");
            }
        } else {
            expect(false, row.name + " has known oracle kind");
        }
    }
}

void indexingAndDimensionCases() {
    std::size_t index = 99;
    expect(flow::gridIndex(4, 3, 2, 1, index) && index == 6,
           "row-major index is row times columns plus column");
    expect(flow::gridIndex(4, 3, 3, 2, index) && index == 11,
           "last row-major cell is valid");
    expect(!flow::gridIndex(4, 3, 4, 1, index), "column equal width rejected");
    expect(!flow::gridIndex(4, 3, 1, 3, index), "row equal height rejected");
    expect(!flow::gridIndex(1, 3, 0, 0, index), "one-column grid rejected");
    expect(!flow::gridDimensionsValid(0, 0), "zero-cell grid rejected");
    expect(!flow::gridDimensionsValid(1, 1), "one-cell grid rejected");
    expect(flow::gridDimensionsValid(2, 2), "smallest bilinear grid accepted");
    expect(flow::gridDimensionsValid(256, 256), "maximum cell grid accepted");
    expect(!flow::gridDimensionsValid(257, 2), "column bound enforced");
    expect(!flow::gridDimensionsValid(256, 257), "row and cell bounds enforced");
    auto design = known();
    design.particle_count = 1024;
    design.maximum_history = 1024;
    expect(!flow::designIsValid(design), "particle-history product is bounded");
}

void interpolationCases() {
    flow::ScalarGrid grid{2, 2, {0.0f, 1.0f, 0.25f, 0.75f}};
    float value = -7.0f;
    expect(flow::sampleGrid(grid, 0.0f, 0.0f, value), "top-left samples");
    expectNear(value, 0.0f, "top-left exact corner");
    expect(flow::sampleGrid(grid, 1.0f, 0.0f, value), "top-right samples");
    expectNear(value, 1.0f, "top-right exact corner");
    expect(flow::sampleGrid(grid, 0.0f, 1.0f, value), "bottom-left samples");
    expectNear(value, 0.25f, "bottom-left exact corner");
    expect(flow::sampleGrid(grid, 1.0f, 1.0f, value), "bottom-right samples");
    expectNear(value, 0.75f, "bottom-right exact corner");
    expect(flow::sampleGrid(grid, 0.5f, 0.5f, value), "cell center samples");
    expectNear(value, 0.5f, "bilinear center known case");
    flow::ScalarGrid three{3, 3, {0, 0.1f, 0.2f, 0.3f, 0.4f,
                                  0.5f, 0.6f, 0.7f, 0.8f}};
    expect(flow::sampleGrid(three, 0.5f, 0.5f, value),
           "exact interior cell boundary samples");
    expectNear(value, 0.4f, "exact boundary returns center cell");
    expect(flow::sampleGrid(three, 1.0f, 0.5f, value),
           "viewport-equivalent right edge samples");
    expectNear(value, 0.5f, "right edge interpolates on final column");
    value = 12.0f;
    expect(!flow::sampleGrid(grid, -0.01f, 0.5f, value) && value == 12.0f,
           "out-of-range sample rejects without output mutation");
    expect(!flow::sampleGrid(grid,
                            std::numeric_limits<float>::quiet_NaN(), 0.5f, value),
           "non-finite sample coordinate rejected");
    const flow::ScalarGrid empty;
    expect(!flow::sampleGrid(empty, 0.5f, 0.5f, value),
           "zero-cell grid cannot be sampled");
}

void hashNoiseAndDirectionCases() {
    const auto hash = flow::latticeHash(3, 5, 7, 42);
    expectNear(flow::hashToUnit(hash), 0.90151518f,
               "hash maps deterministically to unit interval", 0.000001f);
    expectNear(flow::smoothInterpolation(0.0f), 0.0f, "smoothstep starts at zero");
    expectNear(flow::smoothInterpolation(0.5f), 0.5f, "smoothstep midpoint");
    expectNear(flow::smoothInterpolation(1.0f), 1.0f, "smoothstep ends at one");
    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
    expect(flow::valueNoise(1.25f, 2.5f, 0.75f, 99, a) &&
               flow::valueNoise(1.25f, 2.5f, 0.75f, 99, b) &&
               flow::valueNoise(1.25f, 2.5f, 0.75f, 100, c),
           "value noise accepts finite coordinates");
    expectNear(a, b, "same seed and coordinates replay");
    expect(!near(a, c, 0.000001f), "different seed varies deterministic noise");
    float close_time = 0.0f;
    expect(flow::valueNoise(1.25f, 2.5f, 0.76f, 99, close_time),
           "nearby temporal sample succeeds");
    expect(!near(a, close_time, 0.0000001f) && std::fabs(a - close_time) < 0.05f,
           "nearby time changes coherently rather than jumping");
    float integer = 0.0f;
    expect(flow::valueNoise(3.0f, 5.0f, 7.0f, 42, integer),
           "integer lattice sample succeeds");
    expectNear(integer, flow::hashToUnit(hash),
               "integer lattice sample equals its hash value", 0.000001f);
    expect(!flow::valueNoise(std::numeric_limits<float>::max(), 0, 0, 1, integer),
           "unrepresentable lattice coordinate rejected");
    const auto diagonal = flow::directionFromScalar(0.125f);
    expectNear(diagonal.x, 0.70710678f, "non-axis direction x", 0.00001f);
    expectNear(diagonal.y, 0.70710678f, "non-axis direction y", 0.00001f);
    expectNear(std::hypot(diagonal.x, diagonal.y), 1.0f,
               "scalar-to-angle direction is unit length", 0.00001f);
    const auto wrapped = flow::directionFromScalar(1.0f);
    expectNear(wrapped.x, 1.0f, "scalar one closes angle circle");
    expectNear(wrapped.y, 0.0f, "scalar one has zero y", 0.00001f);
}

void fieldGenerationAndSamplingCases() {
    const auto design = known();
    flow::ScalarGrid first;
    flow::ScalarGrid replay;
    flow::ScalarGrid varied;
    expect(flow::generateField(design, {0.3f, 0.7f}, 1.2f, 17, first) &&
               flow::generateField(design, {0.3f, 0.7f}, 1.2f, 17, replay) &&
               flow::generateField(design, {0.3f, 0.7f}, 1.2f, 18, varied),
           "field generation succeeds");
    expect(first.values == replay.values, "same field seed replays exact stored samples");
    bool differs = false;
    for (std::size_t i = 0; i < first.values.size(); ++i) {
        expect(std::isfinite(first.values[i]) && first.values[i] >= 0.0f &&
                   first.values[i] <= 1.0f,
               "generated scalar stays finite in unit interval");
        const auto direction = flow::directionFromScalar(first.values[i]);
        expect(std::isfinite(direction.x) && std::isfinite(direction.y) &&
                   near(std::hypot(direction.x, direction.y), 1.0f, 0.00001f),
               "every generated scalar maps to a finite unit vector");
        differs = differs || !near(first.values[i], varied.values[i], 0.000001f);
    }
    expect(differs, "different seed varies generated field");
    flow::ScalarGrid diagonal{2, 2, {0.125f, 0.125f, 0.125f, 0.125f}};
    flow::Vec2 direction{0, 0};
    expect(flow::sampleDirection(diagonal, {25, 75}, {100, 100}, direction),
           "world position samples field");
    expectNear(direction.x, 0.70710678f, "world sample keeps non-axis x", 0.00001f);
    expectNear(direction.y, 0.70710678f, "world sample keeps non-axis y", 0.00001f);
    expect(flow::sampleDirection(diagonal, {100, 100}, {100, 100}, direction),
           "exact viewport corner samples final cell");
    expect(!flow::sampleDirection(diagonal, {101, 50}, {100, 100}, direction),
           "position beyond viewport rejected");
}

void particleAdvectionAndBoundaryCases() {
    auto design = known();
    design.advection_speed = 10.0f;
    flow::ScalarGrid diagonal{2, 2, {0.125f, 0.125f, 0.125f, 0.125f}};
    flow::Particle particle{{50, 50}, {{50, 50}}};
    expect(flow::advanceParticle(particle, diagonal, 0.1f, design,
                                 {100, 100}, false),
           "particle advection succeeds");
    expectNear(particle.position.x, 50.7071068f, "advection follows field x", 0.0001f);
    expectNear(particle.position.y, 50.7071068f, "advection follows field y", 0.0001f);
    flow::ScalarGrid right{2, 2, {0, 0, 0, 0}};
    flow::Particle wrapping{{97.5f, 50}, {{95.5f, 50}, {96.5f, 50}, {97.5f, 50}}};
    expect(flow::advanceParticle(wrapping, right, 1.0f, design,
                                 {100, 100}, false),
           "right wrapping update succeeds");
    expectNear(wrapping.position.x, 11.5f,
               "right crossing wraps through radius-aware center interval", 0.0001f);
    expect(wrapping.history.size() == 1 &&
               near(wrapping.history.front().x, wrapping.position.x) &&
               near(wrapping.history.front().y, wrapping.position.y),
           "wrapping resets history so no cross-boundary segment is stored");
    flow::ScalarGrid left{2, 2, {0.5f, 0.5f, 0.5f, 0.5f}};
    flow::Particle left_wrap{{2.5f, 50}, {{2.5f, 50}}};
    expect(flow::advanceParticle(left_wrap, left, 1.0f, design,
                                 {100, 100}, false),
           "left wrapping update succeeds");
    expectNear(left_wrap.position.x, 88.5f, "left crossing wraps to right", 0.0001f);
    expect(left_wrap.history.size() == 1 &&
               near(left_wrap.history.front().x, left_wrap.position.x),
           "left wrap breaks discontinuous history");
    flow::ScalarGrid down{2, 2, {0.25f, 0.25f, 0.25f, 0.25f}};
    flow::Particle bottom_wrap{{50, 97.5f}, {{50, 97.5f}}};
    expect(flow::advanceParticle(bottom_wrap, down, 1.0f, design,
                                 {100, 100}, false),
           "bottom wrapping update succeeds");
    expectNear(bottom_wrap.position.y, 11.5f, "bottom crossing wraps to top", 0.0001f);
    expect(bottom_wrap.history.size() == 1 &&
               near(bottom_wrap.history.front().y, bottom_wrap.position.y),
           "bottom wrap breaks vertical discontinuous history");
    flow::ScalarGrid up{2, 2, {0.75f, 0.75f, 0.75f, 0.75f}};
    flow::Particle top_wrap{{50, 2.5f}, {{50, 2.5f}}};
    expect(flow::advanceParticle(top_wrap, up, 1.0f, design,
                                 {100, 100}, false),
           "top wrapping update succeeds");
    expectNear(top_wrap.position.y, 88.5f, "top crossing wraps to bottom", 0.0001f);
    expect(top_wrap.history.size() == 1 &&
               near(top_wrap.history.front().y, top_wrap.position.y),
           "top wrap breaks vertical discontinuous history");
    for (int i = 0; i < 12; ++i) {
        expect(flow::advanceParticle(particle, diagonal, 0.1f, design,
                                     {100, 100}, false),
               "history update succeeds");
    }
    expect(particle.history.size() == design.maximum_history,
           "particle history is capped");
    expect(flow::advanceParticle(particle, diagonal, 0.1f, design,
                                 {100, 100}, true),
           "reduced-motion update succeeds");
    expect(particle.history.size() == 1 &&
               near(particle.history.front().x, particle.position.x),
           "reduced motion keeps only current position");
    flow::Particle exact{{2, 2}, {{2, 2}}};
    expect(flow::advanceParticle(exact, right, 0.01f, design, {4, 4}, false),
           "exact-diameter viewport has one legal wrapped point");
    expectNear(exact.position.x, 2, "exact bounds keep x fixed");
    expectNear(exact.position.y, 2, "exact bounds keep y fixed");
}

bool sameSystem(const flow::System& first, const flow::System& second) {
    if (first.field.columns != second.field.columns ||
        first.field.rows != second.field.rows ||
        first.field.values.size() != second.field.values.size() ||
        first.particles.size() != second.particles.size() ||
        !near(first.field_offset.x, second.field_offset.x, 0.000001f) ||
        !near(first.field_offset.y, second.field_offset.y, 0.000001f) ||
        !near(first.field_time, second.field_time, 0.000001f) ||
        !near(first.accumulator, second.accumulator, 0.000001f) ||
        !near(first.dropped_time, second.dropped_time, 0.000001f) ||
        first.seed != second.seed ||
        first.simulated_steps != second.simulated_steps ||
        first.paused != second.paused ||
        first.reduced_motion != second.reduced_motion ||
        first.time_frozen != second.time_frozen) return false;
    for (std::size_t i = 0; i < first.field.values.size(); ++i) {
        if (!near(first.field.values[i], second.field.values[i], 0.000001f)) return false;
    }
    for (std::size_t i = 0; i < first.particles.size(); ++i) {
        const auto& a = first.particles[i];
        const auto& b = second.particles[i];
        if (!near(a.position.x, b.position.x, 0.00001f) ||
            !near(a.position.y, b.position.y, 0.00001f) ||
            a.history.size() != b.history.size()) return false;
        for (std::size_t j = 0; j < a.history.size(); ++j) {
            if (!near(a.history[j].x, b.history[j].x, 0.00001f) ||
                !near(a.history[j].y, b.history[j].y, 0.00001f)) return false;
        }
    }
    return true;
}

void systemReplayPartitionAndTimeCases() {
    const auto design = known();
    const flow::Bounds bounds{200, 120};
    flow::System once;
    flow::System split;
    flow::System varied;
    flow::reset(once, bounds, design, 123, {0.2f, 0.4f});
    split = once;
    flow::reset(varied, bounds, design, 124, {0.2f, 0.4f});
    const auto initial_field = once.field.values;
    const auto one = flow::advanceFrame(once, 0.08f, design, bounds);
    const auto first = flow::advanceFrame(split, 0.03f, design, bounds);
    const auto second = flow::advanceFrame(split, 0.05f, design, bounds);
    expect(one.simulated_steps == 8 &&
               first.simulated_steps + second.simulated_steps == 8,
           "fixed-step count is partition independent");
    expect(sameSystem(once, split), "fixed-step state is partition independent");
    expect(initial_field != once.field.values,
           "advancing field time changes coherent grid values");
    flow::advanceFrame(varied, 0.08f, design, bounds);
    expect(once.particles.size() == varied.particles.size(),
           "seed replay uses configured particle count");
    bool seed_varies = false;
    for (std::size_t i = 0; i < once.particles.size(); ++i) {
        seed_varies = seed_varies ||
            !near(once.particles[i].position.x, varied.particles[i].position.x,
                  0.000001f) ||
            !near(once.particles[i].position.y, varied.particles[i].position.y,
                  0.000001f);
    }
    expect(seed_varies, "different seed varies deterministic particle placement");
    flow::System replay;
    flow::reset(replay, bounds, design, 123, {0.2f, 0.4f});
    flow::advanceFrame(replay, 0.08f, design, bounds);
    expect(sameSystem(once, replay), "reset seed replays complete advection state");

    flow::System zero_seed;
    flow::System former_sentinel_seed;
    flow::reset(zero_seed, bounds, design, 0u);
    flow::reset(former_sentinel_seed, bounds, design, 0x6d2b79f5u);
    expect(zero_seed.seed == 0u && former_sentinel_seed.seed == 0x6d2b79f5u,
           "seed zero and the former sentinel remain distinct stored seeds");
    expect(zero_seed.field.values != former_sentinel_seed.field.values,
           "seed zero and the former sentinel generate distinct fields");
}

void stateControlDropAndOverflowCases() {
    auto design = known();
    const flow::Bounds bounds{200, 120};
    flow::System system;
    flow::reset(system, bounds, design, 77);
    expect(flow::finiteState(system, design, bounds), "reset state is finite");
    expect(flow::setFieldOffset(system, {1.5f, -2.5f}),
           "finite pointer/keyboard field offset accepted");
    expectNear(system.field_offset.x, 1.5f, "field offset x stored");
    expect(!flow::setFieldOffset(system,
                                {std::numeric_limits<float>::quiet_NaN(), 0}),
           "non-finite field offset rejected");
    flow::setTimeFrozen(system, true);
    const float frozen_time = system.field_time;
    flow::advanceFrame(system, 0.02f, design, bounds);
    expectNear(system.field_time, frozen_time, "frozen field time stays fixed");
    flow::setTimeFrozen(system, false);
    flow::setReducedMotion(system, true);
    expect(system.reduced_motion, "reduced-motion state is explicit");
    for (const auto& particle : system.particles) {
        expect(particle.history.size() == 1,
               "enabling reduced motion clears old trails");
    }
    flow::setPaused(system, true);
    const auto paused = flow::advanceFrame(system, 100.0f, design, bounds);
    expect(paused.simulated_steps == 0 && near(system.accumulator, 0),
           "pause cannot accumulate resume spike");
    flow::setPaused(system, false);
    expect(flow::advanceFrame(system, 0.01f, design, bounds).simulated_steps == 1,
           "resume consumes only new time");
    flow::reset(system, bounds, design, 77);
    expect(!system.paused && !system.reduced_motion && !system.time_frozen &&
               near(system.field_time, 0) && near(system.dropped_time, 0) &&
               system.simulated_steps == 0,
           "reset clears control flags, clocks, and counters");

    design.maximum_catch_up_steps = 3;
    flow::System capped;
    flow::reset(capped, bounds, design, 5);
    const auto result = flow::advanceFrame(capped, 1.0f, design, bounds);
    expect(result.simulated_steps == 3, "catch-up work is capped");
    expectNear(result.dropped_time, 0.97f,
               "frame clamp and capped whole steps are reported", 0.001f);
    expectNear(capped.dropped_time, 0.97f,
               "system accumulates dropped-time evidence", 0.001f);

    flow::System counter;
    flow::reset(counter, bounds, design, 8);
    counter.simulated_steps = std::numeric_limits<std::size_t>::max();
    const auto counter_before = counter;
    expect(flow::advanceFrame(counter, 0.01f, design, bounds).simulated_steps == 0,
           "step-counter overflow rejects frame");
    expect(sameSystem(counter, counter_before),
           "step-counter overflow is transactional");
}

void invalidTransactionalAndDesignCases() {
    const auto design = known();
    const flow::Bounds bounds{200, 120};
    flow::System system;
    flow::reset(system, bounds, design, 44);
    system.accumulator = 0.005f;
    const auto before = system;
    expect(flow::advanceFrame(system,
                              std::numeric_limits<float>::quiet_NaN(),
                              design, bounds).simulated_steps == 0,
           "NaN frame rejected");
    expect(sameSystem(system, before), "NaN frame rejection preserves full system");
    expect(flow::advanceFrame(system, 0.02f, design, {3, 100}).simulated_steps == 0,
           "tiny bounds reject frame");
    expect(sameSystem(system, before), "tiny-bounds rejection preserves full system");
    flow::System time_overflow = before;
    time_overflow.field_time = 1000000.0f;
    auto overflow_design = design;
    overflow_design.field_time_rate = 1000.0f;
    const auto time_before = time_overflow;
    expect(flow::advanceFrame(time_overflow, 0.01f, overflow_design, bounds).simulated_steps == 0,
           "field coordinate beyond declared range rejects frame");
    expect(sameSystem(time_overflow, time_before),
           "field-time rejection is transactional");
    flow::System dropped = before;
    dropped.dropped_time = std::numeric_limits<float>::max();
    auto one_step_design = design;
    one_step_design.maximum_catch_up_steps = 1;
    dropped.accumulator = 0.0f;
    const auto dropped_before = dropped;
    expect(flow::advanceFrame(dropped, 0.02f, one_step_design, bounds).simulated_steps == 0,
           "small positive drop that cannot increase FLT_MAX rejects frame");
    expect(sameSystem(dropped, dropped_before),
           "unrepresentable positive drop preserves the full system");
    auto learner = makeFlowFieldDesign();
    expect(flow::designIsValid(learner),
           "learner design hook accepts every technically valid aesthetic choice");
}

}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "usage supplies flow oracle");
    if (argc == 2) oracleCases(readOracle(argv[1]));
    indexingAndDimensionCases();
    interpolationCases();
    hashNoiseAndDirectionCases();
    fieldGenerationAndSamplingCases();
    particleAdvectionAndBoundaryCases();
    systemReplayPartitionAndTimeCases();
    stateControlDropAndOverflowCases();
    invalidTransactionalAndDesignCases();
    if (failures != 0) {
        std::cerr << "flow_field_model_test: " << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "flow_field_model_test: independent hash/sample/angle oracle, checked row-major grid, smooth deterministic value noise, bilinear corners/boundaries/edges, 2D unit directions, temporal/seed coherence, bounded wrapped advection/history, fixed-step replay/partition, pause/reduced/frozen controls, dropped-time/counter bounds, transactional extremes, and design-hook validity passed\n";
    return 0;
}
