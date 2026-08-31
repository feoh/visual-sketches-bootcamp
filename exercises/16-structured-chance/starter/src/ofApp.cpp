#include "ofApp.h"

namespace {
ofColor colorFor(const StructuredChancePalette& palette, int role, int alpha) {
    if (role == 0) return {palette.first_r, palette.first_g, palette.first_b, alpha};
    return {palette.second_r, palette.second_g, palette.second_b, alpha};
}
ofVec2f screenPoint(structuredchance::Point point) {
    return {point.x * ofGetWidth(), point.y * ofGetHeight()};
}
}

void ofApp::setup() {
    ofSetWindowTitle("Section 16 starter: structured chance");
    ofEnableAlphaBlending();
    settings_ = makeStructuredChanceSettings();
    palette_ = makeStructuredChancePalette();
    rebuild();
}

void ofApp::rebuild() { composition_ = structuredchance::generateComposition(settings_); }

void ofApp::keyPressed(int key) {
    if (key == 'r' || key == 'R') rebuild();
    if (key == 'n' || key == 'N') { ++settings_.seed; rebuild(); }
    if (key == 'g' || key == 'G') soft_layers_ = !soft_layers_;
    if (key == 'h' || key == 'H') show_help_ = !show_help_;
}

void ofApp::draw() {
    ofBackground(palette_.background_r, palette_.background_g, palette_.background_b);
    for (const auto& region : composition_.regions) {
        ofPath panel;
        panel.moveTo(screenPoint(region.quad.top_left));
        panel.lineTo(screenPoint(region.quad.top_right));
        panel.lineTo(screenPoint(region.quad.bottom_right));
        panel.lineTo(screenPoint(region.quad.bottom_left));
        panel.close();
        panel.setFilled(true);
        panel.setFillColor(ofColor(12, 16, 36));
        panel.draw();

        if (soft_layers_) ofEnableBlendMode(OF_BLENDMODE_ADD);
        for (const auto& stroke : region.strokes) {
            const ofVec2f a = screenPoint(stroke.a);
            const ofVec2f b = screenPoint(stroke.b);
            const float width = stroke.width * std::min(ofGetWidth(), ofGetHeight());
            if (soft_layers_) {
                ofSetColor(colorFor(palette_, stroke.palette_role, 20));
                ofSetLineWidth(width * 4.0f);
                ofDrawLine(a, b);
                ofSetColor(colorFor(palette_, stroke.palette_role, 55));
                ofSetLineWidth(width * 2.0f);
                ofDrawLine(a, b);
            }
            ofSetColor(colorFor(palette_, stroke.palette_role, 210));
            ofSetLineWidth(width * 0.55f);
            ofDrawLine(a, b);
        }
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    }

    ofNoFill();
    ofSetColor(palette_.background_r, palette_.background_g, palette_.background_b);
    ofSetLineWidth(5.0f);
    for (const auto& region : composition_.regions) {
        ofBeginShape();
        ofVertex(screenPoint(region.quad.top_left));
        ofVertex(screenPoint(region.quad.top_right));
        ofVertex(screenPoint(region.quad.bottom_right));
        ofVertex(screenPoint(region.quad.bottom_left));
        ofEndShape(true);
    }
    ofFill();
    if (show_help_) {
        ofSetColor(245);
        ofDrawBitmapStringHighlight("R replay   N next seed   G soft layers   H help", 18, 24,
                                    ofColor(0, 0, 0, 190), ofColor(245));
    }
}
