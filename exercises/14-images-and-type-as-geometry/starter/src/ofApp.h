#pragma once
#include "ofMain.h"
#include "image_geometry_design.h"
#include "image_geometry_model.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void windowResized(int width, int height) override;
private:
    void loadAsset();
    ImageGeometryDesign design_{};
    ofImage image_;
    image_geometry::Geometry source_;
    image_geometry::Geometry frame_;
    std::string error_;
    float time_ = 0.0f;
    bool paused_ = false;
    bool reduced_motion_ = false;
};
