#include "learner_known_case.h"

#include "expect_near.h"

#include <array>

namespace course::interlude {

TestResult learnerKnownCaseTest(ModelFactory makeModelUnderTest) {
    // Explained alternative: a different reviewed oracle proves that a learner
    // need not copy the starter case or discover this case from public tests.
    const std::uint32_t seed = 9001U;
    const Viewport viewport{640.0f, 480.0f};
    const Vec2 expectedPosition{341.9251099f, 355.7872925f};
    const Vec2 expectedVelocity{-29.78694916f, 21.36769295f};
    const Model actual = makeModelUnderTest(seed, viewport);

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
