#include "signature_design.h"

signature::Palette makePalette() {
    return {{{21, 35, 54}, {238, 91, 71}, {250, 196, 65}}};
}

signature::SignatureSpecs makeSignatureSpecs() {
    using signature::PrimitiveKind;
    return {{{PrimitiveKind::rectangle, {0.22f, 0.34f}, {0.12f, 0.08f}, 0U},
             {PrimitiveKind::circle, {0.46f, 0.40f}, {0.11f, 0.14f}, 1U},
             {PrimitiveKind::triangle, {0.69f, 0.35f}, {0.13f, 0.12f}, 2U},
             {PrimitiveKind::line, {0.38f, 0.69f}, {0.20f, 0.12f}, 2U},
             {PrimitiveKind::ellipse, {0.70f, 0.70f}, {0.17f, 0.08f}, 0U}}};
}
