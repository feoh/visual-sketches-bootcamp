#include "gesture_design.h"
#include "gesture_model.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
namespace {
struct Fixture { std::string name; float time; gesture::Vec2 raw; gesture::Vec2 smooth; float speed; float turn; float arc; };
int failures = 0;
void expect(bool condition, const std::string& message) { if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; } }
bool near(float a, float b, float tolerance = 0.0005f) { return std::isfinite(a) && std::isfinite(b) && std::fabs(a - b) <= tolerance; }
void expectNear(float a, float b, const std::string& label, float tolerance = 0.0005f) {
    if (!near(a, b, tolerance)) { ++failures; std::cerr << "FAIL: " << label << " actual=" << a << " expected=" << b << " tolerance=" << tolerance << '\n'; }
}
void expectVec(gesture::Vec2 a, gesture::Vec2 b, const std::string& label, float tolerance = 0.0005f) {
    expectNear(a.x, b.x, label + ".x", tolerance); expectNear(a.y, b.y, label + ".y", tolerance);
}
gesture::Design knownDesign() {
    return {0.0f, std::log(2.0f), 5.0f, 2.0f, 10.0f, 64,
            {10, 20, 30}, {210, 120, 60}, {245, 245, 240}};
}
std::vector<Fixture> readFixtures(const char* path) {
    std::ifstream input(path); expect(input.good(), "gesture oracle opens");
    std::vector<Fixture> result; std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        Fixture f{}; std::string extra; std::istringstream fields(line);
        const bool parsed = static_cast<bool>(fields >> f.name >> f.time >> f.raw.x >> f.raw.y >> f.smooth.x >> f.smooth.y >> f.speed >> f.turn >> f.arc);
        const bool exact = parsed && !(fields >> extra);
        expect(exact, "each oracle row has exactly nine fields"); if (exact) result.push_back(f);
    }
    return result;
}
void fixtureCases(const std::vector<Fixture>& fixtures) {
    expect(fixtures.size() == 4, "oracle has four independent rows");
    gesture::Gesture path; const auto design = knownDesign();
    for (const auto& f : fixtures) {
        expect(gesture::addSample(path, f.raw, f.time, design), f.name + " accepted");
        const auto& sample = path.samples.back();
        expectVec(sample.position, f.smooth, f.name + " smoothed");
        expectNear(sample.speed, f.speed, f.name + " speed");
        expectNear(sample.signed_turn_radians, f.turn, f.name + " signed turn");
        expectNear(sample.arc_length, f.arc, f.name + " cumulative arc");
    }
}
void growthFilterAndPruneCases() {
    auto design = knownDesign(); design.minimum_distance = 3.0f; design.maximum_samples = 3;
    gesture::Gesture path;
    expect(gesture::addSample(path, {10, 10}, 0, design), "first sample accepted");
    expect(!gesture::addSample(path, {12, 10}, 1, design), "below-minimum movement filtered");
    expect(path.samples.size() == 1, "filtered input does not grow vector");
    expect(gesture::addSample(path, {13, 10}, 1, design), "equal-minimum movement accepted");
    expect(gesture::addSample(path, {16, 10}, 2, design), "third sample accepted");
    const std::size_t capacity_before = path.samples.capacity();
    expect(gesture::addSample(path, {19, 10}, 3, design), "fourth sample accepted while pruning");
    expect(path.samples.size() == 3 && near(path.samples.front().raw.x, 13), "oldest sample pruned at maximum");
    expect(path.samples.capacity() >= capacity_before, "pruning does not promise to shrink capacity");
    gesture::clear(path);
    expect(path.samples.empty(), "clear resets logical size");
    expect(path.samples.capacity() >= capacity_before, "clear retains reusable capacity");
    const float extreme = std::numeric_limits<float>::max();
    gesture::Gesture extremes;
    expect(gesture::addSample(extremes, {extreme, 0}, 0, design), "first finite extreme sample is accepted");
    expect(!gesture::addSample(extremes, {-extreme, 0}, 1, design), "unsafe extreme displacement is rejected");
    expect(extremes.samples.size() == 1, "derived-value rejection leaves gesture unchanged");
}
void smoothingAndVelocityCases() {
    using namespace gesture;
    const float k = std::log(2.0f);
    const Vec2 once = exponentialSmooth({0, 0}, {10, 0}, k, 1.0f);
    Vec2 split = exponentialSmooth({0, 0}, {10, 0}, k, 0.5f);
    split = exponentialSmooth(split, {10, 0}, k, 0.5f);
    expectVec(once, {5, 0}, "one-second half-life smoothing");
    expectVec(split, once, "time-aware smoothing tolerates frame partition", 0.0001f);
    const float nan = std::numeric_limits<float>::quiet_NaN();
    expectVec(exponentialSmooth({2, 3}, {9, 9}, k, 0), {2, 3}, "zero dt preserves current");
    expectVec(exponentialSmooth({2, 3}, {9, 9}, k, -1), {2, 3}, "negative dt preserves current");
    expectVec(exponentialSmooth({2, 3}, {9, 9}, k, nan), {2, 3}, "non-finite dt preserves current");
    expectVec(guardedVelocity({2, 3}, {8, 11}, 2), {3, 4}, "velocity is displacement over time");
    expectVec(guardedVelocity({2, 3}, {8, 11}, 0), {0, 0}, "zero-dt velocity guarded");
    expectVec(guardedVelocity({2, 3}, {8, 11}, -1), {0, 0}, "negative-dt velocity guarded");
    const float extreme = std::numeric_limits<float>::max();
    const Vec2 extreme_smooth = exponentialSmooth({extreme, extreme}, {-extreme, -extreme}, k, 1.0f);
    expect(std::isfinite(extreme_smooth.x) && std::isfinite(extreme_smooth.y), "extreme smoothing uses finite wider intermediates");
    expectVec(guardedVelocity({-extreme, 0}, {extreme, 0}, 0.001f), {0, 0}, "overflowing velocity is rejected safely");
}
void curvatureCases() {
    using namespace gesture;
    expectNear(signedTurningAngle({0,0}, {5,0}, {10,0}), 0, "straight turn");
    expectNear(signedTurningAngle({0,0}, {5,0}, {5,5}), pi * 0.5f, "positive-down right corner signed positive");
    expectNear(signedTurningAngle({0,0}, {5,0}, {5,-5}), -pi * 0.5f, "opposite corner signed negative");
    expectNear(signedTurningAngle({0,0}, {0,0}, {5,0}), 0, "duplicate incoming segment guarded");
    expectNear(signedTurningAngle({0,0}, {5,0}, {5,0}), 0, "duplicate outgoing segment guarded");
    const float extreme = std::numeric_limits<float>::max();
    expect(std::isfinite(signedTurningAngle({-extreme,0}, {0,extreme}, {extreme,0})), "extreme finite turn remains finite");
}
void resamplingCases() {
    using namespace gesture;
    expect(uniformResample({}, 5).empty(), "empty path stays empty");
    auto one = uniformResample({{2,3}}, 5); expect(one.size() == 1, "single point preserved"); expectVec(one[0], {2,3}, "single coordinate");
    auto short_path = uniformResample({{0,0},{3,0}}, 5);
    expect(short_path.size() == 2, "short path preserves both endpoints"); expectVec(short_path.front(), {0,0}, "short first"); expectVec(short_path.back(), {3,0}, "short last");
    auto duplicate = uniformResample({{0,0},{0,0},{10,0},{10,0}}, 5);
    expect(duplicate.size() == 3, "exact duplicates removed without losing spacing");
    auto near_distinct = uniformResample({{0,0},{0.000001f,0}}, 5);
    expect(near_distinct.size() == 2, "near but distinct endpoints are preserved");
    expect(near_distinct.back().x == 0.000001f, "near endpoint coordinate remains exact");
    expect(uniformResample({{0,0},{10,0}}, minimum_resample_spacing * 0.5f).empty(), "tiny spacing is rejected");
    expect(uniformResample({{0,0},{101,0}}, minimum_resample_spacing).empty(), "output exceeding resample cap is rejected");
    auto corner = uniformResample({{0,0},{10,0},{10,10}}, 5);
    const std::vector<Vec2> expected{{0,0},{5,0},{10,0},{10,5},{10,10}};
    expect(corner.size() == expected.size(), "corner produces uniform count with endpoints");
    for (std::size_t i=0; i<corner.size() && i<expected.size(); ++i) expectVec(corner[i], expected[i], "corner sample " + std::to_string(i));
    for (std::size_t i=1; i<corner.size(); ++i) expectNear(distance(corner[i-1], corner[i]), 5, "uniform five-unit spacing");
    expect(uniformResample({{-std::numeric_limits<float>::max(),0},
                            {std::numeric_limits<float>::max(),0}}, 1).empty(),
           "finite extreme resampling rejects an oversized result");
}
void facetGeometryCases() {
    gesture::Triangle triangle{};
    expect(gesture::facetTriangle({10,10}, {20,10}, 4, triangle), "horizontal facet is constructed");
    expectVec(triangle.first, {10,8}, "facet first uses half-width normal");
    expectVec(triangle.second, {10,12}, "facet second uses half-width normal");
    expectVec(triangle.tip, {20,10}, "facet tip preserves endpoint");
    expectNear(gesture::distance({10,10}, triangle.first), 2, "facet first extent is half width");
    expectNear(gesture::distance({10,10}, triangle.second), 2, "facet second extent is half width");
    expect(!gesture::facetTriangle({1,1}, {1,1}, 4, triangle), "duplicate facet is rejected");
    expect(!gesture::facetTriangle({1,1}, {2,2}, std::numeric_limits<float>::infinity(), triangle), "non-finite facet width is rejected");
}
void boundsDeterminismAndVariationCases() {
    auto design = knownDesign(); design.minimum_distance = 0; gesture::Gesture first, replay, alternate;
    for (int i=0; i<5; ++i) { const gesture::Vec2 p{20.0f + i*20.0f, 30.0f + (i%2)*10.0f}; gesture::addSample(first,p,static_cast<float>(i),design); gesture::addSample(replay,p,static_cast<float>(i),design); }
    for (int i=0; i<5; ++i) gesture::addSample(alternate,{20.0f+i*15.0f, 40.0f+i*12.0f},static_cast<float>(i),design);
    expect(first.samples.size() == replay.samples.size(), "deterministic replay size");
    for (std::size_t i=0; i<first.samples.size(); ++i) expectVec(first.samples[i].position,replay.samples[i].position,"deterministic replay point");
    expect(!near(first.samples.back().position.y, alternate.samples.back().position.y), "different input produces variation");
    expect(gesture::finiteAndStrokeAwareInBounds(first, 140, 100, design.maximum_width), "finite path with stroke radius in bounds");
    first.samples.back().position.x = 139.0f;
    expect(!gesture::finiteAndStrokeAwareInBounds(first, 140, 100, design.maximum_width), "stroke-aware right extent rejected");
    gesture::Sample slow{{},{},0,{},0,0,0}; gesture::Sample quick=slow; quick.speed=240; quick.signed_turn_radians=gesture::pi;
    const auto slow_style=gesture::styleForSample(slow,design); const auto quick_style=gesture::styleForSample(quick,design);
    expectNear(slow_style.width,design.maximum_width,"slow maps wide"); expectNear(quick_style.width,design.minimum_width,"fast maps narrow");
    expect(slow_style.color.r != quick_style.color.r, "curvature varies palette");
}
void learnerContractCases() {
    const auto learner=makeGestureDesign(); expect(gesture::designIsValid(learner),"learner design valid");
    expect(learner.maximum_width-learner.minimum_width >= 2.0f,"learner owns visible width range");
    expect(learner.slow_color.r != learner.fast_color.r || learner.slow_color.g != learner.fast_color.g || learner.slow_color.b != learner.fast_color.b,"learner owns curvature palette variation");
}
}
int main(int argc,char** argv) {
    expect(argc==2,"usage supplies gesture oracle path"); if(argc==2) fixtureCases(readFixtures(argv[1]));
    growthFilterAndPruneCases(); smoothingAndVelocityCases(); curvatureCases(); resamplingCases(); facetGeometryCases(); boundsDeterminismAndVariationCases(); learnerContractCases();
    if(failures){std::cerr<<"gesture_model_test: "<<failures<<" failure(s)\n";return 1;}
    std::cout<<"gesture_model_test: parsed oracle, growth/pruning/filtering, time-aware smoothing, guarded velocity/curvature, bounded arc-length resampling, facet geometry, bounds, determinism/variation, and learner contract passed\n";
}
