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
    ofSetWindowTitle("Section 12 solution: additive star loom");
    ofSetFrameRate(60);
    design_ = makeTrailDesign();
    resetStudy();
}

void ofApp::resetStudy() {
    trails_[0].clear();
    trails_[1].clear();
    marks_[0].clear();
    marks_[1].clear();
    origin_ = {ofGetWidth() * 0.46f, ofGetHeight() * 0.54f};
    phase_ = 0.0f;
    paused_ = false;
    reduced_motion_ = false;
}

void ofApp::moveOrigin(float x, float y) {
    const float inset = design_.maximum_radius + 4.0f;
    if (ofGetWidth() < inset * 2.0f || ofGetHeight() < inset * 2.0f) return;
    origin_ = {std::clamp(x, inset, ofGetWidth() - inset),
               std::clamp(y, inset, ofGetHeight() - inset)};
}

void ofApp::update() {
    if (!paused_) {
        phase_ += std::clamp(static_cast<float>(ofGetLastFrameTime()), 0.0f, 0.05f) * 1.15f;
        const float reach = std::max(0.0f,
            std::min(ofGetWidth(), ofGetHeight()) * 0.34f - design_.maximum_radius);
        const float rose = std::cos(3.0f * phase_);
        const colortrail::Vec2 first{
            origin_.x + std::cos(phase_) * rose * reach,
            origin_.y + std::sin(phase_) * rose * reach};
        const colortrail::Vec2 second{
            origin_.x + std::cos(-phase_ + pi * 0.25f) * rose * reach * 0.82f,
            origin_.y + std::sin(-phase_ + pi * 0.25f) * rose * reach * 0.82f};
        colortrail::appendSample(trails_[0],
            {first, 0.5f + 0.5f * std::sin(phase_ * 1.9f), 0},
            design_.maximum_history);
        colortrail::appendSample(trails_[1],
            {second, 0.5f + 0.5f * std::cos(phase_ * 1.4f), 1},
            design_.maximum_history);
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
        if (key == OF_KEY_LEFT) x -= 20.0f;
        else if (key == OF_KEY_RIGHT) x += 20.0f;
        else if (key == OF_KEY_UP) y -= 20.0f;
        else if (key == OF_KEY_DOWN) y += 20.0f;
        else return;
        moveOrigin(x, y);
    }
}

void ofApp::draw() {
    ofBackground(asOfColor(design_.background));

    // Additive blend and every style mutation are explicitly bracketed.
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofSetLineWidth(1.25f);
    for (std::size_t palette = 0; palette < 2; ++palette) {
        const auto& marks = marks_[palette];
        for (std::size_t index = 0; index < marks.size(); index += 3) {
            const auto& mark = marks[index];
            ofSetColor(asOfColor(mark.color));
            const float spoke = mark.radius * 2.4f;
            ofDrawLine(mark.position.x, mark.position.y,
                       mark.position.x + (mark.position.x - origin_.x) * 0.035f,
                       mark.position.y + (mark.position.y - origin_.y) * 0.035f);
            ofPushMatrix();
            ofTranslate(mark.position.x, mark.position.y);
            ofRotateDeg(static_cast<float>(mark.age_steps * 11 + palette * 45));
            ofNoFill();
            if (palette == 0) {
                ofDrawLine(-spoke, -spoke, spoke, spoke);
                ofDrawLine(-spoke, spoke, spoke, -spoke);
            } else {
                ofDrawLine(0.0f, -spoke, spoke, 0.0f);
                ofDrawLine(spoke, 0.0f, 0.0f, spoke);
                ofDrawLine(0.0f, spoke, -spoke, 0.0f);
                ofDrawLine(-spoke, 0.0f, 0.0f, -spoke);
            }
            ofPopMatrix();
        }
    }
    ofDisableBlendMode();
    ofPopStyle();

    if (ofGetWidth() >= 760 && ofGetHeight() >= 50) {
        ofPushStyle();
        ofSetColor(225, 232, 248);
        ofNoFill();
        ofDrawRectangle(origin_.x - 7.0f, origin_.y - 7.0f, 14.0f, 14.0f);
        ofDrawBitmapString("crosses: palette A | diamonds: palette B | additive overlap | pointer/arrows move | P pause | R reset | M still", 12, 22);
        ofPopStyle();
    }
}
