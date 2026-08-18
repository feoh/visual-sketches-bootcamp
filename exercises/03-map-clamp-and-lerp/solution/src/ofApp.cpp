#include "ofApp.h"

#include <algorithm>

namespace { ofColor colorFrom(responsiveposter::Color c) { return ofColor(c.r, c.g, c.b); } }

void ofApp::setup() {
    ofSetWindowTitle("Section 03 solution: orbit poster");
    ofSetCircleResolution(64);
    design_ = makePosterDesign();
    rebuild(ofGetWidth(), ofGetHeight());
}
void ofApp::windowResized(int width, int height) { rebuild(width, height); }
void ofApp::rebuild(int width, int height) { layout_ = responsiveposter::makeLayout(design_, {width, height}); }
void ofApp::draw() {
    ofBackground(colorFrom(design_.background));
    if (!layout_.valid) return;
    const auto& p = layout_.panel;
    ofNoFill();
    constexpr float orbit_stroke_width = 5.0f;
    ofSetLineWidth(orbit_stroke_width);
    ofSetColor(colorFrom(design_.ink));
    ofDrawRectangle(p.x, p.y, p.width, p.height);
    constexpr int ring_count = 3;
    const float drawable_distance = std::max(
        0.0f, responsiveposter::availableFocusDistance(layout_) - orbit_stroke_width * 0.5f);
    const float ring_step =
        std::min(layout_.focus_radius, drawable_distance / static_cast<float>(ring_count));
    for (int ring = ring_count; ring >= 1; --ring) {
        const float scale = static_cast<float>(ring);
        ofDrawCircle(layout_.focus_center.x, layout_.focus_center.y, ring_step * scale);
    }
    ofFill();
    ofSetColor(colorFrom(design_.accent));
    ofDrawTriangle(p.x, p.y + p.height, layout_.focus_center.x, layout_.focus_center.y,
                   p.x + p.width, p.y + p.height);
    ofSetColor(colorFrom(design_.background));
    ofDrawCircle(layout_.focus_center.x, layout_.focus_center.y, layout_.focus_radius);
}
