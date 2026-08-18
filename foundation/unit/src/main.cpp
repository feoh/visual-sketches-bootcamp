#include "course_probe.h"
#include "deterministic_fixture.h"
#include "expect_near.h"
#include "ofAppNoWindow.h"
#include "ofMain.h"
#include "ofxUnitTests.h"

#include <cmath>
#include <limits>
#include <memory>
#include <string>

class FoundationTests final : public ofxUnitTestsApp {
    void run() override {
        const course::test::DeterministicFixture fixture;

        const auto first = fixture.freshModel();
        const auto replay = fixture.freshModel();
        auto near = course::test::expectNear(first.position.x, replay.position.x, 0.0, 0.0);
        ofxTest(near.passed, "same seed reproduces initial x", near.message);

        const auto other = course::makeModel(fixture.seed + 1U, fixture.viewport);
        ofxTest(first.position.x != other.position.x || first.position.y != other.position.y,
                "different seed changes initial state");

        ofxTest(first.position.x >= 0.0f && first.position.x < fixture.viewport.width &&
                    first.position.y >= 0.0f && first.position.y < fixture.viewport.height,
                "seeded state stays inside fixed viewport");

        auto stepped = first;
        course::update(stepped, fixture.input, fixture.dt, fixture.viewport);
        ofxTestEq(stepped.stepCount, static_cast<std::uint64_t>(1),
                  "explicit fixed dt advances one step");

        auto runA = fixture.freshModel();
        auto runB = fixture.freshModel();
        for (int index = 0; index < 120; ++index) {
            course::update(runA, fixture.input, fixture.dt, fixture.viewport);
            course::update(runB, fixture.input, fixture.dt, fixture.viewport);
        }
        near = course::test::expectNear(runA.position.y, runB.position.y, 0.0, 0.0);
        ofxTest(near.passed, "fixed input replay is deterministic", near.message);

        course::Model wrapping;
        wrapping.position = {639.0f, 1.0f};
        wrapping.velocity = {120.0f, -120.0f};
        course::update(wrapping, {{0.0f, 0.0f}}, fixture.dt, fixture.viewport);
        ofxTest(wrapping.position.x >= 0.0f && wrapping.position.x < fixture.viewport.width &&
                    wrapping.position.y >= 0.0f && wrapping.position.y < fixture.viewport.height,
                "viewport wrap handles both edges");

        course::Model clamped;
        clamped.position = {10.0f, 10.0f};
        clamped.velocity = {4.0f, 0.0f};
        course::update(clamped, {{0.0f, 0.0f}}, 10.0f, fixture.viewport);
        near = course::test::expectNear(clamped.position.x, 11.0, 1e-6, 1e-6);
        ofxTest(near.passed, "extreme dt is clamped to 0.25 seconds", near.message);

        const auto invalidDtLeavesState = [&fixture](float dt) {
            auto before = fixture.freshModel();
            auto after = before;
            course::update(after, fixture.input, dt, fixture.viewport);
            return after.position.x == before.position.x &&
                   after.position.y == before.position.y &&
                   after.velocity.x == before.velocity.x &&
                   after.velocity.y == before.velocity.y && after.stepCount == 1;
        };
        ofxTest(invalidDtLeavesState(std::numeric_limits<float>::quiet_NaN()),
                "NaN dt leaves deterministic state unchanged");
        ofxTest(invalidDtLeavesState(std::numeric_limits<float>::infinity()),
                "positive infinite dt leaves deterministic state unchanged");
        ofxTest(invalidDtLeavesState(-fixture.dt),
                "negative dt leaves deterministic state unchanged");
        ofxTest(invalidDtLeavesState(0.0f), "zero dt leaves deterministic state unchanged");

        const auto intentionalMismatch = course::test::expectNear(1.0, 2.0, 0.01, 0.0);
        ofxTest(!intentionalMismatch.passed &&
                    intentionalMismatch.message.find("actual=1") != std::string::npos &&
                    intentionalMismatch.message.find("expected=2") != std::string::npos &&
                    intentionalMismatch.message.find("allowed=") != std::string::npos,
                "near failures contain actionable values", intentionalMismatch.message);
    }
};

int main() {
    ofInit();
    auto window = std::make_shared<ofAppNoWindow>();
    auto app = std::make_shared<FoundationTests>();
    ofRunApp(window, app);
    return ofRunMainLoop();
}
