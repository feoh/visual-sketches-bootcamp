#include "ofApp.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
ofColor color(DisplayColor value) { return {value.r, value.g, value.b}; }
}

void ofApp::setup() {
    ofSetWindowTitle("Section 14 starter: mask constellation");
    ofSetFrameRate(60);
    design_ = makeImageGeometryDesign();
    loadAsset();
}

void ofApp::loadAsset() {
    error_.clear();
    source_ = {};
    frame_ = {};
    if (!image_.load("seed-mask.png")) {
        error_ = "Asset load failed: bin/data/seed-mask.png is missing or unreadable.";
        return;
    }
    // ofImage owns decoded pixels; copy them into the platform-neutral model.
    const ofPixels& pixels = image_.getPixels();
    image_geometry::Pixels owned{pixels.getWidth(), pixels.getHeight(),
                                 pixels.getNumChannels(), {}};
    owned.bytes.assign(pixels.getData(), pixels.getData() + pixels.size());
    const auto status = image_geometry::sampleMask(owned, design_.sample, source_);
    if (status != image_geometry::Status::ok) {
        error_ = image_geometry::statusMessage(status);
        return;
    }
    frame_ = source_;
}

void ofApp::update() {
    if (paused_ || reduced_motion_ || !error_.empty()) return;
    time_ += std::min(ofGetLastFrameTime(), 0.1f);
}

void ofApp::draw() {
    ofBackground(color(design_.background));
    if (!error_.empty()) {
        ofSetColor(150, 35, 35);
        ofDrawBitmapString(error_, 18, 28);
        ofDrawBitmapString("Restore the bundled asset, then press R.", 18, 48);
        return;
    }
    const float oscillation = reduced_motion_ ? 0.0f : std::sin(time_ * design_.motion_rate);
    const float scale = std::min((ofGetWidth() - 80.0f) / image_.getWidth(),
                                 (ofGetHeight() - 110.0f) / image_.getHeight());
    const image_geometry::Transform transform{
        {ofGetWidth() * 0.5f - source_.centroid.x,
         ofGetHeight() * 0.5f - source_.centroid.y + oscillation * design_.motion_amplitude},
        oscillation * 0.10f, std::max(0.2f, scale)};
    if (image_geometry::transformGeometry(source_, transform, frame_) != image_geometry::Status::ok) return;
    ofSetColor(color(design_.ink));
    for (const auto point : frame_.points)
        ofDrawCircle(point.x, point.y, design_.point_radius);
    ofSetColor(55);
    ofDrawBitmapString("R reload | P pause | M reduced motion", 14, 22);
}

void ofApp::keyPressed(int key) {
    if (key == 'r' || key == 'R') loadAsset();
    else if (key == 'p' || key == 'P') paused_ = !paused_;
    else if (key == 'm' || key == 'M') reduced_motion_ = !reduced_motion_;
}
void ofApp::windowResized(int, int) {}
