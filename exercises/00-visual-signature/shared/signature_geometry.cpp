#include "signature_geometry.h"

#include <algorithm>
#include <cmath>

namespace signature {
namespace {

float safeExtent(float value) {
    return std::isfinite(value) ? std::max(1.0f, value) : 1.0f;
}

float unit(float value) {
    return std::isfinite(value) ? std::clamp(value, 0.0f, 1.0f) : 0.0f;
}

}  // namespace

Signature buildSignature(const SignatureSpecs& specs, Viewport viewport) {
    viewport.width = safeExtent(viewport.width);
    viewport.height = safeExtent(viewport.height);

    Signature result{};
    for (std::size_t index = 0; index < result.size(); ++index) {
        const PrimitiveSpec& spec = specs[index];
        const float half_width = unit(spec.normalized_size.x) * viewport.width * 0.5f;
        const float half_height = unit(spec.normalized_size.y) * viewport.height * 0.5f;
        const float center_x = std::clamp(unit(spec.normalized_center.x) * viewport.width,
                                          half_width, viewport.width - half_width);
        const float center_y = std::clamp(unit(spec.normalized_center.y) * viewport.height,
                                          half_height, viewport.height - half_height);
        result[index] = {spec.kind, {center_x, center_y}, half_width, half_height,
                         spec.color_index};
    }
    return result;
}

bool isInBounds(const Primitive& primitive, Viewport viewport) {
    viewport.width = safeExtent(viewport.width);
    viewport.height = safeExtent(viewport.height);
    return primitive.half_width >= 0.0f && primitive.half_height >= 0.0f &&
           primitive.center.x - primitive.half_width >= 0.0f &&
           primitive.center.y - primitive.half_height >= 0.0f &&
           primitive.center.x + primitive.half_width <= viewport.width &&
           primitive.center.y + primitive.half_height <= viewport.height &&
           primitive.color_index < 3U;
}

bool signatureIsInBounds(const Signature& primitives, Viewport viewport) {
    return std::all_of(primitives.begin(), primitives.end(),
                       [viewport](const Primitive& primitive) {
                           return isInBounds(primitive, viewport);
                       });
}

}  // namespace signature
