#include "image_geometry_design.h"

// A visually divergent choice: sparse bright-space samples become orbiting bars.
ImageGeometryDesign makeImageGeometryDesign() {
    return {{3, 220, false}, 3.4f, 30.0f, 0.7f,
            {18, 24, 38}, {246, 178, 72}};
}
