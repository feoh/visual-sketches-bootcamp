#include "family_design.h"
#include "mark_family.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
struct ExpectedMark {
    float x;
    float y;
    float radius;
    int index;
};
struct Case {
    std::string name;
    markfamily::Viewport viewport;
    int count;
    float spacing;
    float base_radius;
    float radius_step;
    ExpectedMark first;
    ExpectedMark middle;
    ExpectedMark last;
};
int failures = 0;
void expect(bool condition, const std::string& message) {
    if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
bool near(float a, float b) { return std::fabs(a - b) <= 0.0002f; }
markfamily::Design known(const Case& fixture) {
    return {fixture.count, {0.5f, 0.5f}, fixture.spacing, fixture.base_radius,
            fixture.radius_step, {230, 90, 70}, {70, 180, 210}, {20, 25, 40}};
}
void expectMark(const markfamily::Mark& actual, const ExpectedMark& expected,
                const std::string& message) {
    expect(near(actual.center.x, expected.x) && near(actual.center.y, expected.y) &&
               near(actual.radius, expected.radius) && actual.family_index == expected.index,
           message + " expected (" + std::to_string(expected.x) + ", " +
               std::to_string(expected.y) + ", radius " + std::to_string(expected.radius) +
               ", index " + std::to_string(expected.index) + ")");
}
std::vector<Case> readCases(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "family fixture opens");
    std::vector<Case> cases;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream fields(line);
        Case fixture{};
        std::string extra;
        const bool parsed = static_cast<bool>(
            fields >> fixture.name >> fixture.viewport.width >> fixture.viewport.height >>
            fixture.count >> fixture.spacing >> fixture.base_radius >> fixture.radius_step >>
            fixture.first.x >> fixture.first.y >> fixture.first.radius >> fixture.first.index >>
            fixture.middle.x >> fixture.middle.y >> fixture.middle.radius >> fixture.middle.index >>
            fixture.last.x >> fixture.last.y >> fixture.last.radius >> fixture.last.index);
        const bool exact = parsed && !(fields >> extra);
        expect(exact, "each fixture row has exactly nineteen fields");
        if (exact) cases.push_back(fixture);
    }
    return cases;
}
void fixtureCases(const std::vector<Case>& cases) {
    for (const Case& fixture : cases) {
        const auto design = known(fixture);
        const auto first = markfamily::makeMarkFamily(design, fixture.viewport);
        const auto second = markfamily::makeMarkFamily(design, fixture.viewport);
        expect(first.size() == static_cast<std::size_t>(fixture.count), fixture.name + " count follows parameter");
        expect(markfamily::marksAreFiniteAndInBounds(first, fixture.viewport), fixture.name + " marks are finite and in bounds");
        expect(first.size() == second.size(), fixture.name + " repeated result has same size");
        for (std::size_t index = 0; index < first.size() && index < second.size(); ++index) {
            expect(near(first[index].center.x, second[index].center.x) &&
                       near(first[index].center.y, second[index].center.y) &&
                       near(first[index].radius, second[index].radius) &&
                       first[index].family_index == second[index].family_index,
                   fixture.name + " is deterministic at index " + std::to_string(index));
        }
        const bool representativeIndexesValid =
            fixture.first.index == 0 && fixture.middle.index == fixture.count / 2 &&
            fixture.last.index == fixture.count - 1 && fixture.count > 0;
        expect(representativeIndexesValid, fixture.name + " fixture identifies first, middle, and last indexes");
        if (first.size() == static_cast<std::size_t>(fixture.count) && representativeIndexesValid) {
            expectMark(first[static_cast<std::size_t>(fixture.first.index)], fixture.first,
                       fixture.name + " first known oracle");
            expectMark(first[static_cast<std::size_t>(fixture.middle.index)], fixture.middle,
                       fixture.name + " middle known oracle");
            expectMark(first[static_cast<std::size_t>(fixture.last.index)], fixture.last,
                       fixture.name + " last known oracle");
        }
    }
}
void oneParameterAtATimeCases() {
    const markfamily::Viewport viewport{800, 500};
    const Case fixture{"variation", viewport, 5, 30.0f, 10.0f, 0.0f,
                       {}, {}, {}};
    const auto baseline = markfamily::makeMarkFamily(known(fixture), viewport);
    expect(baseline.size() == 5, "baseline family has five marks");
    if (baseline.size() != 5) return;
    expectMark(baseline[0], {340.0f, 242.5f, 10.0f, 0}, "baseline first known oracle");
    expectMark(baseline[2], {400.0f, 242.5f, 10.0f, 2}, "baseline middle known oracle");
    expectMark(baseline[4], {460.0f, 242.5f, 10.0f, 4}, "baseline last known oracle");

    auto design = known(fixture);
    design.normalized_center = {0.6f, 0.6f};
    const auto recentered = markfamily::makeMarkFamily(design, viewport);
    expect(recentered.size() == 5, "normalized-center-only case retains count");
    if (recentered.size() == 5)
        expectMark(recentered[0], {420.0f, 292.5f, 10.0f, 0}, "normalized center alone controls placement");

    design = known(fixture);
    design.spacing_pixels = 40.0f;
    const auto respaced = markfamily::makeMarkFamily(design, viewport);
    expect(respaced.size() == 5, "spacing-only case retains count");
    if (respaced.size() == 5)
        expectMark(respaced[0], {320.0f, 240.0f, 10.0f, 0}, "spacing alone controls horizontal step and wave");

    design = known(fixture);
    design.base_radius_pixels = 14.0f;
    const auto rebased = markfamily::makeMarkFamily(design, viewport);
    expect(rebased.size() == 5, "base-radius-only case retains count");
    if (rebased.size() == 5)
        expectMark(rebased[0], {340.0f, 242.5f, 14.0f, 0}, "base radius alone controls uniform radius");

    design = known(fixture);
    design.radius_step_pixels = 2.0f;
    const auto stepped = markfamily::makeMarkFamily(design, viewport);
    expect(stepped.size() == 5, "radius-step-only case retains count");
    if (stepped.size() == 5) {
        expectMark(stepped[0], {340.0f, 242.5f, 14.0f, 0}, "radius step alone controls outer radius");
        expectMark(stepped[2], {400.0f, 242.5f, 10.0f, 2}, "radius step preserves center base radius");
    }

    const float retained_x = baseline[0].center.x;
    design.normalized_center.x = 0.8f;
    expect(near(baseline[0].center.x, retained_x), "returned vector owns values independent of later parameter changes");
}
void invalidAndBoundaryCase() {
    auto design = known({"boundary", {80, 60}, 24, 80.0f, 40.0f, 3.0f, {}, {}, {}});
    const auto squeezed = markfamily::makeMarkFamily(design, {80, 60});
    expect(squeezed.size() == 24 && markfamily::marksAreFiniteAndInBounds(squeezed, {80, 60}),
           "maximum valid family remains finite and bounded in a small viewport");
    design.count = 2;
    expect(markfamily::makeMarkFamily(design, {800, 600}).empty(), "invalid parameters produce an inspectable empty family");
    design = known({"invalid-viewport", {800, 600}, 5, 30.0f, 10.0f, 0.0f, {}, {}, {}});
    expect(markfamily::makeMarkFamily(design, {0, 600}).empty(), "non-positive viewport produces an empty family");
    expect(markfamily::makeMarkFamily(design, {1, 600}).empty(), "one-pixel-wide viewport produces an empty family");
    expect(markfamily::makeMarkFamily(design, {600, 1}).empty(), "one-pixel-high viewport produces an empty family");
    expect(markfamily::makeMarkFamily(design, {1, 1}).empty(), "one-by-one viewport produces an empty family");
    expect(markfamily::marksAreFiniteAndInBounds({}, {1, 600}) &&
               markfamily::marksAreFiniteAndInBounds({}, {600, 1}) &&
               markfamily::marksAreFiniteAndInBounds({}, {1, 1}),
           "empty family is the consistent valid result for sub-two-pixel viewports");
}
void learnerContract() {
    const auto design = makeFamilyDesign();
    expect(markfamily::designIsValid(design),
           "learner design has valid count, finite parameter ranges, colors, and distinct mark/background RGB");
    const auto marks = markfamily::makeMarkFamily(design, {900, 540});
    expect(marks.size() == static_cast<std::size_t>(design.count), "learner family size follows learner count");
    expect(markfamily::marksAreFiniteAndInBounds(marks, {900, 540}), "learner family is finite and in bounds");
    const auto repeated = markfamily::makeMarkFamily(design, {900, 540});
    expect(marks.size() == repeated.size() && !marks.empty(), "learner family repeats with a non-empty collection");
    if (!marks.empty() && marks.size() == repeated.size()) {
        expect(near(marks.front().center.x, repeated.front().center.x) &&
                   near(marks.back().radius, repeated.back().radius),
               "learner parameters produce deterministic representative values");
    }
}
}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "fixture path is provided");
    const auto cases = argc == 2 ? readCases(argv[1]) : std::vector<Case>{};
    expect(cases.size() == 3, "fixture retains three explicit parsed scenarios");
    fixtureCases(cases);
    oneParameterAtATimeCases();
    invalidAndBoundaryCase();
    learnerContract();
    if (failures) { std::cerr << failures << " section 02 checks failed\n"; return 1; }
    std::cout << "mark_family_test: parsed known oracles, independent parameter cases, bounds, determinism, and learner contract passed\n";
}
