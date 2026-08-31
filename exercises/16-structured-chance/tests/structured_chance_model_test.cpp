#include "structured_chance_design.h"
#include "structured_chance_model.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {
void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "section 16 test failed: " << message << '\n';
        std::exit(1);
    }
}
bool near(float a, float b, float tolerance = 0.00001f) {
    return std::fabs(a - b) <= tolerance;
}
bool samePoint(structuredchance::Point a, structuredchance::Point b) {
    return near(a.x, b.x) && near(a.y, b.y);
}
}

int main() {
    auto settings = makeStructuredChanceSettings();
    require(structuredchance::isValid(settings), "starter settings must be valid");
    const auto first = structuredchance::generateComposition(settings);
    const auto replay = structuredchance::generateComposition(settings);
    require(first.regions.size() == static_cast<std::size_t>(settings.rows * settings.columns),
            "the full region grid must be generated");
    require(first.shared_nodes.size() ==
                static_cast<std::size_t>((settings.rows + 1) * (settings.columns + 1)),
            "one shared node lattice must own every panel corner");
    require(first.quiet_region_index >= 0, "one quiet region must be selected");
    require(first.regions.size() == replay.regions.size(), "same seed must replay region count");

    int quiet_count = 0;
    for (std::size_t index = 0; index < first.regions.size(); ++index) {
        const auto& region = first.regions[index];
        const auto& again = replay.regions[index];
        require(region.row == again.row && region.column == again.column &&
                    near(region.angle_radians, again.angle_radians) &&
                    region.strokes.size() == again.strokes.size(),
                "same seed must replay inherited region records");
        require(structuredchance::signedArea(region.quad) > 0.001f,
                "every warped panel must remain convex with positive area");
        require(std::fabs(region.angle_radians - settings.composition_angle) <=
                    settings.region_angle_jitter + 0.121f,
                "region angles must stay related to the composition angle");
        if (region.quiet) {
            ++quiet_count;
            require(region.strokes.size() <= static_cast<std::size_t>(settings.quiet_strokes),
                    "quiet region must preserve negative space");
        } else {
            require(region.strokes.size() >= static_cast<std::size_t>(settings.min_strokes),
                    "active regions must keep their density floor");
        }
        require(region.strokes.size() <= static_cast<std::size_t>(settings.max_strokes),
                "all region work must remain capped");
        for (const auto& stroke : region.strokes) {
            require(stroke.local_a.x >= -0.0001f && stroke.local_a.x <= 1.0001f &&
                        stroke.local_a.y >= -0.0001f && stroke.local_a.y <= 1.0001f &&
                        stroke.local_b.x >= -0.0001f && stroke.local_b.x <= 1.0001f &&
                        stroke.local_b.y >= -0.0001f && stroke.local_b.y <= 1.0001f,
                    "local line clipping must end on the unit panel");
            require(structuredchance::finitePoint(stroke.a) &&
                        structuredchance::finitePoint(stroke.b),
                    "mapped stroke endpoints must stay finite");
        }
    }
    require(quiet_count == 1, "exactly one region must be deliberately quiet");

    for (int row = 0; row < settings.rows; ++row) {
        for (int column = 0; column + 1 < settings.columns; ++column) {
            const auto& left = first.regions[static_cast<std::size_t>(row * settings.columns + column)];
            const auto& right = first.regions[static_cast<std::size_t>(row * settings.columns + column + 1)];
            require(samePoint(left.quad.top_right, right.quad.top_left) &&
                        samePoint(left.quad.bottom_right, right.quad.bottom_left),
                    "horizontal neighbors must share exact seam nodes");
        }
    }

    structuredchance::Point a{};
    structuredchance::Point b{};
    require(structuredchance::clipLineToUnitSquare({0.5f, 0.5f}, {1.0f, 0.0f}, a, b),
            "a horizontal center line must clip");
    require(near(a.x, 0.0f) && near(b.x, 1.0f) && near(a.y, 0.5f) && near(b.y, 0.5f),
            "known horizontal clipping endpoints must match");

    settings.rows = 1;
    require(!structuredchance::isValid(settings), "a one-row composition must be rejected");
    std::cout << "section-16-model: deterministic hierarchy, seams, clipping, and quiet space passed\n";
}
