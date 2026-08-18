#pragma once

#include "ofMain.h"
#include "design/signature_design.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void draw() override;
    void windowResized(int width, int height) override;

private:
    void rebuild(int width, int height);

    signature::Palette palette_{};
    signature::Signature primitives_{};
};
