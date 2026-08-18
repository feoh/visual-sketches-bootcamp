#include "ofApp.h"

namespace {
ofColor colorFrom(markfamily::Color color) { return ofColor(color.r, color.g, color.b); }
}  // namespace

void ofApp::setup() {
    ofSetWindowTitle("Section 02 starter: family of marks");
    ofSetCircleResolution(40);
    design_ = makeFamilyDesign();
    rebuild(ofGetWidth(), ofGetHeight());
}

void ofApp::windowResized(int width, int height) { rebuild(width, height); }

void ofApp::rebuild(int width, int height) {
    marks_ = markfamily::makeMarkFamily(design_, {width, height});
}

void ofApp::draw() {
    ofBackground(colorFrom(design_.background_color));
    for (const markfamily::Mark& mark : marks_) {
        ofSetColor(colorFrom(design_.mark_color));
        ofDrawCircle(mark.center.x, mark.center.y, mark.radius);
        ofSetColor(colorFrom(design_.accent_color));
        ofSetLineWidth(3.0f);
        ofDrawLine(mark.center.x, mark.center.y - mark.radius,
                   mark.center.x, mark.center.y + mark.radius);
    }
}
