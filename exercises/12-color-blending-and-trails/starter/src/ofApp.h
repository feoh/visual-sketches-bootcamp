#pragma once

#include "color_trail_model.h"
#include "ofMain.h"
#include "trail_design.h"

#include <vector>

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void keyPressed(int key) override;
    void windowResized(int width, int height) override;

private:
    void moveOrigin(float x, float y);
    void resetStudy();

    colortrail::Design design_{};
    std::vector<colortrail::TrailSample> trails_[2];
    std::vector<colortrail::TrailMark> marks_[2];
    colortrail::Vec2 origin_{};
    float phase_ = 0.0f;
    bool paused_ = false;
    bool reduced_motion_ = false;
};
