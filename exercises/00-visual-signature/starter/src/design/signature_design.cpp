#include "signature_design.h"

signature::Palette makePalette() {
    // TODO: Replace these duplicate placeholders with three distinct RGB colors.
    return {{{32, 46, 64}, {32, 46, 64}, {32, 46, 64}}};
}

signature::SignatureSpecs makeSignatureSpecs() {
    // Positions and full sizes are percentages written as decimals from 0 to 1.
    // TODO: Keep five entries, but use at least three primitive kinds. Change
    // the positions, sizes, order, and color numbers (0, 1, or 2) as well.
    using signature::Point;
    using signature::PrimitiveKind;
    return {{{PrimitiveKind::circle, {0.20f, 0.50f}, {0.07f, 0.10f}, 0U},
             {PrimitiveKind::circle, {0.35f, 0.50f}, {0.07f, 0.10f}, 1U},
             {PrimitiveKind::circle, {0.50f, 0.50f}, {0.07f, 0.10f}, 2U},
             {PrimitiveKind::circle, {0.65f, 0.50f}, {0.07f, 0.10f}, 0U},
             {PrimitiveKind::circle, {0.80f, 0.50f}, {0.07f, 0.10f}, 1U}}};
}
