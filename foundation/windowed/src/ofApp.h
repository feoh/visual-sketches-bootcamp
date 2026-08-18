#pragma once

#include "course_probe.h"
#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void windowResized(int width, int height) override;

private:
    void reset();

    static constexpr float fixedDt_ = 1.0f / 60.0f;
    static constexpr std::uint32_t seed_ = 0x5eed1234U;
    course::Viewport viewport_{800.0f, 600.0f};
    course::Model model_;
    ofImage badge_;
    bool paused_ = false;
};
