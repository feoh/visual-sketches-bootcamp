#pragma once

#include "flow_field_design.h"
#include "flow_field_model.h"
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

    flow::Design design_{};
    flow::System system_{};
    flow::Vec2 control_{};
};
