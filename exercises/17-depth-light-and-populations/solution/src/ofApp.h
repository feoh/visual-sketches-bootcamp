#pragma once

#include "ofMain.h"
#include "population_design.h"
#include "population_model.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
private:
    void rebuild();
    lightpopulation::Settings settings_{};
    PopulationPalette palette_{};
    lightpopulation::Population population_{};
    ofEasyCam camera_;
    ofLight warm_light_;
    ofLight cool_light_;
    ofMaterial warm_material_;
    ofMaterial cool_material_;
    float turn_degrees_ = 0.0f;
    bool paused_ = false;
    bool reduced_detail_ = false;
    bool show_help_ = true;
};
