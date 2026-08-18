#include "phase_field_design.h"

phasefield::Design makePhaseFieldDesign() {
    // Explained solution: denser diagonal phase, smaller orbit, distinct palette.
    return {7, 11, 7.0f, 0.18f,
            phasefield::pi / 3.0f, -phasefield::pi / 8.0f, 3.5f,
            {24, 31, 44}, {58, 156, 151}, {246, 225, 188}};
}
