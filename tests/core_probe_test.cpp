#include "course_probe.h"
#include "deterministic_fixture.h"
#include "expect_near.h"

#include <cassert>
#include <iostream>
#include <limits>

int main() {
    const course::test::DeterministicFixture fixture;
    auto a = fixture.freshModel();
    auto b = fixture.freshModel();
    assert(a.position.x == b.position.x && a.velocity.y == b.velocity.y);
    for (int i = 0; i < 120; ++i) {
        course::update(a, fixture.input, fixture.dt, fixture.viewport);
        course::update(b, fixture.input, fixture.dt, fixture.viewport);
    }
    assert(a.position.x == b.position.x && a.position.y == b.position.y);
    assert(a.stepCount == 120);
    const auto nearResult = course::test::expectNear(1000000.1, 1000000.0, 0.0, 1e-6);
    assert(nearResult.passed);
    const auto fail = course::test::expectNear(1.0, 2.0, 0.01, 0.0);
    assert(!fail.passed && fail.message.find("actual=1") != std::string::npos);

    const auto invalidDtLeavesState = [&fixture](float dt) {
        auto before = fixture.freshModel();
        auto after = before;
        course::update(after, fixture.input, dt, fixture.viewport);
        return after.position.x == before.position.x &&
               after.position.y == before.position.y &&
               after.velocity.x == before.velocity.x &&
               after.velocity.y == before.velocity.y && after.stepCount == 1;
    };
    assert(invalidDtLeavesState(std::numeric_limits<float>::quiet_NaN()));
    assert(invalidDtLeavesState(std::numeric_limits<float>::infinity()));
    assert(invalidDtLeavesState(-fixture.dt));
    assert(invalidDtLeavesState(0.0f));
    std::cout << "core_probe_test: deterministic replay, dt sanitization, and tolerance diagnostics passed\n";
}
