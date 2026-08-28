#include "traveler_design.h"
#include "traveler_model.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
struct FrameStream { std::string name; int frame_count; float frame_seconds; };
int failures = 0;
void expect(bool value, const std::string& message) {
    if (!value) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
bool near(float a, float b, float tolerance = 0.0002f) {
    return std::fabs(a - b) <= tolerance * std::max({1.0f, std::fabs(a), std::fabs(b)});
}
traveler::Design known() {
    return {{0.5f, 0.5f}, 120.0f, 10.0f, {240, 80, 60}, {250, 200, 70}, {20, 30, 50}};
}
traveler::Input direction(float x, float y) { return {false, {0, 0}, {x, y}}; }
traveler::Input right() { return direction(1, 0); }

std::vector<FrameStream> readFrameStreams(const char* path) {
    std::ifstream fixture(path);
    std::vector<FrameStream> streams;
    expect(fixture.good(), "frame-stream fixture opens");
    std::string line;
    while (std::getline(fixture, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream fields(line);
        FrameStream stream{};
        std::string extra;
        const bool parsed = static_cast<bool>(fields >> stream.name >> stream.frame_count >> stream.frame_seconds);
        expect(parsed && !(fields >> extra), "each fixture row has exactly name, frame_count, and frame_seconds");
        if (parsed) streams.push_back(stream);
    }
    return streams;
}
const FrameStream& requireStream(const std::vector<FrameStream>& streams, const std::string& name) {
    const auto found = std::find_if(streams.begin(), streams.end(), [&](const FrameStream& stream) {
        return stream.name == name;
    });
    if (found == streams.end()) {
        expect(false, "fixture contains scenario " + name);
        static const FrameStream missing{"missing", 0, 0.0f};
        return missing;
    }
    expect(found->frame_count > 0 && std::isfinite(found->frame_seconds) && found->frame_seconds > 0.0f,
           name + " has usable frame count and duration");
    return *found;
}
void runStream(traveler::State& state, const FrameStream& stream, traveler::Input input,
               traveler::Viewport viewport) {
    for (int frame = 0; frame < stream.frame_count; ++frame) {
        traveler::advanceFrame(state, known(), input, stream.frame_seconds, viewport);
    }
}

void stepDistanceCase() {
    expect(near(traveler::stepDistance(120.0f, 1.0f / 60.0f), 2.0f),
           "step distance multiplies pixels per second by elapsed seconds");
    expect(near(traveler::stepDistance(-90.0f, 0.5f), -45.0f),
           "step distance preserves the direction of a signed rate");
    expect(near(traveler::stepDistance(300.0f, 0.0f), 0.0f),
           "zero elapsed time produces zero distance");
}
void fixedDtCase() {
    auto state = traveler::makeState(known(), {800, 600});
    traveler::advanceFrame(state, known(), right(), traveler::fixedStepSeconds(), {800, 600});
    expect(near(state.position.x, 401.0f) && near(state.position.y, 300.0f),
           "one fixed 1/120-second step advances speed times time");
}
void framePartitionCase(const std::vector<FrameStream>& streams) {
    auto one = traveler::makeState(known(), {800, 600});
    auto four = one;
    runStream(one, requireStream(streams, "one-thirtieth"), right(), {800, 600});
    runStream(four, requireStream(streams, "four-one-one-twentieths"), right(), {800, 600});
    expect(near(one.position.x, four.position.x) && near(one.position.y, four.position.y),
           "fixture-defined equal elapsed time partitions into the same fixed steps");
    auto variable = traveler::makeState(known(), {800, 600});
    traveler::Input pointer{true, {700, 500}, {0, 0}};
    traveler::advanceVariable(variable, known(), pointer,
                              requireStream(streams, "one-thirtieth").frame_seconds, {800, 600});
    expect(traveler::stateIsFiniteAndWrapped(variable, known(), {800, 600}),
           "variable-step comparison remains finite");
}
void pauseSpikeCase(const std::vector<FrameStream>& streams) {
    auto state = traveler::makeState(known(), {800, 600});
    runStream(state, requireStream(streams, "pause-spike"), right(), {800, 600});
    expect(near(state.position.x, 412.0f, 0.001f), "fixture pause spike is clamped to 0.1 second");
    traveler::togglePause(state);
    const float x = state.position.x;
    traveler::advanceFrame(state, known(), right(), 0.1f, {800, 600});
    expect(near(state.position.x, x) && near(state.accumulator_seconds, 0.0f),
           "paused frames neither move nor accumulate catch-up time");
}
void invalidDurationCase() {
    auto state = traveler::makeState(known(), {800, 600});
    const float half = traveler::fixedStepSeconds() * 0.5f;
    traveler::advanceFrame(state, known(), right(), half, {800, 600});
    const float x = state.position.x;
    for (float invalid : {0.0f, -half, std::numeric_limits<float>::quiet_NaN(),
                          std::numeric_limits<float>::infinity()}) {
        traveler::advanceFrame(state, known(), right(), invalid, {800, 600});
        expect(near(state.position.x, x) && near(state.accumulator_seconds, half),
               "zero, negative, NaN, and infinity preserve a pending half-step");
    }
    traveler::advanceFrame(state, known(), right(), half, {800, 600});
    expect(near(state.position.x, x + 1.0f) && near(state.accumulator_seconds, 0.0f),
           "a valid second half-step consumes the remainder retained across invalid frames");
    traveler::advanceFrame(state, known(), right(), half, {800, 600});
    state.paused = true;
    const float paused_x = state.position.x;
    traveler::advanceFrame(state, known(), right(), half, {800, 600});
    expect(near(state.position.x, paused_x) && near(state.accumulator_seconds, 0.0f),
           "a paused frame still discards a pending accumulator remainder");
}
void boundaryCase(const std::vector<FrameStream>& streams) {
    const traveler::Viewport viewport{100, 80};
    const float dt = traveler::fixedStepSeconds();
    auto right_edge = traveler::makeState(known(), viewport);
    right_edge.position = {109.0f, 40.0f};
    traveler::advanceFrame(right_edge, known(), direction(1, 0), dt, viewport);
    expect(near(right_edge.position.x, -10.0f), "exact right outer endpoint wraps to left outer endpoint");

    auto left_edge = traveler::makeState(known(), viewport);
    left_edge.position = {-9.0f, 40.0f};
    traveler::advanceFrame(left_edge, known(), direction(-1, 0), dt, viewport);
    expect(near(left_edge.position.x, -10.0f), "exact left outer endpoint remains in the wrapped domain");

    auto bottom_edge = traveler::makeState(known(), viewport);
    bottom_edge.position = {50.0f, 89.0f};
    traveler::advanceFrame(bottom_edge, known(), direction(0, 1), dt, viewport);
    expect(near(bottom_edge.position.y, -10.0f), "exact bottom outer endpoint wraps to top outer endpoint");

    auto top_edge = traveler::makeState(known(), viewport);
    top_edge.position = {50.0f, -9.0f};
    traveler::advanceFrame(top_edge, known(), direction(0, -1), dt, viewport);
    expect(near(top_edge.position.y, -10.0f), "exact top outer endpoint remains in the wrapped domain");

    runStream(right_edge, requireStream(streams, "boundary-run"), direction(-1, -1), viewport);
    expect(traveler::stateIsFiniteAndWrapped(right_edge, known(), viewport),
           "fixture-defined boundary run remains finite and inside wrapped domain");
}
void learnerContract() {
    const auto design = makeTravelerDesign();
    expect(traveler::designIsValid(design),
           "learner design has finite start, speed/radius ranges, valid colors, and distinct mark/background RGB");
    auto state = traveler::makeState(design, {800, 600});
    expect(traveler::stateIsFiniteAndWrapped(state, design, {800, 600}),
           "learner initial state is inspectable and wrapped");
    traveler::toggleReducedMotion(state);
    traveler::advanceFrame(state, design, right(), 0.1f, {800, 600});
    expect(near(std::sqrt(state.velocity.x * state.velocity.x + state.velocity.y * state.velocity.y),
                design.speed_pixels_per_second * 0.25f),
           "reduced-motion mode uses one quarter speed");
    traveler::reset(state, design, {800, 600});
    expect(near(state.position.x, design.normalized_start.x * 800) &&
               near(state.position.y, design.normalized_start.y * 600),
           "reset reproduces learner start position");
    expect(state.reduced_motion, "reset preserves the reduced-motion preference");
    expect(near(state.velocity.x, 0.0f) && near(state.velocity.y, 0.0f),
           "reset clears both velocity components");
    expect(!state.paused && near(state.accumulator_seconds, 0.0f),
           "reset clears pause and pending simulation time");
}
}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "fixture path is provided");
    const std::vector<FrameStream> streams = argc == 2 ? readFrameStreams(argv[1]) : std::vector<FrameStream>{};
    expect(streams.size() == 4, "frame-stream fixture retains four explicit parsed scenarios");
    stepDistanceCase();
    fixedDtCase();
    framePartitionCase(streams);
    pauseSpikeCase(streams);
    invalidDurationCase();
    boundaryCase(streams);
    learnerContract();
    if (failures) { std::cerr << failures << " section 01 checks failed\n"; return 1; }
    std::cout << "traveler_model_test: fixtures, fixed dt, invalid time, pause, four-edge wrap, and learner contract passed\n";
}
