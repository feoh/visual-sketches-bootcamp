#pragma once

#include "edition_design.h"
#include "edition_model.h"
#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void windowResized(int width, int height) override;
    void keyPressed(int key) override;

private:
    void rebuild();
    controlledchance::Design design_{};
    controlledchance::EditionSet editions_{};
    std::vector<controlledchance::Scene> scenes_;
};
