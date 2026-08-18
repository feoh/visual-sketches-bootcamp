#pragma once

#include "ofMain.h"
#include "poster_design.h"
#include "poster_layout.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void draw() override;
    void windowResized(int width, int height) override;

private:
    void rebuild(int width, int height);
    responsiveposter::Design design_{};
    responsiveposter::Layout layout_{};
};
