#pragma once

#include "ofMain.h"
#include "phase_field_design.h"
#include "phase_field_model.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void windowResized(int width, int height) override;
    void keyPressed(int key) override;

private:
    void rebuild();
    phasefield::Design design_{};
    phasefield::Scene scene_{};
    float time_seconds_ = 0.0f;
    bool paused_ = false;
};
