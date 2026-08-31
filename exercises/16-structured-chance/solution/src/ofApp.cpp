#include "ofApp.h"

namespace {
ofColor familyColor(const StructuredChancePalette& palette, int role, int alpha) {
    return role == 0 ? ofColor(palette.first_r, palette.first_g, palette.first_b, alpha)
                     : ofColor(palette.second_r, palette.second_g, palette.second_b, alpha);
}
ofVec2f toScreen(structuredchance::Point point) {
    return {point.x * ofGetWidth(), point.y * ofGetHeight()};
}
}

void ofApp::setup() {
    ofSetWindowTitle("Section 16 solution: quiet windows");
    ofEnableAlphaBlending();
    settings_ = makeStructuredChanceSettings();
    palette_ = makeStructuredChancePalette();
    rebuild();
}
void ofApp::rebuild() { composition_ = structuredchance::generateComposition(settings_); }
void ofApp::keyPressed(int key) {
    if (key == 'r' || key == 'R') rebuild();
    if (key == 'n' || key == 'N') { settings_.seed += 7u; rebuild(); }
    if (key == 'g' || key == 'G') soft_layers_ = !soft_layers_;
    if (key == 'h' || key == 'H') show_help_ = !show_help_;
}
void ofApp::draw() {
    ofBackground(palette_.background_r, palette_.background_g, palette_.background_b);
    if (soft_layers_) ofEnableBlendMode(OF_BLENDMODE_ADD);
    for (const auto& region : composition_.regions) {
        for (const auto& stroke : region.strokes) {
            const ofVec2f a = toScreen(stroke.a);
            const ofVec2f b = toScreen(stroke.b);
            const float width = stroke.width * std::min(ofGetWidth(), ofGetHeight());
            if (soft_layers_) {
                ofSetColor(familyColor(palette_, stroke.palette_role, 16));
                ofSetLineWidth(width * 5.5f);
                ofDrawLine(a, b);
            }
            ofSetColor(familyColor(palette_, stroke.palette_role, 190));
            ofSetLineWidth(width * (region.quiet ? 0.35f : 0.7f));
            ofDrawLine(a, b);
        }
    }
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofNoFill();
    ofSetColor(palette_.background_r, palette_.background_g, palette_.background_b);
    ofSetLineWidth(7.0f);
    for (const auto& region : composition_.regions) {
        ofBeginShape();
        ofVertex(toScreen(region.quad.top_left));
        ofVertex(toScreen(region.quad.top_right));
        ofVertex(toScreen(region.quad.bottom_right));
        ofVertex(toScreen(region.quad.bottom_left));
        ofEndShape(true);
    }
    ofFill();
    if (show_help_) {
        ofSetColor(245);
        ofDrawBitmapStringHighlight("R replay   N seed + 7   G soft layers   H help", 18, 24,
                                    ofColor(0, 0, 0, 190), ofColor(245));
    }
}
