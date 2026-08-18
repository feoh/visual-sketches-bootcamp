#include "ofApp.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr float pi = 3.14159265358979323846f;

ofColor asOfColor(colortrail::Color color) {
    return ofColor(static_cast<unsigned char>(color.r * 255.0f + 0.5f),
                   static_cast<unsigned char>(color.g * 255.0f + 0.5f),
                   static_cast<unsigned char>(color.b * 255.0f + 0.5f),
                   static_cast<unsigned char>(color.a * 255.0f + 0.5f));
}
}

void ofApp::setup() {
    ofSetWindowTitle("Section 12 starter: paired ribbon study");
    ofSetFrameRate(60);
    design_ = makeTrailDesign();
    resetStudy();
}

void ofApp::resetStudy() {
    trails_[0].clear();
    trails_[1].clear();
    marks_[0].clear();
    marks_[1].clear();
    origin_ = {ofGetWidth() * 0.5f, ofGetHeight() * 0.52f};
    phase_ = 0.0f;
    paused_ = false;
    reduced_motion_ = false;
}

void ofApp::moveOrigin(float x, float y) {
    const float inset = design_.maximum_radius + 2.0f;
    if (ofGetWidth() < inset * 2.0f || ofGetHeight() < inset * 2.0f) return;
    origin_ = {std::clamp(x, inset, ofGetWidth() - inset),
               std::clamp(y, inset, ofGetHeight() - inset)};
}

void ofApp::update() {
    if (!paused_) {
        phase_ += std::clamp(static_cast<float>(ofGetLastFrameTime()), 0.0f, 0.05f) * 1.7f;
        const float reach = std::max(0.0f,
            std::min(ofGetWidth(), ofGetHeight()) * 0.31f - design_.maximum_radius);
        const colortrail::TrailSample first{
            {origin_.x + std::cos(phase_) * reach,
             origin_.y + std::sin(phase_ * 2.0f) * reach * 0.42f},
            0.5f + 0.5f * std::sin(phase_ * 0.73f), 0};
        const colortrail::TrailSample second{
            {origin_.x + std::cos(phase_ + pi) * reach * 0.78f,
             origin_.y + std::sin(phase_ * 1.5f + pi * 0.5f) * reach * 0.62f},
            0.5f + 0.5f * std::cos(phase_ * 0.61f), 1};
        colortrail::appendSample(trails_[0], first, design_.maximum_history);
        colortrail::appendSample(trails_[1], second, design_.maximum_history);
    }
    if (reduced_motion_) {
        for (auto& trail : trails_) {
            if (trail.size() > 1) trail.erase(trail.begin(), trail.end() - 1);
        }
    }
    colortrail::buildTrailMarks(trails_[0], design_, marks_[0]);
    colortrail::buildTrailMarks(trails_[1], design_, marks_[1]);
}

void ofApp::mousePressed(int x, int y, int) {
    moveOrigin(static_cast<float>(x), static_cast<float>(y));
}

void ofApp::mouseDragged(int x, int y, int) {
    moveOrigin(static_cast<float>(x), static_cast<float>(y));
}

void ofApp::windowResized(int, int) { resetStudy(); }

void ofApp::keyPressed(int key) {
    if (key == 'p' || key == 'P') paused_ = !paused_;
    else if (key == 'r' || key == 'R') resetStudy();
    else if (key == 'm' || key == 'M') reduced_motion_ = !reduced_motion_;
    else {
        float x = origin_.x;
        float y = origin_.y;
        if (key == OF_KEY_LEFT) x -= 18.0f;
        else if (key == OF_KEY_RIGHT) x += 18.0f;
        else if (key == OF_KEY_UP) y -= 18.0f;
        else if (key == OF_KEY_DOWN) y += 18.0f;
        else return;
        moveOrigin(x, y);
    }
}

void ofApp::draw() {
    ofBackground(asOfColor(design_.background));

    // Blend mode, fill, line width, and colors are local to this drawing block.
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    for (std::size_t palette = 0; palette < 2; ++palette) {
        const auto& marks = marks_[palette];
        ofSetLineWidth(palette == 0 ? 3.0f : 2.0f);
        for (std::size_t index = 1; index < marks.size(); ++index) {
            ofSetColor(asOfColor(marks[index].color));
            ofDrawLine(marks[index - 1].position.x, marks[index - 1].position.y,
                       marks[index].position.x, marks[index].position.y);
        }
        for (std::size_t index = 0; index < marks.size(); index += 4) {
            const auto& mark = marks[index];
            ofSetColor(asOfColor(mark.color));
            if (palette == 0) {
                ofFill();
                ofDrawCircle(mark.position.x, mark.position.y, mark.radius * 0.55f);
            } else {
                ofNoFill();
                ofDrawRectangle(mark.position.x - mark.radius * 0.5f,
                                mark.position.y - mark.radius * 0.5f,
                                mark.radius, mark.radius);
            }
        }
    }
    ofDisableBlendMode();
    ofPopStyle();

    if (ofGetWidth() >= 720 && ofGetHeight() >= 50) {
        ofPushStyle();
        ofSetColor(38, 42, 48);
        ofNoFill();
        ofDrawCircle(origin_.x, origin_.y, 8.0f);
        ofDrawLine(origin_.x - 11.0f, origin_.y, origin_.x + 11.0f, origin_.y);
        ofDrawBitmapString("circles: palette A | squares: palette B | pointer/arrows move | P pause | R reset | M reduce motion", 12, 22);
        ofPopStyle();
    }
}
