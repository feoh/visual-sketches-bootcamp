#include "ofApp.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
ofColor asColor(TemporalColor color) { return ofColor(color.r, color.g, color.b); }
}

void ofApp::setup() {
    ofSetWindowTitle("Section 13 starter: time windows");
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
    const double next_time = elapsed_ + dt;
    temporal::Sample next;
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
        ofGetWidth() < 80 || ofGetHeight() < 80) return;

    const float strip_width = static_cast<float>(ofGetWidth()) / design_.spatial_slices;
    for (std::size_t slice = 0; slice < design_.spatial_slices; ++slice) {
        const float position = design_.spatial_slices == 1 ? 0.0f :
            static_cast<float>(slice) / static_cast<float>(design_.spatial_slices - 1);
        temporal::Sample sample;
        float opacity = 0.0f;
        if (!temporal::select(history_, position, design_.decay_rate, sample, opacity)) continue;
        const ofColor color = asColor(design_.recent).getLerped(asColor(design_.distant), position);
        ofSetColor(color, static_cast<int>(255.0f * opacity));
        const float x = (static_cast<float>(slice) + 0.5f) * strip_width;
        ofDrawRectangle(x - strip_width * 0.42f, 0.0f, strip_width * 0.84f,
                        static_cast<float>(ofGetHeight()));
        ofSetColor(28, 34, 42);
        ofNoFill();
        ofSetLineWidth(2.0f);
        ofDrawCircle(x, sample.position.y, design_.mark_radius);
        ofDrawLine(x - design_.mark_radius, sample.position.y,
                   x + design_.mark_radius, sample.position.y);
        ofFill();
    }
    ofSetColor(28, 34, 42);
    ofDrawBitmapString("newest  ->  oldest    P pause | R replay | M reduced motion", 14, 24);
}

void ofApp::keyPressed(int key) {
    if (key == 'p' || key == 'P') paused_ = !paused_;
    else if (key == 'r' || key == 'R') resetScene();
    else if (key == 'm' || key == 'M') reduced_motion_ = !reduced_motion_;
}

void ofApp::windowResized(int, int) { resetScene(); }
