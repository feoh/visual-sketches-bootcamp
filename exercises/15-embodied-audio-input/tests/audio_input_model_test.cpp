#include "audio_input_model.h"
#include "audio_instrument_design.h"

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

void check(bool condition, const std::string& label) {
    if (!condition) {
        std::cerr << "FAIL: " << label << '\n';
        ++failures;
    }
}

bool near(float actual, float expected, float tolerance = 0.00002f) {
    return std::fabs(actual - expected) <= tolerance;
}

struct FixtureRow {
    std::string id;
    float amplitude = 0.0f;
    float smoothed = 0.0f;
    float normalized = 0.0f;
    float radius = 0.0f;
    int rays = 0;
    int active = 0;
};

std::vector<FixtureRow> readFixture(const std::string& path) {
    std::ifstream input(path);
    check(static_cast<bool>(input), "recorded amplitude fixture opens");
    std::vector<FixtureRow> rows;
    std::string line;
    int line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty() || line.front() == '#') continue;
        FixtureRow row;
        std::istringstream fields(line);
        std::string extra;
        if (!(fields >> row.id >> row.amplitude >> row.smoothed >> row.normalized >>
              row.radius >> row.rays >> row.active) || (fields >> extra)) {
            std::cerr << "FAIL: fixture line " << line_number
                      << " must have exactly seven fields\n";
            ++failures;
            continue;
        }
        rows.push_back(row);
    }
    check(!rows.empty(), "recorded amplitude fixture has rows");
    return rows;
}

void testKnownReplay(const std::vector<FixtureRow>& rows,
                     const embodied::Design& design) {
    embodied::State state;
    embodied::reset(state, design, embodied::InputSource::recorded);
    std::vector<float> amplitudes;
    for (const auto& row : rows) {
        amplitudes.push_back(row.amplitude);
        check(embodied::consumeAmplitude(state, row.amplitude, design),
              row.id + " accepted");
        check(near(state.smoothed_amplitude, row.smoothed),
              row.id + " smoothing oracle");
        check(near(state.geometry.normalized_level, row.normalized),
              row.id + " dead-zone oracle");
        check(near(state.geometry.radius, row.radius), row.id + " radius oracle");
        check(state.geometry.ray_count == row.rays, row.id + " ray-count oracle");
        check(state.geometry.active == (row.active == 1), row.id + " active oracle");
    }
    const embodied::State first = state;
    check(embodied::replayRecorded(state, amplitudes, design), "recorded replay succeeds");
    check(state.source == embodied::InputSource::recorded, "replay identifies source");
    check(state.accepted_samples == amplitudes.size(), "replay count is deterministic");
    check(near(state.smoothed_amplitude, first.smoothed_amplitude),
          "replay produces same final smoothing");
    check(near(state.geometry.radius, first.geometry.radius) &&
          state.geometry.ray_count == first.geometry.ray_count,
          "replay produces same inspectable geometry");
}

void testBoundariesAndProperties(const embodied::Design& design) {
    embodied::State state;
    embodied::reset(state, design, embodied::InputSource::recorded);
    check(near(state.geometry.radius, design.minimum_radius), "reset maps to minimum radius");
    check(state.geometry.ray_count == design.minimum_rays, "reset maps to minimum rays");

    auto immediate = design;
    immediate.smoothing = 1.0f;
    embodied::State boundary;
    embodied::reset(boundary, immediate, embodied::InputSource::recorded);
    check(embodied::consumeAmplitude(boundary, immediate.dead_zone, immediate),
          "exact dead-zone input accepted");
    check(!boundary.geometry.active && near(boundary.geometry.normalized_level, 0.0f),
          "exact dead-zone boundary remains quiet");

    check(embodied::consumeAmplitude(state, 2.0f, design), "amplitudes above one clamp");
    check(near(state.raw_amplitude, 1.0f), "raw amplitude clamp is inspectable");
    const embodied::State before_invalid = state;
    check(!embodied::consumeAmplitude(state, -0.01f, design), "negative amplitude rejects");
    check(!embodied::consumeAmplitude(state,
          std::numeric_limits<float>::quiet_NaN(), design), "NaN amplitude rejects");
    check(near(state.smoothed_amplitude, before_invalid.smoothed_amplitude),
          "invalid values do not change response state");
    check(state.rejected_samples == before_invalid.rejected_samples + 2,
          "invalid values increment rejection evidence");

    float previous_radius = design.minimum_radius;
    for (int step = 0; step < 32; ++step) {
        check(embodied::consumeAmplitude(state, 1.0f, design), "property input accepted");
        check(state.geometry.radius >= previous_radius, "steady loud input never shrinks radius");
        check(state.geometry.radius >= design.minimum_radius &&
              state.geometry.radius <= design.maximum_radius,
              "radius remains bounded");
        check(state.geometry.ray_count >= design.minimum_rays &&
              state.geometry.ray_count <= design.maximum_rays,
              "ray work remains bounded");
        previous_radius = state.geometry.radius;
    }
}

void testBoundedBatchAndFallback(const embodied::Design& design) {
    embodied::State state;
    embodied::reset(state, design, embodied::InputSource::live_microphone);
    const std::vector<float> large_batch(embodied::kMaximumBatchSize + 17, 0.25f);
    check(embodied::consumeBatch(state, large_batch, design) ==
              embodied::kMaximumBatchSize,
          "batch work cap is enforced");
    check(state.dropped_samples == 17, "batch reports dropped values");

    check(embodied::setNoDeviceFallback(state, 0.75f, design),
          "keyboard fallback accepts finite value");
    check(state.source == embodied::InputSource::no_device,
          "fallback state is explicit");
    check(state.geometry.radius > design.minimum_radius,
          "fallback drives the same geometry without a device");

    const embodied::State before = state;
    std::vector<float> too_large(embodied::kMaximumFixtureSamples + 1, 0.0f);
    check(!embodied::replayRecorded(state, too_large, design),
          "oversize replay is rejected");
    check(state.source == before.source &&
          near(state.smoothed_amplitude, before.smoothed_amplitude),
          "rejected replay is transactional");
}

void testDesignValidation() {
    const embodied::Design valid = makeAudioInstrumentDesign();
    check(embodied::designIsValid(valid), "learner design seam is valid");
    auto invalid = valid;
    invalid.dead_zone = 1.0f;
    check(!embodied::designIsValid(invalid), "dead zone of one rejects");
    invalid = valid;
    invalid.maximum_rays = 129;
    check(!embodied::designIsValid(invalid), "geometry work cap rejects");
    invalid = valid;
    invalid.background.r = 256;
    check(!embodied::designIsValid(invalid), "palette channels are bounded");
}
}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: audio-input-model-test amplitude-replay.txt\n";
        return 2;
    }
    const auto design = makeAudioInstrumentDesign();
    const auto rows = readFixture(argv[1]);
    testDesignValidation();
    testKnownReplay(rows, design);
    testBoundariesAndProperties(design);
    testBoundedBatchAndFallback(design);
    if (failures != 0) return 1;
    std::cout << "section-15-model: recorded replay, smoothing, dead zone, geometry, bounds, and no-device fallback passed\n";
    return 0;
}
