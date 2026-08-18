#include "ofApp.h"

namespace {
constexpr float fixed_dt = 1.0f / 60.0f;
ofColor colorFrom(phasefield::Color color) { return ofColor(color.r, color.g, color.b); }
}

void ofApp::setup() {
    ofSetWindowTitle("Section 05 solution: phase weave");
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
    ofSetLineWidth(2.0f);
    ofSetColor(colorFrom(design_.ink));
    for (int row = 0; row < design_.rows; ++row) {
        ofPolyline thread;
        for (int column = 0; column < design_.columns; ++column) {
            const auto& mark = scene_.marks[static_cast<std::size_t>(
                phasefield::flatIndex(row, column, design_.columns))];
            thread.addVertex(mark.center.x, mark.center.y);
        }
        thread.draw();
    }
    for (const auto& mark : scene_.marks) {
        if ((mark.row + mark.column) % 2 == 0) {
            ofFill();
            ofSetColor(colorFrom(design_.accent));
        } else {
            ofNoFill();
            ofSetColor(colorFrom(design_.ink));
        }
        const float radius = design_.mark_radius;
        ofBeginShape();
        ofVertex(mark.center.x, mark.center.y - radius);
        ofVertex(mark.center.x + radius, mark.center.y);
        ofVertex(mark.center.x, mark.center.y + radius);
        ofVertex(mark.center.x - radius, mark.center.y);
        ofEndShape(true);
    }
}
