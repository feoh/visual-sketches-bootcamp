#include "ofApp.h"

namespace {
ofColor colorFrom(markfamily::Color color) { return ofColor(color.r, color.g, color.b); }
}  // namespace

void ofApp::setup() {
    ofSetWindowTitle("Section 02 solution: linked hourglass family");
    design_ = makeFamilyDesign();
    rebuild(ofGetWidth(), ofGetHeight());
}

void ofApp::windowResized(int width, int height) { rebuild(width, height); }

void ofApp::rebuild(int width, int height) {
    marks_ = markfamily::makeMarkFamily(design_, {width, height});
}

void ofApp::draw() {
    ofBackground(colorFrom(design_.background_color));
    ofNoFill();
    ofSetLineWidth(4.0f);
    ofSetColor(colorFrom(design_.accent_color));
    for (std::size_t index = 1; index < marks_.size(); ++index) {
        ofDrawLine(marks_[index - 1].center.x, marks_[index - 1].center.y,
                   marks_[index].center.x, marks_[index].center.y);
    }
    ofFill();
    for (const markfamily::Mark& mark : marks_) {
        const float x = mark.center.x;
        const float y = mark.center.y;
        const float r = mark.radius;
        ofSetColor(colorFrom(mark.family_index % 2 == 0 ? design_.mark_color : design_.accent_color));
        ofDrawTriangle(x - r, y - r, x + r, y - r, x, y);
        ofDrawTriangle(x - r, y + r, x + r, y + r, x, y);
    }
}
