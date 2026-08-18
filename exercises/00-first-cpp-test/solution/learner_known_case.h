#pragma once

#include "course_probe.h"

#include <cstdint>
#include <string>

namespace course::interlude {
using ModelFactory = Model (*)(std::uint32_t, const Viewport&);
struct TestResult { bool passed = false; std::string message; };
TestResult learnerKnownCaseTest(ModelFactory makeModelUnderTest);
}  // namespace course::interlude
