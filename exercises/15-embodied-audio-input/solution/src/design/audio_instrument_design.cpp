#include "audio_instrument_design.h"

embodied::Design makeAudioInstrumentDesign() {
    return {
        0.22f,
        0.06f,
        34.0f, 210.0f,
        6, 48,
        {12, 20, 36},
        {163, 187, 204},
        {255, 190, 92}
    };
}
