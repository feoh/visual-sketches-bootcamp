#include "ofApp.h"

#include <algorithm>
namespace {
ofColor asColor(gesture::Color c) { return ofColor(c.r, c.g, c.b); }
}
void ofApp::setup() {
    ofSetWindowTitle("Section 08 starter: gesture ribbon");
    ofSetFrameRate(60);
    design_ = makeGestureDesign();
    keyboard_cursor_ = {ofGetWidth() * 0.5f, ofGetHeight() * 0.5f};
}
void ofApp::capture(float x, float y) {
    const float margin = design_.maximum_width * 0.5f;
    const float width = static_cast<float>(ofGetWidth());
    const float height = static_cast<float>(ofGetHeight());
    if (width < margin * 2.0f || height < margin * 2.0f) {
        keyboard_cursor_ = {width * 0.5f, height * 0.5f};
        return;
    }
    const gesture::Vec2 point{std::clamp(x, margin, width - margin),
                              std::clamp(y, margin, height - margin)};
    gesture::addSample(gesture_, point, ofGetElapsedTimef(), design_);
    keyboard_cursor_ = point;
}
void ofApp::mousePressed(int x, int y, int) { capture(static_cast<float>(x), static_cast<float>(y)); }
void ofApp::mouseDragged(int x, int y, int) { capture(static_cast<float>(x), static_cast<float>(y)); }
void ofApp::windowResized(int width, int height) {
    gesture::clear(gesture_);
    keyboard_cursor_ = {static_cast<float>(width) * 0.5f,
                        static_cast<float>(height) * 0.5f};
}
void ofApp::keyPressed(int key) {
    if (key == 'c' || key == 'C') { gesture::clear(gesture_); return; }
    const float step = 12.0f;
    if (key == OF_KEY_LEFT) keyboard_cursor_.x -= step;
    else if (key == OF_KEY_RIGHT) keyboard_cursor_.x += step;
    else if (key == OF_KEY_UP) keyboard_cursor_.y -= step;
    else if (key == OF_KEY_DOWN) keyboard_cursor_.y += step;
    else return;
    capture(keyboard_cursor_.x, keyboard_cursor_.y);
}
void ofApp::draw() {
    ofBackground(asColor(design_.background));
    ofNoFill();
    for (std::size_t i = 1; i < gesture_.samples.size(); ++i) {
        const auto style = gesture::styleForSample(gesture_.samples[i], design_);
        ofSetColor(asColor(style.color));
        ofSetLineWidth(style.width);
        const auto a = gesture_.samples[i - 1].position;
        const auto b = gesture_.samples[i].position;
        ofDrawLine(a.x, a.y, b.x, b.y);
    }
    ofFill(); ofSetColor(asColor(design_.fast_color));
    ofDrawCircle(keyboard_cursor_.x, keyboard_cursor_.y, 3.0f);
}
