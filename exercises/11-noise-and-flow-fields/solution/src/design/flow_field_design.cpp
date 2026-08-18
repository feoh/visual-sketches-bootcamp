#include "flow_field_design.h"

flow::Design makeFlowFieldDesign() {
    return {
        1.0f / 90.0f,
        0.20f,
        10,
        23,
        15,
        108,
        28,
        4.0f,
        112.0f,
        0.14f,
        0.21f,
        0.38f,
        {18, 24, 42},
        {116, 204, 183},
        {245, 179, 74}
    };
}
