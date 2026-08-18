#pragma once
#include "ofMain.h"
#include "traveler_design.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void mouseMoved(int x, int y) override;
    void mouseDragged(int x, int y, int button) override;
private:
    traveler::Design design_{};
    traveler::State state_{};
    traveler::Input input_{false, {0.0f, 0.0f}, {1.0f, 0.0f}};
    bool left_ = false, right_ = false, up_ = false, down_ = false;
    void refreshKeyboardDirection();
};
