#pragma once

#include "course_probe.h"

#include <cstdint>
#include <string>

namespace course::interlude {

using ModelFactory = Model (*)(std::uint32_t, const Viewport&);

struct TestResult {
    bool passed = false;
    std::string message;
};

// The factory argument lets the runner try this test against working and
// intentionally broken model functions instead of accepting a test that always passes.
TestResult learnerKnownCaseTest(ModelFactory makeModelUnderTest);

}  // namespace course::interlude
