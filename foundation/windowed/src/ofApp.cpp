#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("visual_sketches_bootcamp foundation probe");
    ofSetFrameRate(60);
    ofSetBackgroundColor(12, 18, 32);
    viewport_ = {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())};
    if (!badge_.load("foundation-probe.ppm")) {
        ofLogError("foundation") << "Could not load bin/data/foundation-probe.ppm";
    }
    reset();
}

void ofApp::reset() {
    model_ = course::makeModel(seed_, viewport_);
}

void ofApp::update() {
    if (!paused_) {
        course::update(model_, {{18.0f, 5.0f}}, fixedDt_, viewport_);
    }
}

void ofApp::draw() {
    ofSetColor(255);
    if (badge_.isAllocated()) {
        badge_.draw(24.0f, 24.0f, 96.0f, 96.0f);
    }
    ofSetColor(255, 194, 71);
    ofDrawCircle(model_.position.x, model_.position.y, 12.0f);
    ofSetColor(226);
    ofDrawBitmapString("space: pause   r: reset   fixed dt: 1/60   seed: 0x5eed1234", 24, 144);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        paused_ = !paused_;
    } else if (key == 'r' || key == 'R') {
        reset();
    }
}

void ofApp::windowResized(int width, int height) {
    viewport_ = {static_cast<float>(width), static_cast<float>(height)};
    model_.position.x = ofClamp(model_.position.x, 0.0f, viewport_.width);
    model_.position.y = ofClamp(model_.position.y, 0.0f, viewport_.height);
}
