#pragma once
#include "gesture_design.h"
#include "gesture_model.h"
#include "ofMain.h"
class ofApp : public ofBaseApp {
public:
    void setup() override;
    void draw() override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void keyPressed(int key) override;
    void windowResized(int width, int height) override;
private:
    void capture(float x, float y);
    gesture::Design design_{};
    gesture::Gesture gesture_{};
    gesture::Vec2 keyboard_cursor_{};
};
