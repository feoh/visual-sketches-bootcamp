#include "edition_model.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <random>
#include <sstream>

namespace controlledchance {
namespace {
bool finite(float value) { return std::isfinite(value); }
bool colorValid(Color color) {
    return color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 &&
           color.b >= 0 && color.b <= 255;
}
bool colorsDiffer(Color first, Color second) {
    return first.r != second.r || first.g != second.g || first.b != second.b;
}
bool markValid(const MarkParameter& mark) {
    const int motif = static_cast<int>(mark.motif);
    return finite(mark.x_unit) && finite(mark.y_unit) && finite(mark.radius) &&
           mark.x_unit >= 0.0f && mark.x_unit <= 1.0f &&
           mark.y_unit >= 0.0f && mark.y_unit <= 1.0f &&
           mark.radius >= 2.0f && mark.radius <= 8.0f &&
           motif >= static_cast<int>(Motif::dot) && motif <= static_cast<int>(Motif::ring);
}
float mapUnit(float unit, float first, float last) {
    return first + (last - first) * unit;
}
float canonicalFloat(float value) {
    return value == 0.0f ? 0.0f : value;
}
bool parseSeedToken(const std::string& token, std::uint32_t& seed) {
    if (token.empty()) return false;
    for (const char character : token) {
        if (character < '0' || character > '9') return false;
    }
    const char* const begin = token.data();
    const char* const end = begin + token.size();
    const auto result = std::from_chars(begin, end, seed, 10);
    return result.ec == std::errc{} && result.ptr == end;
}
}  // namespace

bool designIsValid(const Design& design) {
    return design.marks_per_edition >= 4 && design.marks_per_edition <= 48 &&
           finite(design.minimum_radius) && finite(design.maximum_radius) &&
           design.minimum_radius >= 2.0f && design.maximum_radius <= 8.0f &&
           design.minimum_radius <= design.maximum_radius &&
           colorValid(design.ink) && colorValid(design.accent) &&
           colorValid(design.background) && colorsDiffer(design.ink, design.background) &&
           colorsDiffer(design.accent, design.background);
}

float triangularCoordinate(float first_unit, float second_unit) {
    if (!finite(first_unit) || !finite(second_unit) || first_unit < 0.0f ||
        first_unit > 1.0f || second_unit < 0.0f || second_unit > 1.0f) return 0.5f;
    return (first_unit + second_unit) * 0.5f;
}

Motif weightedMotif(int ticket) {
    if (ticket >= 0 && ticket < 6) return Motif::dot;
    if (ticket >= 6 && ticket < 9) return Motif::dash;
    return Motif::ring;
}

EditionSet generateEditions(const Design& design) {
    EditionSet set{};
    if (!designIsValid(design)) return set;
    set.seed = design.seed;
    std::mt19937 engine(design.seed);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    std::uniform_real_distribution<float> radius(design.minimum_radius,
                                                 design.maximum_radius);
    std::uniform_int_distribution<int> weighted_ticket(0, 9);
    set.editions.reserve(edition_count);
    for (int edition_index = 0; edition_index < edition_count; ++edition_index) {
        Edition edition{edition_index, {}};
        edition.marks.reserve(static_cast<std::size_t>(design.marks_per_edition));
        for (int mark_index = 0; mark_index < design.marks_per_edition; ++mark_index) {
            edition.marks.push_back({triangularCoordinate(unit(engine), unit(engine)),
                                     triangularCoordinate(unit(engine), unit(engine)),
                                     radius(engine), weightedMotif(weighted_ticket(engine))});
        }
        set.editions.push_back(edition);
    }
    return set;
}

std::string serializeEditions(const EditionSet& set) {
    if (set.editions.size() != edition_count || set.editions.empty()) return {};
    const int marks = static_cast<int>(set.editions.front().marks.size());
    if (!editionSetIsValid(set, marks)) return {};
    std::ostringstream output;
    output.imbue(std::locale::classic());
    output << "CONTROLLED_CHANCE_V1\n" << "seed " << set.seed << "\n"
           << "editions " << edition_count << " marks " << marks << "\n";
    output << std::setprecision(std::numeric_limits<float>::max_digits10);
    for (const auto& edition : set.editions) {
        output << "edition " << edition.index << "\n";
        for (const auto& mark : edition.marks) {
            output << "mark " << canonicalFloat(mark.x_unit) << ' '
                   << canonicalFloat(mark.y_unit) << ' '
                   << canonicalFloat(mark.radius) << ' '
                   << static_cast<int>(mark.motif) << "\n";
        }
    }
    output << "end\n";
    return output.str();
}

bool parseEditions(const std::string& text, EditionSet& set, std::string& error) {
    set = {};
    error.clear();
    std::istringstream input(text);
    input.imbue(std::locale::classic());
    std::string token;
    int edition_total = 0;
    int marks_total = 0;
    std::uint32_t seed = 0;
    std::string seed_token;
    if (!(input >> token) || token != "CONTROLLED_CHANCE_V1") error = "expected CONTROLLED_CHANCE_V1 header";
    else if (!(input >> token >> seed_token) || token != "seed" ||
             !parseSeedToken(seed_token, seed)) error = "invalid seed record";
    else if (!(input >> token >> edition_total) || token != "editions" || edition_total != edition_count ||
             !(input >> token >> marks_total) || token != "marks" || marks_total < 1 || marks_total > 48) error = "invalid editions/marks record";
    if (!error.empty()) return false;
    set.seed = seed;
    for (int expected_edition = 0; expected_edition < edition_count; ++expected_edition) {
        int observed_edition = -1;
        if (!(input >> token >> observed_edition) || token != "edition" || observed_edition != expected_edition) {
            error = "edition indices must be contiguous from zero"; set = {}; return false;
        }
        Edition edition{observed_edition, {}};
        for (int mark_index = 0; mark_index < marks_total; ++mark_index) {
            MarkParameter mark{};
            int motif = -1;
            if (!(input >> token >> mark.x_unit >> mark.y_unit >> mark.radius >> motif) || token != "mark") {
                error = "invalid or missing mark record"; set = {}; return false;
            }
            mark.x_unit = canonicalFloat(mark.x_unit);
            mark.y_unit = canonicalFloat(mark.y_unit);
            mark.radius = canonicalFloat(mark.radius);
            mark.motif = static_cast<Motif>(motif);
            if (!markValid(mark)) { error = "mark values are outside the portable contract"; set = {}; return false; }
            edition.marks.push_back(mark);
        }
        set.editions.push_back(edition);
    }
    if (!(input >> token) || token != "end") { error = "expected end record"; set = {}; return false; }
    std::string extra;
    if (input >> extra) { error = "unexpected data after end"; set = {}; return false; }
    return true;
}

bool editionSetIsValid(const EditionSet& set, int expected_marks_per_edition) {
    if (expected_marks_per_edition < 1 || expected_marks_per_edition > 48 ||
        set.editions.size() != edition_count) return false;
    for (int index = 0; index < edition_count; ++index) {
        const Edition& edition = set.editions[static_cast<std::size_t>(index)];
        if (edition.index != index || edition.marks.size() !=
            static_cast<std::size_t>(expected_marks_per_edition)) return false;
        for (const auto& mark : edition.marks) if (!markValid(mark)) return false;
    }
    return true;
}

Scene makeScene(const EditionSet& set, int edition_index, Viewport viewport) {
    Scene scene{};
    if (edition_index < 0 || edition_index >= edition_count || viewport.width < 64 ||
        viewport.height < 64 || set.editions.size() != edition_count) return scene;
    const Edition& edition = set.editions[static_cast<std::size_t>(edition_index)];
    if (!editionSetIsValid(set, static_cast<int>(edition.marks.size()))) return scene;
    float maximum_radius = 0.0f;
    for (const auto& mark : edition.marks) maximum_radius = std::max(maximum_radius, mark.radius);
    const float inset = maximum_radius + stroke_half_width + outer_margin;
    const float right = static_cast<float>(viewport.width) - inset;
    const float bottom = static_cast<float>(viewport.height) - inset;
    if (right < inset || bottom < inset) return scene;
    scene.edition_index = edition_index;
    for (const auto& mark : edition.marks) {
        scene.marks.push_back({{mapUnit(mark.x_unit, inset, right),
                                mapUnit(mark.y_unit, inset, bottom)},
                               mark.radius, mark.motif});
    }
    scene.valid = true;
    return scene;
}

bool sceneIsFiniteAndInBounds(const Scene& scene, Viewport viewport) {
    if (!scene.valid || viewport.width < 64 || viewport.height < 64 ||
        scene.edition_index < 0 || scene.edition_index >= edition_count ||
        scene.marks.empty()) return false;
    for (const auto& mark : scene.marks) {
        const float extent = mark.radius + stroke_half_width + outer_margin;
        if (!finite(mark.center.x) || !finite(mark.center.y) || !finite(mark.radius) ||
            mark.radius < 2.0f || mark.radius > 8.0f ||
            mark.center.x < extent || mark.center.y < extent ||
            mark.center.x > static_cast<float>(viewport.width) - extent ||
            mark.center.y > static_cast<float>(viewport.height) - extent) return false;
    }
    return true;
}

}  // namespace controlledchance
