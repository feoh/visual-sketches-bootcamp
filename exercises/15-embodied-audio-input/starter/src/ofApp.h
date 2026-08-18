#pragma once

#include "audio_input_model.h"
#include "audio_instrument_design.h"
#include "ofMain.h"

#include <atomic>
#include <cstddef>

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void audioIn(ofSoundBuffer& input) override;
    void exit() override;

private:
    void chooseNoDevice();
    void chooseRecorded();
    void tryLiveMicrophone();
    void stopMicrophone();
    void applyFallback();
    const char* sourceLabel() const;

    embodied::Design design_{};
    embodied::State state_{};
    ofSoundStream sound_stream_{};
    std::atomic<float> pending_amplitude_{0.0f};
    std::atomic<bool> pending_live_value_{false};
    float fallback_amplitude_ = 0.35f;
    std::size_t fixture_index_ = 0;
    int fixture_hold_ = 0;
    bool paused_ = false;
    bool reduced_motion_ = false;
    bool microphone_open_ = false;
};
