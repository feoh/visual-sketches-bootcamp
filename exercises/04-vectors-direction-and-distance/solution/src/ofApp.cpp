#include "ofApp.h"

namespace {
ofColor colorFrom(constellation::Color color) { return ofColor(color.r, color.g, color.b); }
constellation::Vec2 fromGlm(const glm::vec2& value) { return {value.x, value.y}; }
glm::vec2 toGlm(const constellation::Vec2& value) { return {value.x, value.y}; }
}

void ofApp::setup() {
    ofSetWindowTitle("Section 04 solution: distance constellation");
    ofSetCircleResolution(64);
    design_ = makeConstellationDesign();
    const glm::vec2 window_center{ofGetWidth() * 0.5f, ofGetHeight() * 0.5f};
    requested_target_ = fromGlm(window_center);
    rebuild();
}
void ofApp::rebuild() {
    scene_ = constellation::makeScene(design_, {ofGetWidth(), ofGetHeight()}, requested_target_);
    if (scene_.valid) requested_target_ = scene_.target;
}
void ofApp::windowResized(int, int) { rebuild(); }
void ofApp::mouseMoved(int x, int y) {
    const glm::vec2 pointer{static_cast<float>(x), static_cast<float>(y)};
    requested_target_ = fromGlm(pointer);
    rebuild();
}
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
    ofNoFill();
    ofSetLineWidth(3.0f);
    ofSetColor(colorFrom(design_.ink));
    const glm::vec2 anchor = toGlm(scene_.anchor);
    const glm::vec2 target = toGlm(scene_.target);
    ofDrawTriangle(scene_.anchor.x, scene_.anchor.y,
                   scene_.constellation_left.x, scene_.constellation_left.y,
                   scene_.constellation_right.x, scene_.constellation_right.y);
    ofDrawLine(anchor, target);
    ofSetColor(colorFrom(design_.accent));
    ofFill();
    for (int step = 1; step <= 4; ++step) {
        const float amount = static_cast<float>(step) / 5.0f;
        const auto point = constellation::add(scene_.anchor,
            constellation::scale(scene_.direction, amount));
        ofDrawCircle(point.x, point.y, 2.0f + amount * 3.0f);
    }
    ofDrawCircle(scene_.constellation_left.x, scene_.constellation_left.y, 6.0f);
    ofDrawCircle(scene_.constellation_right.x, scene_.constellation_right.y, 6.0f);
    ofSetColor(colorFrom(design_.background));
    ofDrawCircle(scene_.traveler.x, scene_.traveler.y, 4.0f);
}
