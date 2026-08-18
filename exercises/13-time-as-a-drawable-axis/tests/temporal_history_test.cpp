#include "temporal_design.h"
#include "temporal_history.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

bool close(float first, float second, float tolerance = 1e-4f) {
    return std::fabs(first - second) <= tolerance;
}

temporal::Sample numbered(std::uint64_t frame) {
    return {frame, static_cast<double>(frame) * 0.1,
            {static_cast<float>(frame), static_cast<float>(frame * 2u)},
            static_cast<float>(frame) * 0.01f};
}

void testFixture(const std::string& path) {
    std::ifstream input(path);
    require(static_cast<bool>(input), "fixture opens");
    std::string line;
    std::size_t rows = 0;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        std::istringstream stream(line);
        std::vector<std::string> fields;
        std::string field;
        while (stream >> field) fields.push_back(field);
        require(fields.size() >= 2, "fixture row has label and kind");
        if (fields[1] == "motion") {
            require(fields.size() == 9, fields[0] + " has motion fields");
            temporal::Sample sample;
            require(temporal::makeMotionSample(
                        static_cast<std::uint64_t>(std::stoull(fields[2])),
                        std::stod(fields[3]), std::stof(fields[4]),
                        std::stof(fields[5]), sample), fields[0] + " motion succeeds");
            require(close(sample.position.x, std::stof(fields[6])) &&
                    close(sample.position.y, std::stof(fields[7])) &&
                    close(sample.phase, std::stof(fields[8])),
                    fields[0] + " motion values");
        } else if (fields[1] == "selection") {
            require(fields.size() == 5, fields[0] + " has selection fields");
            std::size_t age = 99;
            require(temporal::ageFromPosition(std::stof(fields[3]),
                                               static_cast<std::size_t>(std::stoull(fields[2])),
                                               age), fields[0] + " selection succeeds");
            require(age == static_cast<std::size_t>(std::stoull(fields[4])),
                    fields[0] + " selection age");
        } else {
            throw std::runtime_error(fields[0] + " has known fixture kind");
        }
        ++rows;
    }
    require(rows == 6, "fixture row count");
}

void testOrderWrapAndLength() {
    temporal::History history;
    require(temporal::configure(history, 3), "configure three");
    for (std::uint64_t frame = 1; frame <= 3; ++frame)
        require(temporal::push(history, numbered(frame)), "initial push");
    for (std::size_t index = 0; index < 3; ++index) {
        temporal::Sample sample;
        require(temporal::atOldestIndex(history, index, sample), "oldest indexing");
        require(sample.frame == index + 1, "oldest-to-newest order");
    }
    require(temporal::push(history, numbered(4)) &&
            temporal::push(history, numbered(5)), "wrapped pushes");
    require(temporal::length(history) == 3, "history remains capped");
    for (std::size_t age = 0; age < 3; ++age) {
        temporal::Sample sample;
        require(temporal::atAge(history, age, sample), "age indexing after wrap");
        require(sample.frame == 5 - age, "newest-to-oldest wrap order");
    }
    require(temporal::valid(history), "wrapped history valid");
}

void testSelectionAndDecay() {
    temporal::History history;
    require(temporal::configure(history, 5), "configure selection");
    for (std::uint64_t frame = 1; frame <= 5; ++frame)
        require(temporal::push(history, numbered(frame)), "selection push");
    temporal::Sample newest;
    temporal::Sample middle;
    temporal::Sample oldest;
    float newest_opacity = 0.0f;
    float middle_opacity = 0.0f;
    float oldest_opacity = 0.0f;
    require(temporal::select(history, 0.0f, 2.0f, newest, newest_opacity), "new selection");
    require(temporal::select(history, 0.5f, 2.0f, middle, middle_opacity), "mid selection");
    require(temporal::select(history, 1.0f, 2.0f, oldest, oldest_opacity), "old selection");
    require(newest.frame == 5 && middle.frame == 3 && oldest.frame == 1,
            "space chooses deterministic age");
    require(close(newest_opacity, 1.0f) && newest_opacity > middle_opacity &&
            middle_opacity > oldest_opacity && close(oldest_opacity, std::exp(-2.0f)),
            "decay is monotone and normalized");
}

void testResizeResetAndReplay() {
    temporal::History history;
    require(temporal::configure(history, 6), "configure resize");
    for (std::uint64_t frame = 1; frame <= 6; ++frame)
        require(temporal::push(history, numbered(frame)), "resize push");
    require(temporal::resize(history, 3), "shrink");
    for (std::size_t index = 0; index < 3; ++index) {
        temporal::Sample sample;
        require(temporal::atOldestIndex(history, index, sample), "shrink order");
        require(sample.frame == index + 4, "shrink keeps newest suffix");
    }
    require(temporal::resize(history, 8) && temporal::capacity(history) == 8 &&
            temporal::length(history) == 3, "grow preserves length");
    const temporal::History before_rejected_resize = history;
    require(!temporal::resize(history, temporal::maximum_samples + 1) &&
            history.storage.size() == before_rejected_resize.storage.size() &&
            history.count == before_rejected_resize.count &&
            history.next == before_rejected_resize.next,
            "over-budget resize rejects transactionally");
    temporal::reset(history);
    require(temporal::capacity(history) == 8 && temporal::length(history) == 0 &&
            temporal::valid(history), "reset clears but retains allocation contract");

    temporal::History first;
    temporal::History second;
    require(temporal::configure(first, 16) && temporal::configure(second, 16), "replay configure");
    for (std::uint64_t frame = 0; frame < 12; ++frame) {
        temporal::Sample a;
        temporal::Sample b;
        const double explicit_time = static_cast<double>(frame) / 60.0;
        require(temporal::makeMotionSample(frame, explicit_time, 640.0f, 360.0f, a) &&
                temporal::makeMotionSample(frame, explicit_time, 640.0f, 360.0f, b),
                "explicit replay samples");
        require(temporal::push(first, a) && temporal::push(second, b), "replay pushes");
    }
    for (std::size_t age = 0; age < 12; ++age) {
        temporal::Sample a;
        temporal::Sample b;
        require(temporal::atAge(first, age, a) && temporal::atAge(second, age, b) &&
                temporal::sameSample(a, b, 0.0f), "deterministic replay");
    }
}

void testMemoryFiniteAndOverflowRejection() {
    std::size_t bytes = 0;
    require(temporal::checkedMemoryBytes(1, bytes) && bytes == sizeof(temporal::Sample),
            "one sample byte budget");
    require(temporal::checkedMemoryBytes(temporal::maximum_samples, bytes) &&
            bytes <= temporal::maximum_history_bytes, "maximum budget accepted");
    require(!temporal::checkedMemoryBytes(0, bytes) &&
            !temporal::checkedMemoryBytes(temporal::maximum_samples + 1, bytes) &&
            !temporal::checkedMemoryBytes(std::numeric_limits<std::size_t>::max(), bytes),
            "zero, cap, and arithmetic overflow rejected");

    temporal::History history;
    require(temporal::configure(history, 3), "finite configure");
    require(temporal::push(history, numbered(1)), "finite initial push");
    const temporal::History before = history;
    temporal::Sample invalid = numbered(2);
    invalid.time_seconds = std::numeric_limits<double>::quiet_NaN();
    require(!temporal::push(history, invalid) && history.count == before.count &&
            history.next == before.next, "NaN push rejected transactionally");
    invalid = numbered(2);
    invalid.position.x = std::numeric_limits<float>::infinity();
    require(!temporal::push(history, invalid), "infinite coordinate rejected");
    invalid = numbered(2);
    invalid.time_seconds = 0.05;
    require(!temporal::push(history, invalid), "backward explicit time rejected");
    require(!temporal::push(history, numbered(1)), "non-increasing frame rejected");

    temporal::History maximum_frame;
    require(temporal::configure(maximum_frame, 2), "maximum frame configure");
    temporal::Sample top = numbered(1);
    top.frame = std::numeric_limits<std::uint64_t>::max();
    require(temporal::push(maximum_frame, top), "maximum explicit frame representable");
    require(!temporal::push(maximum_frame, numbered(2)), "frame wraparound rejected");
    temporal::Sample output;
    require(!temporal::makeMotionSample(1, std::numeric_limits<double>::infinity(),
                                        100.0f, 100.0f, output) &&
            !temporal::makeMotionSample(1, 1e308, 100.0f, 100.0f, output),
            "non-finite and derived float overflow rejected");
    std::size_t age = 0;
    require(!temporal::ageFromPosition(std::numeric_limits<float>::quiet_NaN(), 3, age) &&
            !temporal::ageFromPosition(-0.1f, 3, age) &&
            !temporal::ageFromPosition(1.1f, 3, age), "invalid spatial input rejected");
    temporal::Sample selected;
    float opacity = 0.0f;
    require(!temporal::select(history, 0.0f,
                              std::numeric_limits<float>::infinity(), selected, opacity),
            "invalid decay rejected");
}

void testMonotoneSelectionProperty() {
    for (std::size_t count = 1; count <= 64; ++count) {
        std::size_t previous = 0;
        for (int step = 0; step <= 1000; ++step) {
            std::size_t age = 0;
            require(temporal::ageFromPosition(static_cast<float>(step) / 1000.0f,
                                               count, age), "property selection defined");
            require(age < count && age >= previous, "property: x never selects newer age");
            previous = age;
        }
        require(previous == count - 1, "property reaches oldest endpoint");
    }
}

void testDesignSeam() {
    TemporalDesign design = makeTemporalDesign();
    require(temporalDesignIsValid(design), "starter design seam valid");
    design.history_capacity = temporal::maximum_samples + 1;
    require(!temporalDesignIsValid(design), "design seam honors memory cap");
}

}  // namespace

int main(int argc, char** argv) {
    try {
        require(argc == 2, "usage: temporal-history-test FIXTURE");
        testFixture(argv[1]);
        testOrderWrapAndLength();
        testSelectionAndDecay();
        testResizeResetAndReplay();
        testMemoryFiniteAndOverflowRejection();
        testMonotoneSelectionProperty();
        testDesignSeam();
        std::cout << "section-13-tests: order wrap length selection resize reset replay memory finite overflow property design PASS\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "section-13-tests: FAIL: " << error.what() << '\n';
        return 1;
    }
}
