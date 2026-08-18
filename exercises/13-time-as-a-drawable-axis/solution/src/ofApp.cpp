#include "ofApp.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
ofColor asColor(TemporalColor color) { return ofColor(color.r, color.g, color.b); }
}

void ofApp::setup() {
    ofSetWindowTitle("Section 13 solution: amber time loom");
    ofSetFrameRate(60);
    design_ = makeTemporalDesign();
    resetScene();
}

void ofApp::resetScene() {
    temporal::configure(history_, design_.history_capacity);
    frame_ = 0;
    elapsed_ = 0.0;
    temporal::Sample first;
    if (temporal::makeMotionSample(frame_, elapsed_,
                                   static_cast<float>(std::max(ofGetWidth(), 1)),
                                   static_cast<float>(std::max(ofGetHeight(), 1)), first)) {
        temporal::push(history_, first);
    }
}

void ofApp::update() {
    if (paused_ || reduced_motion_ || frame_ == std::numeric_limits<std::uint64_t>::max()) return;
    const double dt = ofGetLastFrameTime();
    if (!std::isfinite(dt) || dt < 0.0 || dt > 0.25) return;
    temporal::Sample next;
    const double next_time = elapsed_ + dt;
    if (std::isfinite(next_time) && temporal::makeMotionSample(
            frame_ + 1, next_time, static_cast<float>(std::max(ofGetWidth(), 1)),
            static_cast<float>(std::max(ofGetHeight(), 1)), next) &&
        temporal::push(history_, next)) {
        ++frame_;
        elapsed_ = next_time;
    }
}

void ofApp::draw() {
    ofBackground(asColor(design_.background));
    if (!temporalDesignIsValid(design_) || history_.count == 0 ||
        ofGetWidth() < 100 || ofGetHeight() < 100) return;

    ofSetLineWidth(1.5f);
    const float margin = 38.0f;
    const float span = ofGetWidth() - margin * 2.0f;
    for (std::size_t slice = 0; slice < design_.spatial_slices; ++slice) {
        const float position = static_cast<float>(slice) /
                               static_cast<float>(design_.spatial_slices - 1);
        temporal::Sample sample;
        float opacity = 0.0f;
        if (!temporal::select(history_, position, design_.decay_rate, sample, opacity)) continue;
        const float x = margin + position * span;
        const float y = ofMap(sample.position.y, 0.0f,
                              static_cast<float>(std::max(ofGetHeight(), 1)),
                              92.0f, ofGetHeight() - 62.0f, true);
        const ofColor color = asColor(design_.recent).getLerped(asColor(design_.distant), position);
        ofSetColor(color, static_cast<int>(80.0f + 175.0f * opacity));
        ofDrawLine(x, ofGetHeight() * 0.5f, x, y);
        ofPushMatrix();
        ofTranslate(x, y);
        ofRotateRad(sample.phase);
        ofNoFill();
        ofDrawRectRounded(-design_.mark_radius, -design_.mark_radius,
                          design_.mark_radius * 2.0f, design_.mark_radius * 2.0f, 2.0f);
        ofDrawLine(-design_.mark_radius * 0.6f, 0.0f,
                   design_.mark_radius * 0.6f, 0.0f);
        ofFill();
        ofPopMatrix();
        if (slice % 3 == 0) ofDrawCircle(x, ofGetHeight() * 0.5f, 2.0f + 3.0f * opacity);
    }
    ofSetColor(235, 239, 232);
    ofDrawBitmapString("NOW  < spatial delay loom >  THEN    P pause | R replay | M still", 14, 24);
}

void ofApp::keyPressed(int key) {
    if (key == 'p' || key == 'P') paused_ = !paused_;
    else if (key == 'r' || key == 'R') resetScene();
    else if (key == 'm' || key == 'M') reduced_motion_ = !reduced_motion_;
}

void ofApp::windowResized(int, int) { resetScene(); }
