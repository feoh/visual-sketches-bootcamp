#include "phase_field_design.h"

phasefield::Design makePhaseFieldDesign() {
    // Choose the grid, motion range and speed, starting rhythm, mark size, and colors.
    return {6, 8, 10.0f, 0.25f,
            phasefield::pi / 4.0f, phasefield::pi / 6.0f, 5.0f,
            {30, 41, 59}, {220, 92, 75}, {242, 237, 224}};
}
