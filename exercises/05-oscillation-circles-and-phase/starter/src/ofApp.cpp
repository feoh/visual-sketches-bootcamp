#include "ofApp.h"

namespace {
constexpr float fixed_dt = 1.0f / 60.0f;
ofColor colorFrom(phasefield::Color color) { return ofColor(color.r, color.g, color.b); }
}

void ofApp::setup() {
    ofSetWindowTitle("Section 05 starter: orbit grid");
    ofSetCircleResolution(40);
    ofSetFrameRate(60);
    design_ = makePhaseFieldDesign();
    rebuild();
}
void ofApp::update() {
    if (!paused_) time_seconds_ += fixed_dt;
    rebuild();
}
void ofApp::rebuild() {
    scene_ = phasefield::makeScene(design_, {ofGetWidth(), ofGetHeight()}, time_seconds_);
}
void ofApp::windowResized(int, int) { rebuild(); }
void ofApp::keyPressed(int key) {
    if (key == ' ' || key == 'p' || key == 'P') paused_ = !paused_;
    if (key == 'r' || key == 'R') time_seconds_ = 0.0f;
    rebuild();
}
void ofApp::draw() {
    ofBackground(colorFrom(design_.background));
    if (!scene_.valid) return;
    ofSetLineWidth(3.0f);
    for (const auto& mark : scene_.marks) {
        ofNoFill();
        ofSetColor(colorFrom(design_.ink));
        ofDrawCircle(mark.base.x, mark.base.y, design_.amplitude);
        const float crosshair_arm = design_.mark_radius < 3.0f
                                        ? design_.mark_radius
                                        : 3.0f;
        ofDrawLine(mark.base.x - crosshair_arm, mark.base.y,
                   mark.base.x + crosshair_arm, mark.base.y);
        ofDrawLine(mark.base.x, mark.base.y - crosshair_arm,
                   mark.base.x, mark.base.y + crosshair_arm);
        ofFill();
        ofSetColor(colorFrom(design_.accent));
        ofDrawCircle(mark.center.x, mark.center.y, design_.mark_radius);
    }
}
