#include "flow_field_design.h"

flow::Design makeFlowFieldDesign() {
    return {
        1.0f / 60.0f,
        0.25f,
        8,
        16,
        11,
        72,
        18,
        3.0f,
        78.0f,
        0.22f,
        0.32f,
        0.55f,
        {245, 240, 224},
        {34, 119, 128},
        {202, 71, 64}
    };
}
