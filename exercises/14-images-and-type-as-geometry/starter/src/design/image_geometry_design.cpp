#include "image_geometry_design.h"

// LEARNER DESIGN SEAM: change density, threshold, mark scale, motion, and palette.
// Keep step > 0 and point_radius > 0; the pure model tests this seam.
ImageGeometryDesign makeImageGeometryDesign() {
    return {{2, 150, true}, 2.2f, 18.0f, 1.2f,
            {244, 239, 222}, {28, 70, 92}};
}
