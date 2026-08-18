#include "ofApp.h"

namespace {
ofColor colorFrom(controlledchance::Color color) {
    return ofColor(color.r, color.g, color.b);
}
}

void ofApp::setup() {
    ofSetWindowTitle("Section 06 starter: seeded paper samples");
    ofSetCircleResolution(36);
    design_ = makeEditionDesign();
    editions_ = controlledchance::generateEditions(design_);
    rebuild();
}
void ofApp::update() {}
void ofApp::windowResized(int, int) { rebuild(); }
void ofApp::rebuild() {
    scenes_.clear();
    const controlledchance::Viewport panel{ofGetWidth() / 3, ofGetHeight() / 2};
    for (int index = 0; index < controlledchance::edition_count; ++index) {
        scenes_.push_back(controlledchance::makeScene(editions_, index, panel));
    }
}
void ofApp::keyPressed(int key) {
    if (key == 'r' || key == 'R') {
        editions_ = controlledchance::generateEditions(design_);  // Same seed, same build.
    }
    if (key == 'n' || key == 'N') {
        ++design_.seed;
        editions_ = controlledchance::generateEditions(design_);
    }
    if (key == 's' || key == 'S') {
        ofBuffer buffer;
        buffer.set(controlledchance::serializeEditions(editions_));
        ofBufferToFile(ofToDataPath("edition-parameters.txt", true), buffer);
    }
    rebuild();
}
void ofApp::draw() {
    ofBackground(colorFrom(design_.background));
    const float panel_width = static_cast<float>(ofGetWidth() / 3);
    const float panel_height = static_cast<float>(ofGetHeight() / 2);
    ofSetLineWidth(3.0f);
    for (std::size_t index = 0; index < scenes_.size(); ++index) {
        const auto& scene = scenes_[index];
        if (!scene.valid) continue;
        const float offset_x = static_cast<float>(index % 3) * panel_width;
        const float offset_y = static_cast<float>(index / 3) * panel_height;
        ofPushMatrix();
        ofTranslate(offset_x, offset_y);
        ofNoFill();
        ofSetColor(colorFrom(design_.ink));
        ofDrawRectangle(1.5f, 1.5f, panel_width - 3.0f, panel_height - 3.0f);
        for (const auto& mark : scene.marks) {
            if (mark.motif == controlledchance::Motif::dot) {
                ofFill();
                ofSetColor(colorFrom(design_.accent));
                ofDrawCircle(mark.center.x, mark.center.y, mark.radius);
            } else if (mark.motif == controlledchance::Motif::dash) {
                ofSetColor(colorFrom(design_.ink));
                ofDrawLine(mark.center.x - mark.radius, mark.center.y,
                           mark.center.x + mark.radius, mark.center.y);
            } else {
                ofNoFill();
                ofSetColor(colorFrom(design_.accent));
                ofDrawCircle(mark.center.x, mark.center.y, mark.radius);
            }
        }
        ofPopMatrix();
    }
}
