#include "signature_design.h"
#include "signature_geometry.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

namespace {

int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

bool approximately(float actual, float expected, float tolerance = 0.0001f) {
    return std::fabs(actual - expected) <=
           tolerance * std::max({1.0f, std::fabs(actual), std::fabs(expected)});
}

bool unitValue(float value) {
    return std::isfinite(value) && value >= 0.0f && value <= 1.0f;
}

bool validKind(signature::PrimitiveKind kind) {
    using signature::PrimitiveKind;
    switch (kind) {
        case PrimitiveKind::circle:
        case PrimitiveKind::rectangle:
        case PrimitiveKind::triangle:
        case PrimitiveKind::line:
        case PrimitiveKind::ellipse:
            return true;
    }
    return false;
}

signature::SignatureSpecs knownSpecs() {
    using signature::PrimitiveKind;
    return {{{PrimitiveKind::circle, {0.25f, 0.50f}, {0.10f, 0.20f}, 0U},
             {PrimitiveKind::rectangle, {0.50f, 0.50f}, {0.10f, 0.10f}, 1U},
             {PrimitiveKind::triangle, {0.75f, 0.50f}, {0.10f, 0.20f}, 2U},
             {PrimitiveKind::line, {0.50f, 0.25f}, {0.25f, 0.05f}, 0U},
             {PrimitiveKind::ellipse, {0.50f, 0.75f}, {0.20f, 0.10f}, 2U}}};
}

void knownHelperCase() {
    const signature::Signature result = signature::buildSignature(knownSpecs(), {800.0f, 600.0f});
    expect(result.size() == 5U, "helper signature has exactly five primitives");
    expect(approximately(result[0].center.x, 200.0f) &&
               approximately(result[0].center.y, 300.0f),
           "normalized center maps to pixels");
    expect(approximately(result[0].half_width, 40.0f) &&
               approximately(result[0].half_height, 60.0f),
           "normalized full size maps to pixel half-extents");
    expect(result[3].color_index == 0U && result[4].color_index == 2U,
           "valid color indices are retained without wrapping");
}

void boundaryHelperCase() {
    signature::SignatureSpecs specs = knownSpecs();
    specs[0].normalized_center = {-2.0f, 4.0f};
    specs[0].normalized_size = {2.0f, -1.0f};
    specs[1].normalized_center.x = std::numeric_limits<float>::quiet_NaN();
    specs[1].normalized_size.y = std::numeric_limits<float>::infinity();
    const signature::Signature result = signature::buildSignature(specs, {0.0f, -20.0f});
    expect(signature::signatureIsInBounds(result, {0.0f, -20.0f}),
           "helper sanitizes non-finite and out-of-range geometry into a one-pixel viewport");

    specs[0].color_index = 3U;
    const signature::Signature invalid = signature::buildSignature(specs, {800.0f, 600.0f});
    expect(invalid[0].color_index == 3U,
           "helper preserves an invalid palette index instead of hiding it with modulo wrapping");
    expect(!signature::signatureIsInBounds(invalid, {800.0f, 600.0f}),
           "helper reports an invalid palette index");
}

void learnerDesignContract() {
    const signature::Palette palette = makePalette();
    const signature::SignatureSpecs specs = makeSignatureSpecs();
    expect(palette.size() == 3U, "learner design exposes exactly three palette colors");
    expect(specs.size() == 5U, "learner design exposes exactly five primitive specifications");

    for (std::size_t index = 0; index < palette.size(); ++index) {
        const signature::Color& color = palette[index];
        expect(color.red >= 0 && color.red <= 255 && color.green >= 0 && color.green <= 255 &&
                   color.blue >= 0 && color.blue <= 255,
               "palette color " + std::to_string(index) + " has RGB channels from 0 through 255");
        for (std::size_t previous = 0; previous < index; ++previous) {
            const signature::Color& other = palette[previous];
            expect(color.red != other.red || color.green != other.green || color.blue != other.blue,
                   "the three learner palette entries are distinct colors");
        }
    }

    bool used_colors[3] = {false, false, false};
    for (std::size_t index = 0; index < specs.size(); ++index) {
        const signature::PrimitiveSpec& spec = specs[index];
        expect(validKind(spec.kind), "spec " + std::to_string(index) + " has a declared primitive kind");
        expect(unitValue(spec.normalized_center.x) && unitValue(spec.normalized_center.y),
               "spec " + std::to_string(index) + " has finite normalized center fields in [0,1]");
        expect(unitValue(spec.normalized_size.x) && unitValue(spec.normalized_size.y) &&
                   spec.normalized_size.x > 0.0f && spec.normalized_size.y > 0.0f,
               "spec " + std::to_string(index) + " has finite positive normalized size fields in (0,1]");
        expect(spec.color_index < palette.size(),
               "spec " + std::to_string(index) + " has a palette index from 0 through 2");
        if (spec.color_index < palette.size()) {
            used_colors[spec.color_index] = true;
        }
    }
    expect(used_colors[0] && used_colors[1] && used_colors[2],
           "the five specifications use all three palette colors");
}

void learnerViewportProperty(const std::string& fixture_path) {
    std::ifstream fixture(fixture_path);
    expect(fixture.good(), "viewport fixture opens");
    std::string line;
    while (std::getline(fixture, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        float width = 0.0f;
        float height = 0.0f;
        std::istringstream row(line);
        row >> width >> height;
        expect(!row.fail(), "fixture row has numeric width and height");
        if (row.fail()) {
            continue;
        }
        const signature::Signature result =
            signature::buildSignature(makeSignatureSpecs(), {width, height});
        expect(result.size() == 5U, "learner design retains exactly five primitives at every fixture");
        expect(signature::signatureIsInBounds(result, {width, height}),
               "learner nominal geometry remains in bounds at every fixture");
        for (const signature::Primitive& primitive : result) {
            expect(std::isfinite(primitive.center.x) && std::isfinite(primitive.center.y) &&
                       std::isfinite(primitive.half_width) && std::isfinite(primitive.half_height),
                   "all built learner geometry fields are finite");
            expect(validKind(primitive.kind), "built learner geometry retains a declared kind");
            expect(primitive.color_index < 3U, "built learner geometry retains a valid palette index");
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    const std::string fixture = argc == 2 ? argv[1] : "exercises/00-visual-signature/fixtures/viewports.tsv";
    knownHelperCase();
    boundaryHelperCase();
    learnerDesignContract();
    learnerViewportProperty(fixture);
    if (failures != 0) {
        std::cerr << failures << " section 00 checks failed\n";
        return 1;
    }
    std::cout << "signature_geometry_test: helper cases and learner design contract passed\n";
    return 0;
}
