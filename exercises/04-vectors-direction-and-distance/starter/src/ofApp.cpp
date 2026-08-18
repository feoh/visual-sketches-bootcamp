#include "ofApp.h"

namespace { ofColor colorFrom(constellation::Color color) { return ofColor(color.r, color.g, color.b); } }

void ofApp::setup() {
    ofSetWindowTitle("Section 04 starter: vector connector");
    ofSetCircleResolution(48);
    design_ = makeConstellationDesign();
    requested_target_ = {ofGetWidth() * 0.5f, ofGetHeight() * 0.5f};
    rebuild();
}
void ofApp::rebuild() {
    scene_ = constellation::makeScene(design_, {ofGetWidth(), ofGetHeight()}, requested_target_);
    if (scene_.valid) requested_target_ = scene_.target;
}
void ofApp::windowResized(int, int) { rebuild(); }
void ofApp::mouseMoved(int x, int y) { requested_target_ = {static_cast<float>(x), static_cast<float>(y)}; rebuild(); }
void ofApp::keyPressed(int key) {
    constexpr float keyboard_step = 12.0f;
    if (key == OF_KEY_LEFT) requested_target_.x -= keyboard_step;
    if (key == OF_KEY_RIGHT) requested_target_.x += keyboard_step;
    if (key == OF_KEY_UP) requested_target_.y -= keyboard_step;
    if (key == OF_KEY_DOWN) requested_target_.y += keyboard_step;
    rebuild();
}
void ofApp::draw() {
    ofBackground(colorFrom(design_.background));
    if (!scene_.valid) return;
    ofSetLineWidth(4.0f);
    ofSetColor(colorFrom(design_.ink));
    ofDrawLine(scene_.anchor.x, scene_.anchor.y, scene_.target.x, scene_.target.y);
    ofFill();
    ofDrawCircle(scene_.anchor.x, scene_.anchor.y, 8.0f);
    ofSetColor(colorFrom(design_.accent));
    ofNoFill();
    ofDrawRectangle(scene_.target.x - 8.0f, scene_.target.y - 8.0f, 16.0f, 16.0f);
    ofDrawCircle(scene_.traveler.x, scene_.traveler.y, 6.0f);
}
