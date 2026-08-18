#pragma once

#include "course_probe.h"

#include <cstdint>

namespace course::test {

struct DeterministicFixture {
    static constexpr float dt = 1.0f / 60.0f;
    static constexpr std::uint32_t seed = 0x5eed1234U;
    static constexpr Viewport viewport{640.0f, 360.0f};
    static constexpr StepInput input{{12.0f, -6.0f}};

    Model freshModel() const { return makeModel(seed, viewport); }
};

}  // namespace course::test
