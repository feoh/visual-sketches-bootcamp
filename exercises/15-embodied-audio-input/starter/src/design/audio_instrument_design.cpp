#include "audio_instrument_design.h"

embodied::Design makeAudioInstrumentDesign() {
    // Learner seam: change response, geometry range, density, and palette here.
    return {
        0.5f,                 // smoothing: new = old + smoothing * (raw - old)
        0.1f,                 // dead zone: normalized output stays zero at/below this
        20.0f, 120.0f,        // quiet and loud radii
        4, 24,                // quiet and loud repeated-mark counts
        {247, 244, 235},      // background
        {42, 55, 68},         // quiet mark
        {194, 55, 74}         // active mark
    };
}
