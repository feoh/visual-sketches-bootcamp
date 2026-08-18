#pragma once

#include "constellation_design.h"
#include "constellation_model.h"
#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void draw() override;
    void windowResized(int width, int height) override;
    void mouseMoved(int x, int y) override;
    void keyPressed(int key) override;

private:
    void rebuild();
    constellation::Design design_{};
    constellation::Vec2 requested_target_{};
    constellation::Scene scene_{};
};
