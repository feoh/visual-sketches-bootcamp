#include "ofApp.h"

namespace { ofColor colorFrom(responsiveposter::Color c) { return ofColor(c.r, c.g, c.b); } }

void ofApp::setup() {
    ofSetWindowTitle("Section 03 starter: responsive poster");
    ofSetCircleResolution(48);
    design_ = makePosterDesign();
    rebuild(ofGetWidth(), ofGetHeight());
}
void ofApp::windowResized(int width, int height) { rebuild(width, height); }
void ofApp::rebuild(int width, int height) { layout_ = responsiveposter::makeLayout(design_, {width, height}); }
void ofApp::draw() {
    ofBackground(colorFrom(design_.background));
    if (!layout_.valid) return;
    ofSetColor(colorFrom(design_.ink));
    ofDrawRectangle(layout_.panel.x, layout_.panel.y, layout_.panel.width, layout_.panel.height);
    ofSetColor(colorFrom(design_.accent));
    ofDrawCircle(layout_.focus_center.x, layout_.focus_center.y, layout_.focus_radius);
    ofSetLineWidth(4.0f);
    ofDrawLine(layout_.panel.x, layout_.panel.y + layout_.headline_size,
               layout_.panel.x + layout_.panel.width, layout_.panel.y + layout_.headline_size);
}
