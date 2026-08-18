#pragma once

#include "force_design.h"
#include "force_model.h"
#include "ofMain.h"

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
    void moveControl(float x, float y);
    void resetScene();

    forces::Design design_{};
    forces::System system_{};
    bool reduced_motion_ = false;
};
