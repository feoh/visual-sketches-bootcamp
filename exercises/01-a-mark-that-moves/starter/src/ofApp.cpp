#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("Section 01 — a mark that moves");
    ofSetBackgroundAuto(true);
    design_ = makeTravelerDesign();
    state_ = traveler::makeState(design_, {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())});
}
void ofApp::update() {
    traveler::advanceFrame(state_, design_, input_, ofGetLastFrameTime(),
                           {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())});
}
void ofApp::draw() {
    ofBackground(design_.background_color.red, design_.background_color.green, design_.background_color.blue);
    ofSetLineWidth(state_.reduced_motion ? 2.0f : 5.0f);
    ofSetColor(design_.trail_color.red, design_.trail_color.green, design_.trail_color.blue);
    const float trail_seconds = state_.reduced_motion ? 0.08f : 0.20f;
    ofDrawLine(state_.position.x, state_.position.y,
               state_.position.x - state_.velocity.x * trail_seconds,
               state_.position.y - state_.velocity.y * trail_seconds);
    ofSetColor(design_.mark_color.red, design_.mark_color.green, design_.mark_color.blue);
    ofDrawCircle(state_.position.x, state_.position.y, design_.radius_pixels);
    if (state_.paused) {
        ofNoFill();
        ofSetLineWidth(3.0f);
        ofDrawCircle(state_.position.x, state_.position.y, design_.radius_pixels + 8.0f);
        ofFill();
    }
}
void ofApp::refreshKeyboardDirection() {
    input_.keyboard_direction = {static_cast<float>(right_) - static_cast<float>(left_),
                                 static_cast<float>(down_) - static_cast<float>(up_)};
}
void ofApp::keyPressed(int key) {
    if (key == ' ') traveler::togglePause(state_);
    else if (key == 'r' || key == 'R') traveler::reset(state_, design_, {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())});
    else if (key == 'm' || key == 'M') traveler::toggleReducedMotion(state_);
    else if (key == OF_KEY_LEFT) { left_ = true; right_ = false; input_.pointer_active = false; }
    else if (key == OF_KEY_RIGHT) { right_ = true; left_ = false; input_.pointer_active = false; }
    else if (key == OF_KEY_UP) { up_ = true; down_ = false; input_.pointer_active = false; }
    else if (key == OF_KEY_DOWN) { down_ = true; up_ = false; input_.pointer_active = false; }
    refreshKeyboardDirection();
}
void ofApp::keyReleased(int key) {
    if (key == OF_KEY_LEFT) left_ = false;
    else if (key == OF_KEY_RIGHT) right_ = false;
    else if (key == OF_KEY_UP) up_ = false;
    else if (key == OF_KEY_DOWN) down_ = false;
    refreshKeyboardDirection();
}
void ofApp::mouseMoved(int x, int y) { input_.pointer_active = true; input_.pointer = {static_cast<float>(x), static_cast<float>(y)}; }
void ofApp::mouseDragged(int x, int y, int) { mouseMoved(x, y); }
