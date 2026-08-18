#pragma once

#include <array>
#include <cstddef>

#if (defined(_MSVC_LANG) && _MSVC_LANG < 201703L) || \
    (!defined(_MSVC_LANG) && __cplusplus < 201703L)
#error "The section 00 geometry requires C++17 or newer"
#endif

namespace signature {

struct Viewport {
    float width;
    float height;
};

struct Point {
    float x;
    float y;
};

struct Color {
    int red;
    int green;
    int blue;
};

enum class PrimitiveKind {
    circle,
    rectangle,
    triangle,
    line,
    ellipse
};

struct Primitive {
    PrimitiveKind kind;
    Point center;
    float half_width;
    float half_height;
    std::size_t color_index;
};

struct PrimitiveSpec {
    PrimitiveKind kind;
    Point normalized_center;
    Point normalized_size;
    std::size_t color_index;
};

using Palette = std::array<Color, 3>;
using Signature = std::array<Primitive, 5>;
using SignatureSpecs = std::array<PrimitiveSpec, 5>;

Signature buildSignature(const SignatureSpecs& specs, Viewport viewport);
bool isInBounds(const Primitive& primitive, Viewport viewport);
bool signatureIsInBounds(const Signature& primitives, Viewport viewport);

}  // namespace signature
