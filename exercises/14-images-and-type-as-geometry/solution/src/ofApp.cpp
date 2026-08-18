#include "ofApp.h"

#include <algorithm>
#include <cmath>

namespace {
ofColor color(DisplayColor value) {
    return ofColor(static_cast<unsigned char>(value.r),
                   static_cast<unsigned char>(value.g),
                   static_cast<unsigned char>(value.b));
}
constexpr float tau = 6.28318530718f;
}

void ofApp::setup() {
    ofSetWindowTitle("Section 14 solution: negative-space ribbons");
    ofSetFrameRate(60);
    design_ = makeImageGeometryDesign();
    loadAsset();
}

void ofApp::loadAsset() {
    error_.clear(); source_ = {}; frame_ = {}; time_ = 0.0f;
    if (!imageGeometryDesignIsValid(design_)) {
        error_ = "Design error: use finite bounded sampling, motion, radius, and RGB values.";
        return;
    }
    if (!image_.load("seed-mask.png")) {
        error_ = "Asset load failed: bin/data/seed-mask.png is missing or unreadable.";
        return;
    }
    const ofPixels& pixels = image_.getPixels();
    image_geometry::Pixels owned{pixels.getWidth(), pixels.getHeight(),
                                 pixels.getNumChannels(), {}};
    owned.bytes.assign(pixels.getData(), pixels.getData() + pixels.size());
    const auto status = image_geometry::sampleMask(owned, design_.sample, source_);
    if (status != image_geometry::Status::ok) error_ = image_geometry::statusMessage(status);
    else frame_ = source_;
}

void ofApp::update() {
    if (paused_ || reduced_motion_ || !error_.empty()) return;
    const float frame_time = static_cast<float>(ofGetLastFrameTime());
    if (std::isfinite(frame_time) && frame_time > 0.0f)
        time_ += std::min(frame_time, 0.1f);
}

void ofApp::draw() {
    if (imageGeometryDesignIsValid(design_)) ofBackground(color(design_.background));
    else ofBackground(18, 24, 38);
    if (!error_.empty()) {
        ofSetColor(255, 130, 100);
        ofDrawBitmapString(error_ + " Fix the issue, then press R or rebuild.", 18, 28);
        return;
    }
    const float motion = reduced_motion_ ? 0.0f : time_ * design_.motion_rate;
    const float fit = std::min((ofGetWidth() - 90.0f) / image_.getWidth(),
                               (ofGetHeight() - 120.0f) / image_.getHeight());
    const image_geometry::Transform transform{
        {ofGetWidth()*0.5f-source_.centroid.x, ofGetHeight()*0.5f-source_.centroid.y},
        std::sin(motion)*0.22f, std::max(0.2f,fit)};
    const auto transform_status = image_geometry::transformGeometry(source_,transform,frame_);
    if (transform_status != image_geometry::Status::ok) {
        error_ = std::string("Transform error: ") + image_geometry::statusMessage(transform_status);
        ofSetColor(255, 130, 100);
        ofDrawBitmapString(error_, 18, 28);
        return;
    }
    ofSetColor(color(design_.ink));
    ofSetLineWidth(2.0f);
    for (std::size_t index=0; index<frame_.points.size(); ++index) {
        const auto point=frame_.points[index];
        const float phase=motion+tau*static_cast<float>(index%17)/17.0f;
        const float dx=std::cos(phase)*design_.motion_amplitude;
        const float dy=std::sin(phase)*design_.motion_amplitude;
        ofDrawLine(point.x-dx*0.25f,point.y-dy*0.25f,point.x+dx,point.y+dy);
    }
    ofSetColor(230);
    ofDrawBitmapString("bright-space ribbons | R reload | P pause | M reduced motion",14,22);
}

void ofApp::keyPressed(int key) {
    if (key=='r'||key=='R') loadAsset();
    else if (key=='p'||key=='P') paused_=!paused_;
    else if (key=='m'||key=='M') reduced_motion_=!reduced_motion_;
}
void ofApp::windowResized(int,int) {}
