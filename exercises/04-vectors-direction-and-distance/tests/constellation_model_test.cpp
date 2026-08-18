#include "constellation_design.h"
#include "constellation_model.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
struct Case {
    std::string name;
    constellation::Viewport viewport;
    constellation::Vec2 requested;
    constellation::Vec2 anchor;
    constellation::Vec2 target;
    constellation::Vec2 direction;
    float distance;
    constellation::Vec2 unit;
};
int failures = 0;
void expect(bool condition, const std::string& message) {
    if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
bool near(float actual, float expected, float abs_tolerance = 0.002f,
          float rel_tolerance = 0.000001f) {
    const float difference = std::fabs(actual - expected);
    const float allowed = std::max(abs_tolerance, rel_tolerance * std::fabs(expected));
    return std::isfinite(actual) && std::isfinite(expected) && difference <= allowed;
}
void expectNear(float actual, float expected, const std::string& label) {
    if (!near(actual, expected)) {
        ++failures;
        std::cerr << "FAIL: " << label << " actual=" << actual << " expected=" << expected
                  << " difference=" << std::fabs(actual - expected)
                  << " absTolerance=0.002 relTolerance=0.000001\n";
    }
}
void expectVec(constellation::Vec2 actual, constellation::Vec2 expected,
               const std::string& label) {
    expectNear(actual.x, expected.x, label + ".x");
    expectNear(actual.y, expected.y, label + ".y");
}
constellation::Design knownDesign() {
    return {0.25f, 0.35f, 80.0f, {20, 30, 40}, {210, 80, 90}, {240, 230, 210}};
}
std::vector<Case> readCases(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "scene fixture opens");
    std::vector<Case> cases;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream fields(line);
        Case fixture{};
        std::string extra;
        const bool parsed = static_cast<bool>(
            fields >> fixture.name >> fixture.viewport.width >> fixture.viewport.height >>
            fixture.requested.x >> fixture.requested.y >> fixture.anchor.x >> fixture.anchor.y >>
            fixture.target.x >> fixture.target.y >> fixture.direction.x >> fixture.direction.y >>
            fixture.distance >> fixture.unit.x >> fixture.unit.y);
        const bool exact = parsed && !(fields >> extra);
        expect(exact, "each fixture row has exactly fourteen fields");
        if (exact) cases.push_back(fixture);
    }
    return cases;
}
void vectorCases() {
    using namespace constellation;
    expectVec(subtract({8.0f, 9.0f}, {2.0f, 5.0f}), {6.0f, 4.0f},
              "subtraction points from second point to first point");
    expectVec(add({2.0f, 3.0f}, {4.0f, -1.0f}), {6.0f, 2.0f}, "component addition");
    expectVec(scale({3.0f, 4.0f}, 2.5f), {7.5f, 10.0f}, "scaling changes magnitude");
    expectNear(dot({3.0f, 4.0f}, {3.0f, 4.0f}), 25.0f,
               "self dot product is squared length");
    expectNear(length({3.0f, 4.0f}), 5.0f, "3-4-5 vector length");
    expectNear(distance({2.0f, 5.0f}, {8.0f, 9.0f}), std::sqrt(52.0f),
               "distance ignores direction sign");
    expectVec(normalizeOrZero({3.0f, 4.0f}), {0.6f, 0.8f}, "normalization keeps direction");
    expectNear(length(normalizeOrZero({-3.0f, 4.0f})), 1.0f,
               "nonzero normalized vector has length one");
    expectVec(normalizeOrZero({0.0f, 0.0f}), {0.0f, 0.0f},
              "zero-length normalization returns zero vector");
    expectVec(moveToward({0.0f, 0.0f}, {30.0f, 40.0f}, 10.0f), {6.0f, 8.0f},
              "speed scales a unit direction by maximum step");
    expectVec(moveToward({0.0f, 0.0f}, {3.0f, 4.0f}, 20.0f), {3.0f, 4.0f},
              "movement does not overshoot a nearby target");
    expectVec(moveToward({2.0f, 3.0f}, {2.0f, 3.0f}, 10.0f), {2.0f, 3.0f},
              "movement zero guard remains at target");
    const float infinity = std::numeric_limits<float>::infinity();
    expectNear(length({infinity, 1.0f}), 0.0f, "non-finite length policy is deterministic");
    expectVec(normalizeOrZero({infinity, 1.0f}), {},
              "non-finite normalization policy is deterministic");
    expectVec(moveToward({infinity, 1.0f}, {2.0f, 3.0f}, 4.0f), {},
              "non-finite movement origin produces deterministic zero");
    expectVec(moveToward({2.0f, 3.0f}, {infinity, 1.0f}, 4.0f), {2.0f, 3.0f},
              "non-finite movement target preserves finite origin");
}
void fixtureCases(const std::vector<Case>& cases) {
    for (const auto& expected : cases) {
        const auto actual = constellation::makeScene(knownDesign(), expected.viewport, expected.requested);
        expect(actual.valid, expected.name + " fixture is valid");
        expect(constellation::sceneIsFiniteAndInBounds(actual, expected.viewport),
               expected.name + " fixture is finite and stroke-aware bounded");
        expectVec(actual.anchor, expected.anchor, expected.name + " anchor oracle");
        expectVec(actual.target, expected.target, expected.name + " target oracle");
        expectVec(actual.direction, expected.direction, expected.name + " direction oracle");
        expectNear(actual.distance, expected.distance, expected.name + " distance oracle");
        expectVec(actual.unit_direction, expected.unit, expected.name + " unit oracle");
    }
}
void boundaryAndDeterminismCases() {
    for (const auto viewport : {constellation::Viewport{64, 64}, {64, 900}, {900, 64},
                                {400, 300}, {1600, 900}}) {
        for (const auto target : {constellation::Vec2{-100.0f, -100.0f},
                                  {10000.0f, 10000.0f}, {32.0f, 32.0f}}) {
            const auto scene = constellation::makeScene(knownDesign(), viewport, target);
            expect(constellation::sceneIsFiniteAndInBounds(scene, viewport),
                   "tiny/extreme viewport target remains stroke-aware in bounds");
        }
    }
    const auto pointer_at_right_edge =
        constellation::makeScene(knownDesign(), {800, 600}, {10000.0f, 200.0f});
    const auto one_key_step_inward = constellation::makeScene(
        knownDesign(), {800, 600},
        constellation::add(pointer_at_right_edge.target, {-12.0f, 0.0f}));
    expectVec(pointer_at_right_edge.target, {788.0f, 200.0f},
              "out-of-bounds pointer state synchronizes to the clamped right edge");
    expectVec(one_key_step_inward.target, {776.0f, 200.0f},
              "left key transition moves exactly twelve pixels inward from the clamped edge");

    const auto first = constellation::makeScene(knownDesign(), {800, 600}, {713.0f, 211.0f});
    const auto second = constellation::makeScene(knownDesign(), {800, 600}, {713.0f, 211.0f});
    expectVec(first.anchor, second.anchor, "deterministic anchor replay");
    expectVec(first.traveler, second.traveler, "deterministic traveler replay");
    expectNear(first.distance, second.distance, "deterministic distance replay");

    auto changed = knownDesign();
    changed.anchor_x = 0.75f;
    changed.anchor_y = 0.65f;
    changed.reach = 30.0f;
    const auto variation = constellation::makeScene(changed, {800, 600}, {713.0f, 211.0f});
    expect(!near(first.anchor.x, variation.anchor.x) && !near(first.anchor.y, variation.anchor.y),
           "learner anchor parameters vary geometry");
    expectNear(constellation::distance(variation.anchor, variation.traveler), 30.0f,
               "learner reach parameter varies bounded travel distance");
}
void invalidCases() {
    for (const auto viewport : {constellation::Viewport{0, 400}, {63, 400}, {400, 0},
                                {400, 63}, {-1, 400}}) {
        const auto scene = constellation::makeScene(knownDesign(), viewport, {20.0f, 20.0f});
        expect(!scene.valid, "smaller-than-64 viewport is explicitly invalid");
        expect(!constellation::sceneIsFiniteAndInBounds(scene, viewport),
               "invalid viewport is not reported in bounds");
    }
    const float nan = std::numeric_limits<float>::quiet_NaN();
    expect(!constellation::makeScene(knownDesign(), {400, 300}, {nan, 20.0f}).valid,
           "non-finite requested target is explicitly invalid");
    auto invalid_design = knownDesign();
    invalid_design.reach = 201.0f;
    expect(!constellation::makeScene(invalid_design, {400, 300}, {20.0f, 20.0f}).valid,
           "out-of-contract learner design is invalid");
}
void learnerContract() {
    const auto design = makeConstellationDesign();
    expect(constellation::designIsValid(design),
           "starter learner owns valid anchor, reach, and contrasting palette");
    const auto scene = constellation::makeScene(design, {960, 640}, {900.0f, 40.0f});
    expect(constellation::sceneIsFiniteAndInBounds(scene, {960, 640}),
           "starter learner design produces bounded inspectable geometry");
}
}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "fixture path is provided");
    const auto cases = argc == 2 ? readCases(argv[1]) : std::vector<Case>{};
    expect(cases.size() == 3, "fixture retains three parsed numerical scenarios");
    vectorCases();
    fixtureCases(cases);
    boundaryAndDeterminismCases();
    invalidCases();
    learnerContract();
    if (failures) { std::cerr << failures << " section 04 checks failed\n"; return 1; }
    std::cout << "constellation_model_test: components, subtraction, direction, distance, normalization and zero guard, scaling, parsed oracles, boundaries, determinism, parameter variation, and learner contract passed\n";
}
