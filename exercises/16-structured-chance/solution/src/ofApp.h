#pragma once

#include "ofMain.h"
#include "structured_chance_design.h"
#include "structured_chance_model.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void draw() override;
    void keyPressed(int key) override;
private:
    void rebuild();
    structuredchance::Settings settings_{};
    StructuredChancePalette palette_{};
    structuredchance::Composition composition_{};
    bool soft_layers_ = true;
    bool show_help_ = true;
};
