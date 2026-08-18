#include "learner_known_case.h"

#include "expect_near.h"

#include <array>

namespace course::interlude {

TestResult learnerKnownCaseTest(ModelFactory makeModelUnderTest) {
    // Arrange: replace this green example with a seed and reviewed oracle of
    // your own. Keep expected values independent of makeModelUnderTest.
    const std::uint32_t seed = 17U;
    const Viewport viewport{640.0f, 480.0f};
    const Vec2 expectedPosition{0.6848907471f, 128.5272827f};
    const Vec2 expectedVelocity{-27.47343063f, -79.1796875f};

    // Act.
    const Model actual = makeModelUnderTest(seed, viewport);

    // Assert. The first failing comparison names the component and supplies its
    // numeric diagnostic.
    const std::array<const char*, 4> labels{"position.x", "position.y", "velocity.x", "velocity.y"};
    const std::array<test::NearResult, 4> results{
        test::expectNear(actual.position.x, expectedPosition.x, 1e-5, 1e-6),
        test::expectNear(actual.position.y, expectedPosition.y, 1e-5, 1e-6),
        test::expectNear(actual.velocity.x, expectedVelocity.x, 1e-5, 1e-6),
        test::expectNear(actual.velocity.y, expectedVelocity.y, 1e-5, 1e-6)};
    for (std::size_t index = 0; index < results.size(); ++index) {
        if (!results[index].passed) {
            return {false, std::string(labels[index]) + ": " + results[index].message};
        }
    }
    if (actual.seed != seed || actual.stepCount != 0U) {
        return {false, "seed or initial step count did not match the known case"};
    }
    return {true, "known case matched"};
}

}  // namespace course::interlude
