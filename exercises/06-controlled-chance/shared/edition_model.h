#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace controlledchance {

struct Vec2 { float x; float y; };
struct Color { int r; int g; int b; };
struct Viewport { int width; int height; };
enum class Motif { dot = 0, dash = 1, ring = 2 };

struct Design {
    std::uint32_t seed;
    int marks_per_edition;
    float minimum_radius;
    float maximum_radius;
    Color ink;
    Color accent;
    Color background;
};
struct MarkParameter {
    float x_unit;
    float y_unit;
    float radius;
    Motif motif;
};
struct Edition {
    int index;
    std::vector<MarkParameter> marks;
};
struct EditionSet {
    std::uint32_t seed;
    std::vector<Edition> editions;
};
struct PlacedMark {
    Vec2 center;
    float radius;
    Motif motif;
};
struct Scene {
    bool valid;
    int edition_index;
    std::vector<PlacedMark> marks;
};

constexpr int edition_count = 6;
constexpr float stroke_half_width = 1.5f;
constexpr float outer_margin = 2.0f;

bool designIsValid(const Design& design);
float triangularCoordinate(float first_unit, float second_unit);
Motif weightedMotif(int ticket);
EditionSet generateEditions(const Design& design);
std::string serializeEditions(const EditionSet& set);
bool parseEditions(const std::string& text, EditionSet& set, std::string& error);
bool editionSetIsValid(const EditionSet& set, int expected_marks_per_edition);
Scene makeScene(const EditionSet& set, int edition_index, Viewport viewport);
bool sceneIsFiniteAndInBounds(const Scene& scene, Viewport viewport);

}  // namespace controlledchance
