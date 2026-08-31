#include "population_model.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace lightpopulation {
namespace {
constexpr int maximum_clusters = 12;
constexpr int maximum_pebbles = 2000;

float draw(std::mt19937& engine, float low, float high) {
    return std::uniform_real_distribution<float>(low, high)(engine);
}

int drawInt(std::mt19937& engine, int low, int high) {
    return std::uniform_int_distribution<int>(low, high)(engine);
}

float centerBiased(std::mt19937& engine) {
    return (draw(engine, -1.0f, 1.0f) + draw(engine, -1.0f, 1.0f) +
            draw(engine, -1.0f, 1.0f)) /
           3.0f;
}
}  // namespace

bool finite(Vec3 value) {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}

float distance(Vec3 a, Vec3 b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool isValid(const Settings& settings) {
    return settings.cluster_count >= 2 && settings.cluster_count <= maximum_clusters &&
           settings.pebble_count >= settings.cluster_count &&
           settings.pebble_count <= maximum_pebbles &&
           std::isfinite(settings.volume_radius) && settings.volume_radius > 10.0f &&
           std::isfinite(settings.minimum_scale) && settings.minimum_scale > 0.0f &&
           std::isfinite(settings.maximum_scale) &&
           settings.maximum_scale >= settings.minimum_scale &&
           settings.maximum_scale <= 40.0f && std::isfinite(settings.cluster_spread) &&
           settings.cluster_spread > 0.0f &&
           settings.cluster_spread <= settings.volume_radius;
}

Population generatePopulation(const Settings& settings) {
    Population result{settings, {}, {}};
    if (!isValid(settings)) return result;

    std::mt19937 engine(settings.seed);
    result.clusters.reserve(static_cast<std::size_t>(settings.cluster_count));
    for (int index = 0; index < settings.cluster_count; ++index) {
        const float longitude = draw(engine, 0.0f, 6.28318530718f);
        const float latitude = draw(engine, -1.0f, 1.0f);
        const float radial = settings.volume_radius * draw(engine, 0.15f, 0.72f);
        const float horizontal = std::sqrt(std::max(0.0f, 1.0f - latitude * latitude));
        result.clusters.push_back({
            {std::cos(longitude) * horizontal * radial, latitude * radial,
             std::sin(longitude) * horizontal * radial},
            {draw(engine, -35.0f, 35.0f), draw(engine, -70.0f, 70.0f),
             draw(engine, -30.0f, 30.0f)},
            settings.cluster_spread * draw(engine, 0.65f, 1.0f),
            draw(engine, settings.minimum_scale, settings.maximum_scale), index % 2});
    }

    result.pebbles.reserve(static_cast<std::size_t>(settings.pebble_count));
    for (int index = 0; index < settings.pebble_count; ++index) {
        const int cluster_index = drawInt(engine, 0, settings.cluster_count - 1);
        const Cluster& cluster = result.clusters[static_cast<std::size_t>(cluster_index)];
        Vec3 offset{centerBiased(engine) * cluster.spread,
                    centerBiased(engine) * cluster.spread,
                    centerBiased(engine) * cluster.spread};
        const float inherited = cluster.typical_scale;
        const float scale = std::clamp(inherited * draw(engine, 0.58f, 1.38f),
                                       settings.minimum_scale, settings.maximum_scale);
        result.pebbles.push_back({
            {cluster.center.x + offset.x, cluster.center.y + offset.y,
             cluster.center.z + offset.z},
            {cluster.preferred_rotation.x + draw(engine, -18.0f, 18.0f),
             cluster.preferred_rotation.y + draw(engine, -18.0f, 18.0f),
             cluster.preferred_rotation.z + draw(engine, -18.0f, 18.0f)},
            {scale * draw(engine, 1.2f, 2.7f), scale * draw(engine, 0.55f, 1.0f),
             scale * draw(engine, 0.55f, 1.0f)},
            cluster_index, cluster.material_role});
    }
    return result;
}

}  // namespace lightpopulation
