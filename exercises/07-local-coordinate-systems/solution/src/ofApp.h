#pragma once
#include "ofMain.h"
#include "sculpture_design.h"
#include "sculpture_model.h"
#include <array>
class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void windowResized(int width, int height) override;
private:
    void rebuild();
    sculpture::Design design_{};
    std::array<sculpture::Scene, 3> scenes_{};
};
