#include "phase_field_design.h"
#include "phase_field_model.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
struct Fixture {
    std::string name;
    phasefield::Viewport viewport;
    float time;
    int row;
    int column;
    int index;
    phasefield::Vec2 base;
    float phase;
    phasefield::Vec2 center;
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
void expectVec(phasefield::Vec2 actual, phasefield::Vec2 expected,
               const std::string& label) {
    expectNear(actual.x, expected.x, label + ".x");
    expectNear(actual.y, expected.y, label + ".y");
}
void expectExtremaInBounds(phasefield::Vec2 center, float radius,
                           phasefield::Viewport viewport,
                           const std::string& label) {
    constexpr float stroke_half_width = 1.5f;
    constexpr float outer_margin = 2.0f;
    const float extent = radius + stroke_half_width + outer_margin;
    expect(center.x - extent >= -0.002f, label + " left extent");
    expect(center.y - extent >= -0.002f, label + " top extent");
    expect(center.x + extent <= static_cast<float>(viewport.width) + 0.002f,
           label + " right extent");
    expect(center.y + extent <= static_cast<float>(viewport.height) + 0.002f,
           label + " bottom extent");
}
phasefield::Design knownDesign() {
    return {3, 4, 8.0f, 0.5f, phasefield::pi / 2.0f,
            phasefield::pi / 2.0f, 4.0f,
            {20, 30, 40}, {200, 80, 60}, {240, 230, 210}};
}
std::vector<Fixture> readFixtures(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "phase fixture opens");
    std::vector<Fixture> fixtures;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream fields(line);
        Fixture fixture{};
        std::string extra;
        const bool parsed = static_cast<bool>(
            fields >> fixture.name >> fixture.viewport.width >> fixture.viewport.height >>
            fixture.time >> fixture.row >> fixture.column >> fixture.index >>
            fixture.base.x >> fixture.base.y >> fixture.phase >>
            fixture.center.x >> fixture.center.y);
        const bool exact = parsed && !(fields >> extra);
        expect(exact, "each fixture row has exactly twelve fields");
        if (exact) fixtures.push_back(fixture);
    }
    return fixtures;
}
void conversionCases() {
    using namespace phasefield;
    expectNear(degreesToRadians(0.0f), 0.0f, "zero degrees");
    expectNear(degreesToRadians(90.0f), pi / 2.0f, "ninety degrees is half pi");
    expectNear(degreesToRadians(180.0f), pi, "one hundred eighty degrees is pi");
    expectNear(degreesToRadians(360.0f), tau, "full turn is tau");
    expectNear(radiansToDegrees(pi / 2.0f), 90.0f, "half pi is ninety degrees");
    expectNear(radiansToDegrees(-pi), -180.0f, "negative pi conversion preserves direction");
    expectNear(degreesToRadians(radiansToDegrees(1.234f)), 1.234f,
               "conversion round trip");
}
void circularAndPolarCases() {
    using namespace phasefield;
    expectVec(polarToCartesian({10.0f, 0.0f}), {10.0f, 0.0f}, "cos/sin zero cardinal");
    expectVec(polarToCartesian({10.0f, pi / 2.0f}), {0.0f, 10.0f},
              "cos/sin quarter-turn cardinal");
    expectVec(polarToCartesian({10.0f, pi}), {-10.0f, 0.0f},
              "cos/sin half-turn cardinal");
    expectVec(polarToCartesian({10.0f, 3.0f * pi / 2.0f}), {0.0f, -10.0f},
              "cos/sin three-quarter cardinal");
    for (const auto value : {Vec2{3.0f, 4.0f}, {-3.0f, 4.0f}, {-3.0f, -4.0f},
                             {3.0f, -4.0f}}) {
        const Polar polar = cartesianToPolar(value);
        expectNear(polar.radius, 5.0f, "atan2 quadrant radius");
        expectVec(polarToCartesian(polar), value, "polar/cartesian quadrant round trip");
    }
    expectNear(cartesianToPolar({1.0f, 1.0f}).angle_radians, pi / 4.0f,
               "atan2 quadrant one");
    expectNear(cartesianToPolar({-1.0f, 1.0f}).angle_radians, 3.0f * pi / 4.0f,
               "atan2 quadrant two");
    expectNear(cartesianToPolar({-1.0f, -1.0f}).angle_radians, -3.0f * pi / 4.0f,
               "atan2 quadrant three signed angle");
    expectNear(cartesianToPolar({1.0f, -1.0f}).angle_radians, -pi / 4.0f,
               "atan2 quadrant four signed angle");
    const Polar zero = cartesianToPolar({0.0f, 0.0f});
    expectNear(zero.radius, 0.0f, "zero vector polar radius policy");
    expectNear(zero.angle_radians, 0.0f, "zero vector polar angle policy");
}
void waveCases() {
    using namespace phasefield;
    expectNear(oscillate(7.0f, 2.0f, 0.0f, 0.0f), 0.0f, "wave starts at zero");
    expectNear(oscillate(7.0f, 2.0f, 0.125f, 0.0f), 7.0f,
               "amplitude is wave maximum at quarter period");
    expectNear(oscillate(3.0f, 2.0f, 0.125f, 0.0f), 3.0f,
               "amplitude parameter changes maximum");
    expectNear(oscillate(7.0f, 1.0f, 0.25f, 0.0f),
               oscillate(7.0f, 2.0f, 0.125f, 0.0f),
               "frequency changes period");
    expectNear(oscillate(7.0f, 2.0f, 0.0f, pi / 2.0f), 7.0f,
               "phase shifts starting point");
    expectNear(oscillate(7.0f, 2.0f, 0.137f, 0.4f),
               oscillate(7.0f, 2.0f, 0.137f, 0.4f + tau),
               "phase plus two pi is periodic");
    expectNear(oscillate(7.0f, 2.0f, 0.137f, 0.4f),
               oscillate(7.0f, 2.0f, 0.137f + 0.5f, 0.4f),
               "time plus one frequency period is periodic");
}
void indexingCases() {
    using namespace phasefield;
    expect(flatIndex(0, 0, 4) == 0, "nested grid begins at zero");
    expect(flatIndex(1, 2, 4) == 6, "row-major index is row times columns plus column");
    expect(flatIndex(2, 3, 4) == 11, "last three-by-four index is eleven");
    expect(flatIndex(-1, 0, 4) == -1 && flatIndex(0, 4, 4) == -1,
           "invalid nested-loop coordinates return minus one");
    const auto scene = makeScene(knownDesign(), {400, 300}, 0.0f);
    expect(scene.marks.size() == 12, "three nested rows by four columns create twelve marks");
    for (std::size_t index = 0; index < scene.marks.size(); ++index) {
        expect(scene.marks[index].index == static_cast<int>(index),
               "nested row/column order matches flat index");
    }
}
void fixtureCases(const std::vector<Fixture>& fixtures) {
    for (const auto& fixture : fixtures) {
        const auto scene = phasefield::makeScene(knownDesign(), fixture.viewport, fixture.time);
        expect(scene.valid, fixture.name + " fixture scene is valid");
        expect(phasefield::sceneIsFiniteAndInBounds(scene, knownDesign(), fixture.viewport),
               fixture.name + " fixture scene is finite and stroke-aware bounded");
        const auto& mark = scene.marks[static_cast<std::size_t>(fixture.index)];
        expect(mark.row == fixture.row && mark.column == fixture.column,
               fixture.name + " row and column oracle");
        expectVec(mark.base, fixture.base, fixture.name + " base oracle");
        expectNear(mark.phase_radians, fixture.phase, fixture.name + " phase oracle");
        expectVec(mark.center, fixture.center, fixture.name + " center oracle");
    }
}
void periodicityAndDeterminismCases() {
    using namespace phasefield;
    const auto design = knownDesign();
    const auto first = makeScene(design, {800, 500}, 0.371f);
    const auto replay = makeScene(design, {800, 500}, 0.371f);
    const auto period_later = makeScene(design, {800, 500}, 0.371f + 1.0f / design.frequency_hz);
    for (std::size_t i = 0; i < first.marks.size(); ++i) {
        expectVec(first.marks[i].center, replay.marks[i].center,
                  "identical input deterministic replay");
        expectVec(first.marks[i].center, period_later.marks[i].center,
                  "scene repeats after one time period");
    }
}
void geometryExtremaCases() {
    using namespace phasefield;
    const Viewport viewport{64, 64};
    for (const auto parameters : {std::pair<float, float>{0.0f, 2.0f},
                                  {0.0f, 8.0f},
                                  {16.0f, 2.0f},
                                  {16.0f, 8.0f}}) {
        auto design = knownDesign();
        design.rows = parameters.first == 0.0f ? 2 : 16;
        design.columns = design.rows;
        design.amplitude = parameters.first;
        design.mark_radius = parameters.second;
        const auto scene = makeScene(design, viewport, 0.137f);
        expect(scene.valid, "min/max learner geometry makes a 64 by 64 scene");
        for (const auto& mark : scene.marks) {
            expectExtremaInBounds(mark.base, design.amplitude, viewport,
                                  "starter orbit extrema");
            expectExtremaInBounds(mark.base, std::min(3.0f, design.mark_radius),
                                  viewport, "starter crosshair extrema");
            expectExtremaInBounds(mark.center, design.mark_radius, viewport,
                                  "starter circle extrema");
            expectExtremaInBounds(mark.center, design.mark_radius, viewport,
                                  "solution axis-vertex diamond extrema");
        }
    }
}
void boundsInvalidAndVariationCases() {
    using namespace phasefield;
    for (const auto viewport : {Viewport{64, 64}, {64, 900}, {900, 64},
                                {400, 300}, {1600, 900}}) {
        const auto scene = makeScene(knownDesign(), viewport, 19.75f);
        expect(sceneIsFiniteAndInBounds(scene, knownDesign(), viewport),
               "tiny, narrow, wide scenes remain finite and stroke-aware bounded");
    }
    for (const auto viewport : {Viewport{0, 400}, {63, 400}, {400, 0},
                                {400, 63}, {-1, 400}}) {
        const auto scene = makeScene(knownDesign(), viewport, 0.0f);
        expect(!scene.valid && !sceneIsFiniteAndInBounds(scene, knownDesign(), viewport),
               "viewport smaller than 64 is explicitly invalid");
    }
    auto offscreen_base = makeScene(knownDesign(), {400, 300}, 0.0f);
    const Vec2 retained_center = offscreen_base.marks.front().center;
    offscreen_base.marks.front().base.x = 0.0f;
    expectVec(offscreen_base.marks.front().center, retained_center,
              "offscreen-base mutation keeps its center valid");
    expect(!sceneIsFiniteAndInBounds(offscreen_base, knownDesign(), {400, 300}),
           "offscreen base and orbit fail even when center remains valid");
    const float nan = std::numeric_limits<float>::quiet_NaN();
    expect(!makeScene(knownDesign(), {400, 300}, nan).valid,
           "non-finite time is explicitly invalid");
    expectVec(polarToCartesian({-1.0f, 0.0f}), {}, "negative polar radius policy is zero");
    expectNear(oscillate(-1.0f, 1.0f, 0.0f, 0.0f), 0.0f,
               "negative amplitude policy is zero");
    auto changed = knownDesign();
    changed.rows = 5;
    changed.columns = 2;
    changed.amplitude = 2.0f;
    changed.frequency_hz = 1.25f;
    changed.row_phase_step = pi / 5.0f;
    const auto variation = makeScene(changed, {400, 300}, 0.2f);
    expect(variation.marks.size() == 10, "learner row/column parameters vary count");
    expect(!near(variation.marks.front().center.x,
                 makeScene(knownDesign(), {400, 300}, 0.2f).marks.front().center.x),
           "learner geometry parameters vary the field");
    auto invalid = knownDesign();
    invalid.rows = 17;
    expect(!designIsValid(invalid), "out-of-contract learner design is invalid");
}
void learnerContract() {
    const auto design = makePhaseFieldDesign();
    expect(phasefield::designIsValid(design),
           "starter learner owns valid geometry, rhythm, and contrasting palette");
    const auto scene = phasefield::makeScene(design, {960, 640}, 1.25f);
    expect(phasefield::sceneIsFiniteAndInBounds(scene, design, {960, 640}),
           "starter learner design produces bounded inspectable geometry");
}
}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "fixture path is provided");
    const auto fixtures = argc == 2 ? readFixtures(argv[1]) : std::vector<Fixture>{};
    expect(fixtures.size() == 3, "fixture retains three parsed numerical scenarios");
    conversionCases();
    circularAndPolarCases();
    waveCases();
    indexingCases();
    fixtureCases(fixtures);
    periodicityAndDeterminismCases();
    geometryExtremaCases();
    boundsInvalidAndVariationCases();
    learnerContract();
    if (failures) { std::cerr << failures << " section 05 checks failed\n"; return 1; }
    std::cout << "phase_field_model_test: conversions, cardinals, polar/atan2, wave parameters, nested indexing, parsed oracles, periodicity, independent render extrema, bounds mutations, determinism, variation, and learner contract passed\n";
}
