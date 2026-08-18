#pragma once

#include "ofMain.h"
#include "particle_design.h"
#include "particle_model.h"

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
    void resetScene();

    particles::Design design_{};
    particles::Emitter emitter_{};
    bool reduced_motion_ = false;
};
