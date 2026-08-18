#pragma once

#include "family_design.h"
#include "mark_family.h"
#include "ofMain.h"

#include <vector>

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void windowResized(int width, int height) override;
    void draw() override;

private:
    void rebuild(int width, int height);
    markfamily::Design design_{};
    std::vector<markfamily::Mark> marks_;
};
