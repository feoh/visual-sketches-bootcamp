#include "population_design.h"
#include "population_model.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {
void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "section 17 test failed: " << message << '\n';
        std::exit(1);
    }
}
bool near(float a, float b, float tolerance = 0.00001f) {
    return std::fabs(a - b) <= tolerance;
}
bool same(lightpopulation::Vec3 a, lightpopulation::Vec3 b) {
    return near(a.x, b.x) && near(a.y, b.y) && near(a.z, b.z);
}
}

int main() {
    auto settings = makePopulationSettings();
    require(lightpopulation::isValid(settings), "starter settings must be valid");
    const auto first = lightpopulation::generatePopulation(settings);
    const auto replay = lightpopulation::generatePopulation(settings);
    require(first.clusters.size() == static_cast<std::size_t>(settings.cluster_count),
            "the requested cluster count must be generated");
    require(first.pebbles.size() == static_cast<std::size_t>(settings.pebble_count),
            "the requested bounded population must be generated");
    require(first.pebbles.size() == replay.pebbles.size(), "same seed must replay count");

    for (std::size_t index = 0; index < first.clusters.size(); ++index) {
        const auto& cluster = first.clusters[index];
        require(lightpopulation::finite(cluster.center) &&
                    lightpopulation::finite(cluster.preferred_rotation),
                "cluster records must remain finite");
        require(lightpopulation::distance(cluster.center, {0.0f, 0.0f, 0.0f}) <=
                    settings.volume_radius * 0.721f,
                "cluster anchors must stay inside the designed volume");
        require(same(cluster.center, replay.clusters[index].center),
                "same seed must replay cluster anchors");
    }

    for (std::size_t index = 0; index < first.pebbles.size(); ++index) {
        const auto& pebble = first.pebbles[index];
        const auto& again = replay.pebbles[index];
        require(pebble.cluster >= 0 && pebble.cluster < settings.cluster_count,
                "every pebble must inherit from one valid cluster");
        const auto& cluster = first.clusters[static_cast<std::size_t>(pebble.cluster)];
        require(pebble.material_role == cluster.material_role,
                "material family must be inherited rather than independently randomized");
        require(std::fabs(pebble.rotation_degrees.x - cluster.preferred_rotation.x) <= 18.001f &&
                    std::fabs(pebble.rotation_degrees.y - cluster.preferred_rotation.y) <= 18.001f &&
                    std::fabs(pebble.rotation_degrees.z - cluster.preferred_rotation.z) <= 18.001f,
                "orientation jitter must stay near its cluster preference");
        require(lightpopulation::distance(pebble.position, cluster.center) <=
                    cluster.spread * 1.733f,
                "clustered positions must stay inside their bounded local volume");
        require(pebble.scale.x >= settings.minimum_scale * 1.2f &&
                    pebble.scale.x <= settings.maximum_scale * 2.7f &&
                    pebble.scale.y >= settings.minimum_scale * 0.55f &&
                    pebble.scale.y <= settings.maximum_scale &&
                    pebble.scale.z >= settings.minimum_scale * 0.55f &&
                    pebble.scale.z <= settings.maximum_scale,
                "elongated scales must remain positive and capped");
        require(same(pebble.position, again.position) && same(pebble.scale, again.scale) &&
                    pebble.material_role == again.material_role,
                "same seed must replay transform and material records");
    }

    settings.pebble_count = 2001;
    require(!lightpopulation::isValid(settings), "population above the work cap must be rejected");
    settings = makePopulationSettings();
    settings.minimum_scale = -1.0f;
    require(!lightpopulation::isValid(settings), "negative scale must be rejected");
    std::cout << "section-17-model: deterministic clusters, inherited transforms, and caps passed\n";
}
