#pragma once

#include <cstdint>
#include <vector>

namespace lightpopulation {

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Pebble {
    Vec3 position;
    Vec3 rotation_degrees;
    Vec3 scale;
    int cluster;
    int material_role;
};

struct Cluster {
    Vec3 center;
    Vec3 preferred_rotation;
    float spread;
    float typical_scale;
    int material_role;
};

struct Settings {
    std::uint32_t seed;
    int cluster_count;
    int pebble_count;
    float volume_radius;
    float minimum_scale;
    float maximum_scale;
    float cluster_spread;
};

struct Population {
    Settings settings;
    std::vector<Cluster> clusters;
    std::vector<Pebble> pebbles;
};

bool finite(Vec3 value);
float distance(Vec3 a, Vec3 b);
bool isValid(const Settings& settings);
Population generatePopulation(const Settings& settings);

}  // namespace lightpopulation
