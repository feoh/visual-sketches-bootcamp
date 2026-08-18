#pragma once

#include "ofMain.h"
#include "temporal_design.h"
#include "temporal_history.h"

#include <cstdint>

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void windowResized(int width, int height) override;

private:
    void resetScene();

    TemporalDesign design_{};
    temporal::History history_{};
    std::uint64_t frame_ = 0;
    double elapsed_ = 0.0;
    bool paused_ = false;
    bool reduced_motion_ = false;
};
