#include "signature_design.h"

signature::Palette makePalette() {
    // Keep exactly three colors, but replace these channel values with your palette.
    return {{{32, 46, 64}, {238, 180, 56}, {232, 236, 241}}};
}

signature::SignatureSpecs makeSignatureSpecs() {
    // Positions and full sizes are percentages written as decimals from 0 to 1.
    // Keep five entries. Change the shapes, positions, sizes, order, and color
    // numbers (0 for the first palette color, 1 for the second, or 2 for the third).
    using signature::Point;
    using signature::PrimitiveKind;
    return {{{PrimitiveKind::circle, {0.20f, 0.50f}, {0.07f, 0.10f}, 0U},
             {PrimitiveKind::circle, {0.35f, 0.50f}, {0.07f, 0.10f}, 1U},
             {PrimitiveKind::circle, {0.50f, 0.50f}, {0.07f, 0.10f}, 2U},
             {PrimitiveKind::circle, {0.65f, 0.50f}, {0.07f, 0.10f}, 0U},
             {PrimitiveKind::circle, {0.80f, 0.50f}, {0.07f, 0.10f}, 1U}}};
}
