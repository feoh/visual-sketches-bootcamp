#include "color_trail_model.h"
#include "trail_design.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

bool near(float first, float second, float tolerance = 0.00001f) {
    return std::isfinite(first) && std::isfinite(second) &&
           std::fabs(first - second) <= tolerance;
}

void expectNear(float actual, float expected, const std::string& message,
                float tolerance = 0.00001f) {
    if (!near(actual, expected, tolerance)) {
        ++failures;
        std::cerr << "FAIL: " << message << " actual=" << actual
                  << " expected=" << expected << '\n';
    }
}

bool sameColor(colortrail::Color first, colortrail::Color second,
               float tolerance = 0.00001f) {
    return near(first.r, second.r, tolerance) &&
           near(first.g, second.g, tolerance) &&
           near(first.b, second.b, tolerance) &&
           near(first.a, second.a, tolerance);
}

colortrail::Design known() {
    return {
        5, 0.8f, 0.75f, 2.0f, 10.0f,
        {0.05f, 0.06f, 0.07f, 1.0f},
        {
            {{0.1f, 0.2f, 0.3f, 1.0f}, {0.9f, 0.6f, 0.3f, 0.5f}},
            {{0.0f, 0.2f, 1.0f, 1.0f}, {1.0f, 0.8f, 0.0f, 1.0f}}
        }
    };
}

struct OracleRow {
    std::string name;
    std::string kind;
    std::vector<std::string> fields;
};

std::vector<OracleRow> readOracle(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "trail oracle opens");
    std::vector<OracleRow> rows;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream stream(line);
        OracleRow row;
        expect(static_cast<bool>(stream >> row.name >> row.kind),
               "oracle row has name and kind");
        std::string field;
        while (stream >> field) row.fields.push_back(field);
        rows.push_back(row);
    }
    return rows;
}

void oracleCases(const std::vector<OracleRow>& rows) {
    expect(rows.size() == 5, "oracle has five independent rows");
    const colortrail::Palette palette{{0.1f, 0.2f, 0.3f, 1.0f},
                                      {0.9f, 0.6f, 0.3f, 0.5f}};
    for (const auto& row : rows) {
        if (row.kind == "palette") {
            expect(row.fields.size() == 5, row.name + " has palette fields");
            if (row.fields.size() == 5) {
                colortrail::Color result{};
                expect(colortrail::paletteColor(palette, std::stof(row.fields[0]),
                                                result),
                       row.name + " evaluates");
                expectNear(result.r, std::stof(row.fields[1]), row.name + " red");
                expectNear(result.g, std::stof(row.fields[2]), row.name + " green");
                expectNear(result.b, std::stof(row.fields[3]), row.name + " blue");
                expectNear(result.a, std::stof(row.fields[4]), row.name + " alpha");
            }
        } else if (row.kind == "over") {
            expect(row.fields.size() == 12, row.name + " has over fields");
            if (row.fields.size() == 12) {
                colortrail::Color foreground{std::stof(row.fields[0]),
                                             std::stof(row.fields[1]),
                                             std::stof(row.fields[2]),
                                             std::stof(row.fields[3])};
                colortrail::Color background{std::stof(row.fields[4]),
                                             std::stof(row.fields[5]),
                                             std::stof(row.fields[6]),
                                             std::stof(row.fields[7])};
                colortrail::Color result{};
                expect(colortrail::sourceOver(foreground, background, result),
                       row.name + " composites");
                for (int channel = 0; channel < 4; ++channel) {
                    const float actual[] = {result.r, result.g, result.b, result.a};
                    expectNear(actual[channel], std::stof(row.fields[8 + channel]),
                               row.name + " channel " + std::to_string(channel));
                }
            }
        } else if (row.kind == "decay") {
            expect(row.fields.size() == 4, row.name + " has decay fields");
            if (row.fields.size() == 4) {
                float result = -1.0f;
                expect(colortrail::decayAlpha(
                           std::stof(row.fields[0]), std::stof(row.fields[1]),
                           static_cast<std::size_t>(std::stoul(row.fields[2])), result),
                       row.name + " decays");
                expectNear(result, std::stof(row.fields[3]), row.name + " result");
            }
        } else {
            expect(false, row.name + " has known oracle kind");
        }
    }
}

void paletteEndpointBoundaryAndPropertyCases() {
    const auto design = known();
    for (const auto& palette : design.palettes) {
        colortrail::Color output{};
        expect(colortrail::paletteColor(palette, 0.0f, output) &&
                   sameColor(output, palette.first),
               "palette zero is exact first endpoint");
        expect(colortrail::paletteColor(palette, 1.0f, output) &&
                   sameColor(output, palette.second),
               "palette one is exact second endpoint");
        for (int step = 0; step <= 100; ++step) {
            const float amount = static_cast<float>(step) / 100.0f;
            expect(colortrail::paletteColor(palette, amount, output),
                   "in-range palette sample succeeds");
            expect(colortrail::colorIsValid(output),
                   "every interpolated channel stays in bounds");
            expect(output.r >= std::min(palette.first.r, palette.second.r) &&
                       output.r <= std::max(palette.first.r, palette.second.r) &&
                       output.g >= std::min(palette.first.g, palette.second.g) &&
                       output.g <= std::max(palette.first.g, palette.second.g) &&
                       output.b >= std::min(palette.first.b, palette.second.b) &&
                       output.b <= std::max(palette.first.b, palette.second.b),
                   "palette channels stay between their endpoints");
        }
    }
    colortrail::Color sentinel{0.2f, 0.3f, 0.4f, 0.5f};
    const auto before = sentinel;
    expect(!colortrail::paletteColor(design.palettes[0], -0.001f, sentinel) &&
               sameColor(sentinel, before),
           "negative amount rejects without output mutation");
    expect(!colortrail::paletteColor(design.palettes[0], 1.001f, sentinel) &&
               sameColor(sentinel, before),
           "amount above one rejects without output mutation");
    expect(!colortrail::paletteColor(
               design.palettes[0], std::numeric_limits<float>::quiet_NaN(), sentinel),
           "NaN palette amount rejects");
}

void alphaCompositionCases() {
    colortrail::Color output{};
    const colortrail::Color red{1, 0, 0, 0.5f};
    const colortrail::Color blue{0, 0, 1, 1};
    expect(colortrail::sourceOver(red, blue, output),
           "half red over opaque blue composites");
    expect(sameColor(output, {0.5f, 0.0f, 0.5f, 1.0f}),
           "half red over blue is opaque purple");
    expect(colortrail::sourceOver({1, 1, 0, 0}, blue, output) &&
               sameColor(output, blue),
           "transparent foreground leaves background");
    expect(colortrail::sourceOver({1, 0, 0, 1}, blue, output) &&
               sameColor(output, {1, 0, 0, 1}),
           "opaque foreground replaces background");
    expect(colortrail::sourceOver({0.7f, 0.2f, 0.9f, 0},
                                  {0.1f, 0.5f, 0.3f, 0}, output) &&
               sameColor(output, {0, 0, 0, 0}),
           "fully transparent composition has canonical zero channels");
    for (int foreground_alpha = 0; foreground_alpha <= 10; ++foreground_alpha) {
        for (int background_alpha = 0; background_alpha <= 10; ++background_alpha) {
            const float fa = foreground_alpha / 10.0f;
            const float ba = background_alpha / 10.0f;
            expect(colortrail::sourceOver({0.8f, 0.1f, 0.4f, fa},
                                          {0.2f, 0.9f, 0.3f, ba}, output) &&
                       colortrail::colorIsValid(output),
                   "source-over property keeps all channels bounded");
            expectNear(output.a, fa + ba * (1.0f - fa),
                       "source-over alpha follows symbolic equation");
        }
    }
}

void decayCases() {
    float previous = 1.0f;
    for (std::size_t age = 0; age < 80; ++age) {
        float output = -1.0f;
        expect(colortrail::decayAlpha(0.75f, 0.9f, age, output),
               "finite decay evaluates");
        expect(output <= previous && output >= 0.0f,
               "decay is monotonically non-increasing and bounded");
        if (age == 0) expectNear(output, 0.75f, "age zero keeps initial alpha");
        previous = output;
    }
    float output = -1.0f;
    expect(colortrail::decayAlpha(0.6f, 1.0f, 1000000, output),
           "retention one accepts large age");
    expectNear(output, 0.6f, "retention one never decays");
    expect(colortrail::decayAlpha(0.6f, 0.0f, 0, output),
           "zero retention at age zero is defined");
    expectNear(output, 0.6f, "zero retention age zero keeps initial alpha");
    expect(colortrail::decayAlpha(0.6f, 0.0f, 1, output),
           "zero retention after one step is defined");
    expectNear(output, 0.0f, "zero retention clears after one step");
    output = 0.42f;
    expect(!colortrail::decayAlpha(-0.1f, 0.9f, 2, output) && near(output, 0.42f),
           "invalid initial alpha rejects transactionally");
}

bool sameMarks(const std::vector<colortrail::TrailMark>& first,
               const std::vector<colortrail::TrailMark>& second) {
    if (first.size() != second.size()) return false;
    for (std::size_t index = 0; index < first.size(); ++index) {
        if (!near(first[index].position.x, second[index].position.x) ||
            !near(first[index].position.y, second[index].position.y) ||
            !sameColor(first[index].color, second[index].color) ||
            !near(first[index].radius, second[index].radius) ||
            first[index].age_steps != second[index].age_steps ||
            first[index].palette_index != second[index].palette_index) return false;
    }
    return true;
}

void trailKnownBoundaryAndStateIndependentCases() {
    const auto design = known();
    const std::vector<colortrail::TrailSample> history{
        {{10, 20}, 0.0f, 0}, {{20, 30}, 0.5f, 1}, {{30, 40}, 1.0f, 0}};
    std::vector<colortrail::TrailMark> first;
    expect(colortrail::buildTrailMarks(history, design, first),
           "known trail builds");
    expect(first.size() == 3 && first[0].age_steps == 2 &&
               first[1].age_steps == 1 && first[2].age_steps == 0,
           "mark ages are oldest to newest");
    expectNear(first.front().radius, design.minimum_radius,
               "oldest mark has minimum radius");
    expectNear(first.back().radius, design.maximum_radius,
               "newest mark has maximum radius");
    expectNear(first[0].color.a, 0.75f * 0.8f * 0.8f,
               "oldest alpha has two symbolic decay steps");
    expectNear(first[2].color.a, 0.5f * 0.75f,
               "newest endpoint alpha has no age decay");
    expect(first[0].palette_index == 0 && first[1].palette_index == 1,
           "two palette identities survive into render marks");
    expect(colortrail::marksAreFinite(first, design),
           "known marks satisfy finite and bound contract");

    std::vector<colortrail::TrailMark> second{{{999, 999}, {1, 1, 1, 1}, 2, 0, 0}};
    expect(colortrail::buildTrailMarks(history, design, second),
           "same explicit inputs rebuild after unrelated output state");
    expect(sameMarks(first, second),
           "render-plan output is state-independent and exactly replayable");
    std::vector<colortrail::TrailMark> unrelated;
    expect(colortrail::buildTrailMarks({{{-5, 8}, 0.25f, 1}}, design, unrelated),
           "interleaved unrelated build succeeds");
    std::vector<colortrail::TrailMark> replay;
    expect(colortrail::buildTrailMarks(history, design, replay) &&
               sameMarks(first, replay),
           "call order and unrelated model output do not affect replay");

    std::vector<colortrail::TrailMark> empty{{{1, 1}, {1, 1, 1, 1}, 2, 0, 0}};
    expect(colortrail::buildTrailMarks({}, design, empty) && empty.empty(),
           "empty history produces an empty plan");
}

void appendBoundsAndInvalidFiniteCases() {
    std::vector<colortrail::TrailSample> history;
    expect(colortrail::appendSample(history, {{1, 2}, 0.0f, 0}, 3) &&
               colortrail::appendSample(history, {{2, 3}, 0.5f, 1}, 3) &&
               colortrail::appendSample(history, {{3, 4}, 1.0f, 0}, 3) &&
               colortrail::appendSample(history, {{4, 5}, 0.25f, 1}, 3),
           "bounded append accepts valid samples");
    expect(history.size() == 3 && near(history.front().position.x, 2.0f) &&
               near(history.back().position.x, 4.0f),
           "bounded append drops exactly the oldest sample");
    const auto before = history;
    expect(!colortrail::appendSample(
               history, {{std::numeric_limits<float>::infinity(), 0}, 0.5f, 0}, 3),
           "infinite position rejects");
    expect(history.size() == before.size() &&
               near(history.front().position.x, before.front().position.x),
           "invalid append preserves history");
    expect(!colortrail::appendSample(history, {{1, 2}, 0.5f, 2}, 3),
           "third palette index rejects");
    expect(!colortrail::appendSample(history, {{1, 2}, 0.5f, 0}, 1),
           "history cap below two rejects");

    auto design = known();
    std::vector<colortrail::TrailMark> output;
    expect(colortrail::buildTrailMarks(history, design, output),
           "valid history still builds after invalid append");
    const auto output_before = output;
    auto invalid_history = history;
    invalid_history[1].palette_amount = std::numeric_limits<float>::quiet_NaN();
    expect(!colortrail::buildTrailMarks(invalid_history, design, output) &&
               sameMarks(output, output_before),
           "NaN history rejects without output mutation");
    design.palettes[0].first.r = std::numeric_limits<float>::infinity();
    expect(!colortrail::buildTrailMarks(history, design, output) &&
               sameMarks(output, output_before),
           "infinite design channel rejects transactionally");
    design = known();
    design.maximum_history = 2;
    expect(!colortrail::buildTrailMarks(history, design, output),
           "history larger than design cap rejects");

    auto learner = makeTrailDesign();
    expect(colortrail::designIsValid(learner),
           "learner design seam accepts every technically valid aesthetic choice");
}

}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "usage supplies trail oracle");
    if (argc == 2) oracleCases(readOracle(argv[1]));
    paletteEndpointBoundaryAndPropertyCases();
    alphaCompositionCases();
    decayCases();
    trailKnownBoundaryAndStateIndependentCases();
    appendBoundsAndInvalidFiniteCases();
    if (failures != 0) {
        std::cerr << "color_trail_model_test: " << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "color_trail_model_test: independent palette/over/decay oracle, exact endpoints, bounded channels, source-over boundaries/properties, monotone decay, two-palette trail ages/radii, state-independent replay, history caps, finite rejection, transactional output, and learner design validity passed\n";
    return 0;
}
