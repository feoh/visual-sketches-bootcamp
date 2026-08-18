#include "poster_design.h"
#include "poster_layout.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
struct Case {
    std::string name;
    responsiveposter::Viewport viewport;
    responsiveposter::Rect panel;
    float headline;
    float radius;
    responsiveposter::Vec2 focus;
};
int failures = 0;
void expect(bool condition, const std::string& message) {
    if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
bool near(float actual, float expected, float abs_tolerance = 0.002f,
          float rel_tolerance = 0.000001f) {
    const float difference = std::fabs(actual - expected);
    const float allowed = std::max(abs_tolerance, rel_tolerance * std::fabs(expected));
    return std::isfinite(actual) && std::isfinite(expected) && difference <= allowed;
}
void expectNear(float actual, float expected, const std::string& label) {
    if (!near(actual, expected)) {
        ++failures;
        std::cerr << "FAIL: " << label << " actual=" << actual << " expected=" << expected
                  << " difference=" << std::fabs(actual - expected)
                  << " absTolerance=0.002 relTolerance=0.000001\n";
    }
}
responsiveposter::Design knownDesign() {
    return {0.25f, 0.65f, {20, 30, 40}, {210, 80, 90}, {240, 230, 210}};
}
std::vector<Case> readCases(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "layout fixture opens");
    std::vector<Case> cases;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream fields(line);
        Case fixture{};
        std::string extra;
        const bool parsed = static_cast<bool>(
            fields >> fixture.name >> fixture.viewport.width >> fixture.viewport.height >>
            fixture.panel.x >> fixture.panel.y >> fixture.panel.width >> fixture.panel.height >>
            fixture.headline >> fixture.radius >> fixture.focus.x >> fixture.focus.y);
        const bool exact = parsed && !(fields >> extra);
        expect(exact, "each fixture row has exactly eleven fields");
        if (exact) cases.push_back(fixture);
    }
    return cases;
}
void helperCases() {
    using namespace responsiveposter;
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float infinity = std::numeric_limits<float>::infinity();
    expectNear(clamp01(-0.25f), 0.0f, "clamp01 clamps below zero");
    expectNear(clamp01(1.25f), 1.0f, "clamp01 clamps above one");
    expectNear(normalizeClamped(10.0f, 10.0f, 30.0f), 0.0f, "normalize lower endpoint");
    expectNear(normalizeClamped(20.0f, 10.0f, 30.0f), 0.5f, "normalize midpoint");
    expectNear(normalizeClamped(30.0f, 10.0f, 30.0f), 1.0f, "normalize upper endpoint");
    expectNear(normalizeClamped(5.0f, 10.0f, 30.0f), 0.0f, "normalize clamps below range");
    expectNear(normalizeClamped(35.0f, 10.0f, 30.0f), 1.0f, "normalize clamps above range");
    expectNear(lerpClamped(100.0f, 200.0f, 0.0f), 100.0f, "lerp lower endpoint");
    expectNear(lerpClamped(100.0f, 200.0f, 0.5f), 150.0f, "lerp midpoint");
    expectNear(lerpClamped(100.0f, 200.0f, 1.0f), 200.0f, "lerp upper endpoint");
    expectNear(lerpClamped(100.0f, 200.0f, -0.5f), 100.0f, "lerp clamps below range");
    expectNear(lerpClamped(100.0f, 200.0f, 1.5f), 200.0f, "lerp clamps above range");
    expectNear(lerpClamped(200.0f, 100.0f, 0.25f), 175.0f,
               "lerp permits descending destination endpoints");
    expectNear(mapClamped(15.0f, 10.0f, 20.0f, -1.0f, 1.0f), 0.0f,
               "map performs inverse normalization then interpolation");
    expectNear(mapClamped(5.0f, 0.0f, 10.0f, 80.0f, 40.0f), 60.0f,
               "map preserves descending destination endpoints");
    expectNear(mapClamped(-5.0f, 0.0f, 10.0f, 40.0f, 80.0f), 40.0f, "map clamps below range");
    expectNear(mapClamped(15.0f, 0.0f, 10.0f, 40.0f, 80.0f), 80.0f, "map clamps above range");
    expectNear(smoothstep(0.0f), 0.0f, "smoothstep lower endpoint");
    expectNear(smoothstep(0.5f), 0.5f, "smoothstep midpoint");
    expectNear(smoothstep(1.0f), 1.0f, "smoothstep upper endpoint");
    float previous = -1.0f;
    for (int index = 0; index <= 100; ++index) {
        const float value = smoothstep(static_cast<float>(index) / 100.0f);
        expect(value + 0.000001f >= previous, "smoothstep is monotonic at sample " + std::to_string(index));
        previous = value;
    }
    expectNear(normalizeClamped(4.0f, 2.0f, 2.0f), 0.0f,
               "equal source endpoints are deterministic");
    expectNear(normalizeClamped(4.0f, 6.0f, 2.0f), 0.0f,
               "descending source range is invalid and deterministic");
    expectNear(mapClamped(4.0f, 6.0f, 2.0f, 40.0f, 80.0f), 40.0f,
               "map with descending source returns destination start");
    expectNear(clamp01(nan), 0.0f, "NaN clamp input is deterministic");
    expectNear(clamp01(infinity), 0.0f, "infinite clamp input is deterministic");
    expectNear(normalizeClamped(infinity, 0.0f, 1.0f), 0.0f,
               "non-finite normalize value is deterministic");
    expectNear(normalizeClamped(0.5f, -infinity, 1.0f), 0.0f,
               "non-finite normalize source endpoint is deterministic");
    expectNear(lerpClamped(infinity, 2.0f, 0.5f), 0.0f,
               "non-finite lerp endpoint is deterministic");
    expectNear(lerpClamped(1.0f, 2.0f, nan), 1.0f,
               "non-finite lerp amount selects destination start");
    expectNear(smoothstep(infinity), 0.0f, "non-finite smoothstep amount is deterministic");
}
void fixtureCases(const std::vector<Case>& cases) {
    for (const Case& expected : cases) {
        const auto actual = responsiveposter::makeLayout(knownDesign(), expected.viewport);
        expect(actual.valid, expected.name + " fixture is valid");
        expect(responsiveposter::layoutIsFiniteAndInBounds(actual, expected.viewport),
               expected.name + " fixture is finite and in bounds");
        expectNear(actual.panel.x, expected.panel.x, expected.name + " panel.x oracle");
        expectNear(actual.panel.y, expected.panel.y, expected.name + " panel.y oracle");
        expectNear(actual.panel.width, expected.panel.width, expected.name + " panel.width oracle");
        expectNear(actual.panel.height, expected.panel.height, expected.name + " panel.height oracle");
        expectNear(actual.headline_size, expected.headline, expected.name + " headline oracle");
        expectNear(actual.focus_radius, expected.radius, expected.name + " radius oracle");
        expectNear(actual.focus_center.x, expected.focus.x, expected.name + " focus.x oracle");
        expectNear(actual.focus_center.y, expected.focus.y, expected.name + " focus.y oracle");
        expectNear(actual.panel.width / actual.panel.height, 0.8f,
                   expected.name + " preserves four-to-five panel aspect");
    }
}
void endpointAndResizeCases() {
    auto design = knownDesign();
    design.focus = 0.0f;
    const auto left = responsiveposter::makeLayout(design, {800, 600});
    expectNear(left.focus_center.x, left.panel.x + left.focus_radius, "focus zero reaches left safe endpoint");
    design.focus = 1.0f;
    const auto right = responsiveposter::makeLayout(design, {800, 600});
    expectNear(right.focus_center.x, right.panel.x + right.panel.width - right.focus_radius,
               "focus one reaches right safe endpoint");
    design.focus = 0.5f;
    const auto middle = responsiveposter::makeLayout(design, {800, 600});
    expectNear(middle.focus_center.x, middle.panel.x + middle.panel.width * 0.5f,
               "focus midpoint reaches panel midpoint");

    const auto portrait = responsiveposter::makeLayout(knownDesign(), {600, 900});
    const auto landscape = responsiveposter::makeLayout(knownDesign(), {900, 600});
    expect(portrait.valid && landscape.valid, "portrait and landscape resize layouts are valid");
    expect(!near(portrait.panel.x, landscape.panel.x) || !near(portrait.panel.y, landscape.panel.y),
           "resize recomputes responsive placement");
    expectNear(portrait.panel.width / portrait.panel.height, 0.8f, "portrait aspect remains four-to-five");
    expectNear(landscape.panel.width / landscape.panel.height, 0.8f, "landscape aspect remains four-to-five");

    for (const auto viewport : {responsiveposter::Viewport{64, 64}, {320, 1200}, {1200, 320},
                                {800, 600}, {1600, 900}}) {
        const auto layout = responsiveposter::makeLayout(knownDesign(), viewport);
        expect(responsiveposter::layoutIsFiniteAndInBounds(layout, viewport),
               "valid resize remains finite and in bounds at " + std::to_string(viewport.width) + "x" +
                   std::to_string(viewport.height));
    }
}
void orbitContainmentCase() {
    const responsiveposter::Design solution_design = {
        0.72f, 0.35f, {235, 242, 239}, {79, 214, 190}, {31, 44, 55}};
    const auto layout = responsiveposter::makeLayout(solution_design, {64, 64});
    expect(responsiveposter::layoutIsFiniteAndInBounds(layout, {64, 64}),
           "solution design has a valid minimum-size layout");
    constexpr float ring_count = 3.0f;
    constexpr float orbit_stroke_half_width = 2.5f;
    const float available_distance = responsiveposter::availableFocusDistance(layout);
    const float drawable_distance = std::max(0.0f, available_distance - orbit_stroke_half_width);
    const float outer_radius = std::min(layout.focus_radius, drawable_distance / ring_count) * ring_count;
    const float rendered_extent = outer_radius + orbit_stroke_half_width;
    expect(rendered_extent <= available_distance + 0.000001f,
           "responsive outer orbit includes stroke width at 64x64");
    expect(layout.focus_center.x - rendered_extent >= layout.panel.x - 0.000001f &&
               layout.focus_center.x + rendered_extent <=
                   layout.panel.x + layout.panel.width + 0.000001f &&
               layout.focus_center.y - rendered_extent >= layout.panel.y - 0.000001f &&
               layout.focus_center.y + rendered_extent <=
                   layout.panel.y + layout.panel.height + 0.000001f,
           "solution rendered outer orbit remains contained by the minimum-size panel");
    expectNear(responsiveposter::availableFocusDistance({}), 0.0f,
               "invalid layout has no available focus distance");
}
void invalidCases() {
    for (const auto viewport : {responsiveposter::Viewport{0, 400}, {1, 400}, {63, 400},
                                {400, 0}, {400, 1}, {400, 63}, {-1, 400}}) {
        const auto layout = responsiveposter::makeLayout(knownDesign(), viewport);
        expect(!layout.valid, "tiny or invalid viewport returns invalid inspectable layout");
        expect(!responsiveposter::layoutIsFiniteAndInBounds(layout, viewport),
               "invalid layout is not reported in bounds");
    }
    auto design = knownDesign();
    design.focus = 1.1f;
    expect(!responsiveposter::makeLayout(design, {800, 600}).valid,
           "out-of-range design returns invalid layout");
}
void learnerContract() {
    const auto design = makePosterDesign();
    expect(responsiveposter::designIsValid(design),
           "learner owns valid focus, vertical bias, and contrasting palette values");
    const auto first = responsiveposter::makeLayout(design, {960, 640});
    const auto second = responsiveposter::makeLayout(design, {960, 640});
    expect(responsiveposter::layoutIsFiniteAndInBounds(first, {960, 640}),
           "learner layout is finite and bounded");
    expectNear(first.panel.x, second.panel.x, "learner layout repeats deterministically");
    expectNear(first.focus_center.x, second.focus_center.x, "learner focus repeats deterministically");
}
}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "fixture path is provided");
    const auto cases = argc == 2 ? readCases(argv[1]) : std::vector<Case>{};
    expect(cases.size() == 3, "fixture retains three explicit parsed scenarios");
    helperCases();
    fixtureCases(cases);
    endpointAndResizeCases();
    orbitContainmentCase();
    invalidCases();
    learnerContract();
    if (failures) { std::cerr << failures << " section 03 checks failed\n"; return 1; }
    std::cout << "poster_layout_test: helpers, clamp and non-finite policies, parsed oracles, endpoints, midpoint, monotonicity, resize, aspect, orbit containment, bounds, invalid viewports, and learner contract passed\n";
}
