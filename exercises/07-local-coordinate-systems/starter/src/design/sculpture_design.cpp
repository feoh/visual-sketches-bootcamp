#include "sculpture_design.h"
sculpture::Design makeSculptureDesign() {
    // Own these dimensions, motion ranges, and colors; keep the contract valid.
    return {4.0f, 40.0f, 28.0f, 6.0f, 3.0f, 30.0f, 60.0f,
            {30, 43, 58}, {218, 93, 73}, {241, 235, 219}};
}
