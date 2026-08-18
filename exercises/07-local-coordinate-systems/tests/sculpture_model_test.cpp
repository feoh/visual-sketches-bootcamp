#include "sculpture_design.h"
#include "sculpture_model.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
struct Fixture {
    std::string name;
    sculpture::Viewport viewport;
    float time;
    float scale;
    float root_angle;
    float child_angle;
    sculpture::Vec2 pivot;
    sculpture::Vec2 elbow;
    sculpture::Vec2 tip;
};
int failures = 0;
void expect(bool condition, const std::string& message) {
    if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
bool near(float actual, float expected, float tolerance = 0.002f) {
    return std::isfinite(actual) && std::isfinite(expected) &&
           std::fabs(actual - expected) <= tolerance;
}
void expectNear(float actual, float expected, const std::string& label) {
    if (!near(actual, expected)) {
        ++failures;
        std::cerr << "FAIL: " << label << " actual=" << actual << " expected=" << expected
                  << " difference=" << std::fabs(actual - expected) << " tolerance=0.002\n";
    }
}
void expectVec(sculpture::Vec2 actual, sculpture::Vec2 expected, const std::string& label) {
    expectNear(actual.x, expected.x, label + ".x");
    expectNear(actual.y, expected.y, label + ".y");
}
sculpture::Design knownDesign() {
    return {4.0f, 40.0f, 28.0f, 6.0f, 3.0f, 30.0f, 60.0f,
            {30, 43, 58}, {218, 93, 73}, {241, 235, 219}};
}
std::vector<Fixture> readFixtures(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "transformed-anchor fixture opens");
    std::vector<Fixture> fixtures;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        Fixture fixture{};
        std::istringstream fields(line);
        std::string extra;
        const bool parsed = static_cast<bool>(fields >> fixture.name >> fixture.viewport.width >>
            fixture.viewport.height >> fixture.time >> fixture.scale >> fixture.root_angle >>
            fixture.child_angle >> fixture.pivot.x >> fixture.pivot.y >> fixture.elbow.x >>
            fixture.elbow.y >> fixture.tip.x >> fixture.tip.y);
        const bool exact = parsed && !(fields >> extra);
        expect(exact, "each fixture row has exactly thirteen fields");
        if (exact) fixtures.push_back(fixture);
    }
    return fixtures;
}
void primitiveTransformCases() {
    using namespace sculpture;
    expectVec(transformPoint(identity(), {3.0f, -2.0f}), {3.0f, -2.0f}, "identity");
    expectVec(transformPoint(translation(7.0f, 4.0f), {3.0f, -2.0f}), {10.0f, 2.0f},
              "translation");
    expectVec(transformPoint(rotationDegrees(90.0f), {2.0f, 0.0f}), {0.0f, 2.0f},
              "positive angle moves right vector down on positive-down screen");
    expectVec(transformPoint(scaling(2.0f, 3.0f), {4.0f, -2.0f}), {8.0f, -6.0f},
              "nonuniform scaling");
    expectVec(transformPoint(scaling(0.0f, 2.0f), {4.0f, 3.0f}), {0.0f, 6.0f},
              "zero scale deliberately collapses one axis");
    expectVec(transformPoint(scaling(-2.0f, 3.0f), {4.0f, -2.0f}), {-8.0f, -6.0f},
              "negative matrix scale deliberately mirrors one axis");
    const float nan = std::numeric_limits<float>::quiet_NaN();
    expectVec(transformPoint(scaling(nan, 2.0f), {4.0f, 3.0f}), {4.0f, 3.0f},
              "non-finite scaling policy is identity");
}
void orderCases() {
    using namespace sculpture;
    const Vec2 point{1.0f, 0.0f};
    const Mat3 translate_then_rotate = multiply(translation(10.0f, 0.0f),
                                                 rotationDegrees(90.0f));
    const Mat3 rotate_then_translate = multiply(rotationDegrees(90.0f),
                                                 translation(10.0f, 0.0f));
    expectVec(transformPoint(translate_then_rotate, point), {10.0f, 1.0f},
              "rightmost rotation applies before left translation");
    expectVec(transformPoint(rotate_then_translate, point), {0.0f, 11.0f},
              "rightmost translation applies before left rotation");
    const Vec2 first = transformPoint(translate_then_rotate, point);
    const Vec2 second = transformPoint(rotate_then_translate, point);
    expect(!near(first.x, second.x) || !near(first.y, second.y),
           "transform order is noncommutative");
}
void fixtureCases(const std::vector<Fixture>& fixtures) {
    expect(fixtures.size() == 3, "fixture contains exactly three frames");
    const auto design = knownDesign();
    for (const auto& fixture : fixtures) {
        const auto scene = sculpture::makeScene(design, fixture.viewport, fixture.time);
        expect(scene.valid, fixture.name + " scene valid");
        expectNear(scene.model_scale, fixture.scale, fixture.name + " scale");
        expectNear(scene.root_angle_degrees, fixture.root_angle, fixture.name + " root angle");
        expectNear(scene.child_angle_degrees, fixture.child_angle, fixture.name + " child angle");
        expectVec(scene.pivot, fixture.pivot, fixture.name + " pivot");
        expectVec(scene.elbow, fixture.elbow, fixture.name + " elbow");
        expectVec(scene.tip, fixture.tip, fixture.name + " child-chain tip");
    }
}
void timeCases() {
    using namespace sculpture;
    const auto design = knownDesign();
    expectNear(wrapTime(5.0f, 4.0f), 1.0f, "positive time wraps");
    expectNear(wrapTime(-1.0f, 4.0f), 3.0f, "negative time wraps");
    expectNear(wrapTime(std::numeric_limits<float>::infinity(), 4.0f), 0.0f,
               "invalid time policy is zero");
    const auto a = makeScene(design, {320, 240}, 0.75f);
    const auto b = makeScene(design, {320, 240}, 0.75f);
    const auto periodic = makeScene(design, {320, 240}, 4.75f);
    expectVec(a.tip, b.tip, "same input is deterministic");
    expectVec(a.tip, periodic.tip, "time plus period gives same child anchor");
}
void boundsCases() {
    using namespace sculpture;
    const auto design = knownDesign();
    for (const auto viewport : {Viewport{48, 48}, {64, 240}, {420, 72}, {800, 500}}) {
        for (const float time : {0.0f, 0.37f, 1.0f, 2.0f, 3.41f}) {
            const auto scene = makeScene(design, viewport, time);
            expect(scene.valid, "stroke-aware scene valid in legal viewport");
            expect(sceneIsFiniteAndInBounds(scene, design, viewport),
                   "production bounds predicate accepts legal scene");

            const float independent_extent = design.ornament_radius * scene.model_scale +
                                             design.stroke_width * 0.5f + 2.0f;
            const std::array<Vec2, 3> anchors{scene.pivot, scene.elbow, scene.tip};
            const std::array<const char*, 3> names{"pivot", "elbow", "tip"};
            for (std::size_t index = 0; index < anchors.size(); ++index) {
                const auto anchor = anchors[index];
                const std::string prefix = std::string(names[index]) + " independent extrema ";
                expect(anchor.x - independent_extent >= -0.002f, prefix + "left in viewport");
                expect(anchor.y - independent_extent >= -0.002f, prefix + "top in viewport");
                expect(anchor.x + independent_extent <=
                           static_cast<float>(viewport.width) + 0.002f,
                       prefix + "right in viewport");
                expect(anchor.y + independent_extent <=
                           static_cast<float>(viewport.height) + 0.002f,
                       prefix + "bottom in viewport");
            }
        }
    }
    expect(!makeScene(design, {47, 100}, 0.0f).valid, "viewport narrower than 48 rejected");
    expect(!makeScene(design, {100, 47}, 0.0f).valid, "viewport shorter than 48 rejected");

    Scene invalid_scale_scene = makeScene(design, {320, 240}, 0.0f);
    invalid_scale_scene.model_scale = 0.0f;
    expect(!sceneIsFiniteAndInBounds(invalid_scale_scene, design, {320, 240}),
           "scene predicate rejects zero model scale");
    invalid_scale_scene.model_scale = -1.0f;
    expect(!sceneIsFiniteAndInBounds(invalid_scale_scene, design, {320, 240}),
           "scene predicate rejects negative model scale");
    invalid_scale_scene.model_scale = std::numeric_limits<float>::quiet_NaN();
    expect(!sceneIsFiniteAndInBounds(invalid_scale_scene, design, {320, 240}),
           "scene predicate rejects non-finite model scale");
}
void invalidAndLearnerContractCases() {
    using namespace sculpture;
    auto invalid = knownDesign(); invalid.first_length = 0.0f;
    expect(!designIsValid(invalid), "zero arm length rejected");
    invalid = knownDesign(); invalid.period_seconds = std::numeric_limits<float>::quiet_NaN();
    expect(!designIsValid(invalid), "non-finite period rejected");
    invalid = knownDesign(); invalid.ink.r = 300;
    expect(!designIsValid(invalid), "out-of-range color rejected");
    const Design learner = makeSculptureDesign();
    expect(designIsValid(learner), "learner design satisfies public validity contract");
    expect(learner.first_length + learner.second_length > learner.ornament_radius * 4.0f,
           "learner hierarchy has visible arm reach");
    expect(std::fabs(learner.root_swing_degrees) >= 5.0f &&
           std::fabs(learner.child_swing_degrees) >= 5.0f,
           "learner owns visible parent and child motion");
}
}
int main(int argc, char** argv) {
    expect(argc == 2, "usage supplies transformed-anchor fixture path");
    primitiveTransformCases();
    orderCases();
    if (argc == 2) fixtureCases(readFixtures(argv[1]));
    timeCases();
    boundsCases();
    invalidAndLearnerContractCases();
    if (failures != 0) {
        std::cerr << "sculpture_model_test: " << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "sculpture_model_test: identity/translation/rotation/scale, order, parsed three-frame parent-child anchors, periodic deterministic time, policies, stroke-aware bounds, and learner contract passed\n";
    return 0;
}
