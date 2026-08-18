#include "edition_design.h"
#include "edition_model.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

namespace {
int failures = 0;
void expect(bool condition, const std::string& message) {
    if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
bool near(float first, float second, float tolerance = 0.00001f) {
    return std::isfinite(first) && std::isfinite(second) &&
           std::fabs(first - second) <= tolerance;
}
std::string readText(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "portable serialized fixture opens");
    std::ostringstream text;
    text << input.rdbuf();
    return text.str();
}
controlledchance::Design knownDesign(std::uint32_t seed = 123456u) {
    return {seed, 20, 2.0f, 7.0f,
            {20, 30, 40}, {210, 90, 60}, {240, 232, 215}};
}
void engineAndChoiceCases() {
    using namespace controlledchance;
    expect(weightedMotif(0) == Motif::dot && weightedMotif(5) == Motif::dot,
           "tickets zero through five select the six-of-ten dot weight");
    expect(weightedMotif(6) == Motif::dash && weightedMotif(8) == Motif::dash,
           "tickets six through eight select the three-of-ten dash weight");
    expect(weightedMotif(9) == Motif::ring,
           "ticket nine selects the one-of-ten ring weight");
    int motif_counts[3] = {0, 0, 0};
    for (int ticket = 0; ticket < 10; ++ticket) {
        ++motif_counts[static_cast<int>(weightedMotif(ticket))];
    }
    expect(motif_counts[static_cast<int>(Motif::dot)] == 6 &&
           motif_counts[static_cast<int>(Motif::dash)] == 3 &&
           motif_counts[static_cast<int>(Motif::ring)] == 1,
           "all ten tickets map deterministically to exact six/three/one weights");
    int center = 0;
    int edges = 0;
    for (int first = 0; first <= 10; ++first) {
        for (int second = 0; second <= 10; ++second) {
            const float value = triangularCoordinate(static_cast<float>(first) / 10.0f,
                                                     static_cast<float>(second) / 10.0f);
            if (value >= 0.4f && value <= 0.6f) ++center;
            if (value <= 0.1f || value >= 0.9f) ++edges;
        }
    }
    expect(center > edges,
           "deterministic triangular grid has more center values than tail values");
    expect(near(triangularCoordinate(0.0f, 1.0f), 0.5f) &&
           near(triangularCoordinate(0.2f, 0.4f), 0.3f),
           "triangular coordinate is the documented mean of two uniform inputs");
}
void generationCases() {
    using namespace controlledchance;
    const EditionSet first = generateEditions(knownDesign());
    const EditionSet replay = generateEditions(knownDesign());
    expect(first.editions.size() == edition_count,
           "generation creates exactly six editions");
    expect(editionSetIsValid(first, 20), "generated parameters satisfy ranges and counts");
    expect(serializeEditions(first) == serializeEditions(replay),
           "same seed replays exactly within this build");
    bool motifs_are_legal = true;
    std::size_t generated_mark_count = 0;
    for (const auto& edition : first.editions) {
        generated_mark_count += edition.marks.size();
        for (const auto& mark : edition.marks) {
            motifs_are_legal = motifs_are_legal &&
                (mark.motif == Motif::dot || mark.motif == Motif::dash ||
                 mark.motif == Motif::ring);
        }
    }
    expect(generated_mark_count == static_cast<std::size_t>(edition_count * 20),
           "generator integration creates the exact requested mark count");
    expect(motifs_are_legal, "generator integration emits only legal motifs");
}
void fixtureAndSerializationCases(const std::string& fixture_text) {
    using namespace controlledchance;
    EditionSet fixture{};
    std::string error;
    expect(parseEditions(fixture_text, fixture, error),
           "independent portable fixture parses: " + error);
    expect(fixture.seed == 424242u && editionSetIsValid(fixture, 4),
           "fixture records seed, exact six editions, four finite marks, and legal ranges");
    expect(near(fixture.editions[0].marks[0].x_unit, 0.1f) &&
           fixture.editions[0].marks[2].motif == Motif::dash &&
           near(fixture.editions[5].marks[3].radius, 8.0f),
           "fixture semantic oracles are independent of the random generator");
    const std::string canonical = serializeEditions(fixture);
    EditionSet round_trip{};
    expect(parseEditions(canonical, round_trip, error),
           "canonical serialized model parameters parse on replay: " + error);
    expect(serializeEditions(round_trip) == canonical,
           "serialization reaches a stable canonical text after round trip");
    expect(round_trip.seed == fixture.seed &&
           near(round_trip.editions[3].marks[1].y_unit, 0.66f),
           "round trip preserves seed and generated parameters for cross-toolchain replay");
}
void malformedCases(const std::string& fixture_text) {
    using namespace controlledchance;
    std::string error;
    EditionSet parsed{};
    const std::string seed_record = "seed 424242";
    for (const std::string invalid_seed : {"seed -0", "seed +1",
                                           "seed 4294967296", "seed 42suffix"}) {
        std::string malformed_seed = fixture_text;
        malformed_seed.replace(malformed_seed.find(seed_record), seed_record.size(),
                               invalid_seed);
        expect(!parseEditions(malformed_seed, parsed, error) &&
               error.find("seed") != std::string::npos,
               "seed grammar rejects signs, uint32 overflow, and suffixes: " + invalid_seed);
    }
    std::string maximum_seed = fixture_text;
    maximum_seed.replace(maximum_seed.find(seed_record), seed_record.size(),
                         "seed 4294967295");
    expect(parseEditions(maximum_seed, parsed, error) &&
           parsed.seed == std::numeric_limits<std::uint32_t>::max(),
           "seed grammar accepts the full uint32 upper bound");
    expect(!parseEditions("WRONG_V1\n", parsed, error) && !error.empty(),
           "wrong version is rejected with a diagnostic");
    std::string bad_motif = fixture_text;
    const std::size_t motif = bad_motif.find("mark 0.1 0.2 2 0");
    bad_motif.replace(motif, std::string("mark 0.1 0.2 2 0").size(),
                      "mark 0.1 0.2 2 9");
    expect(!parseEditions(bad_motif, parsed, error) && error.find("portable contract") != std::string::npos,
           "unknown motif is rejected with a range diagnostic");
    std::string missing = fixture_text.substr(0, fixture_text.rfind("mark"));
    expect(!parseEditions(missing, parsed, error), "truncated edition is rejected");
    expect(!parseEditions(fixture_text + "extra\n", parsed, error) &&
           error.find("unexpected") != std::string::npos,
           "trailing data is rejected rather than silently ignored");
    std::string nonfinite = fixture_text;
    const std::size_t coordinate = nonfinite.find("0.1 0.2");
    nonfinite.replace(coordinate, std::string("0.1").size(), "nan");
    expect(!parseEditions(nonfinite, parsed, error), "non-finite parameters are rejected");

    std::string zero = fixture_text;
    const std::string zero_mark = "mark 0 0.5 2 0";
    const std::size_t zero_position = zero.find(zero_mark);
    std::string negative_zero = zero;
    std::string decimal_negative_zero = zero;
    negative_zero.replace(zero_position, zero_mark.size(), "mark -0 0.5 2 0");
    decimal_negative_zero.replace(zero_position, zero_mark.size(),
                                  "mark -0.0 0.5 2 0");
    EditionSet zero_set{};
    EditionSet negative_zero_set{};
    EditionSet decimal_negative_zero_set{};
    expect(parseEditions(zero, zero_set, error) &&
           parseEditions(negative_zero, negative_zero_set, error) &&
           parseEditions(decimal_negative_zero, decimal_negative_zero_set, error),
           "portable parser accepts equivalent zero spellings");
    expect(serializeEditions(zero_set) == serializeEditions(negative_zero_set) &&
           serializeEditions(zero_set) == serializeEditions(decimal_negative_zero_set),
           "0, -0, and -0.0 serialize to the same canonical float text");
}
void boundsCases(const controlledchance::EditionSet& fixture) {
    using namespace controlledchance;
    for (const auto viewport : {Viewport{64, 64}, {64, 900}, {900, 64},
                                {480, 320}, {1600, 900}}) {
        for (int edition = 0; edition < edition_count; ++edition) {
            const Scene scene = makeScene(fixture, edition, viewport);
            expect(sceneIsFiniteAndInBounds(scene, viewport),
                   "all six serialized scenes stay finite and stroke-aware bounded");
            for (const auto& mark : scene.marks) {
                const float extent = mark.radius + stroke_half_width + outer_margin;
                expect(mark.center.x - extent >= 0.0f && mark.center.y - extent >= 0.0f &&
                       mark.center.x + extent <= static_cast<float>(viewport.width) &&
                       mark.center.y + extent <= static_cast<float>(viewport.height),
                       "independent circle/dash/ring extrema remain inside viewport");
            }
        }
    }
    for (const auto viewport : {Viewport{63, 64}, {64, 63}, {0, 400}, {-1, 64}}) {
        expect(!makeScene(fixture, 0, viewport).valid,
               "viewport below sixty-four is explicitly invalid");
    }
    Scene mutated = makeScene(fixture, 0, {64, 64});
    mutated.marks.front().center.x = 0.0f;
    expect(!sceneIsFiniteAndInBounds(mutated, {64, 64}),
           "offscreen mutation proves bounds validator checks records");
}
void invalidDesignAndLearnerCases() {
    using namespace controlledchance;
    Design invalid = knownDesign();
    invalid.maximum_radius = std::numeric_limits<float>::infinity();
    expect(!designIsValid(invalid) && generateEditions(invalid).editions.empty(),
           "non-finite invalid design produces no editions");
    invalid = knownDesign();
    invalid.marks_per_edition = 49;
    expect(!designIsValid(invalid), "mark count above contract is invalid");
    const Design learner = makeEditionDesign();
    expect(designIsValid(learner),
           "starter learner owns a valid seed, density, radius range, and contrasting palette");
    const EditionSet set = generateEditions(learner);
    expect(editionSetIsValid(set, learner.marks_per_edition),
           "starter learner design generates exactly six valid editions");
    for (int index = 0; index < edition_count; ++index) {
        expect(sceneIsFiniteAndInBounds(makeScene(set, index, {960 / 3, 640 / 2}),
                                        {960 / 3, 640 / 2}),
               "starter six-panel composition remains inspectable and bounded");
    }
}
}  // namespace

int main(int argc, char** argv) {
    expect(argc == 2, "portable fixture path is provided");
    const std::string fixture_text = argc == 2 ? readText(argv[1]) : std::string{};
    engineAndChoiceCases();
    generationCases();
    fixtureAndSerializationCases(fixture_text);
    malformedCases(fixture_text);
    controlledchance::EditionSet fixture{};
    std::string error;
    if (controlledchance::parseEditions(fixture_text, fixture, error)) boundsCases(fixture);
    invalidDesignAndLearnerCases();
    if (failures) { std::cerr << failures << " section 06 checks failed\n"; return 1; }
    std::cout << "edition_model_test: same-build seed replay, exact six editions, exact ticket weights, legal generated motifs, triangular shape, strict portable parsing, canonical zeros, ranges, independent stroke-aware bounds, and learner contract passed\n";
}
