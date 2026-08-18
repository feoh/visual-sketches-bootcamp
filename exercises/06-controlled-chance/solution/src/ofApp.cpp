#include "ofApp.h"

namespace {
ofColor colorFrom(controlledchance::Color color) {
    return ofColor(color.r, color.g, color.b);
}
}

void ofApp::setup() {
    ofSetWindowTitle("Section 06 solution: six seeded routes");
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
    if (key == 'r' || key == 'R') editions_ = controlledchance::generateEditions(design_);
    if (key == 'n' || key == 'N') { ++design_.seed; editions_ = controlledchance::generateEditions(design_); }
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
        ofPushMatrix();
        ofTranslate(static_cast<float>(index % 3) * panel_width,
                    static_cast<float>(index / 3) * panel_height);
        ofNoFill();
        ofSetColor(colorFrom(design_.accent));
        if (!scene.marks.empty()) {
            ofBeginShape();
            for (const auto& mark : scene.marks) ofVertex(mark.center.x, mark.center.y);
            ofEndShape(false);
        }
        for (const auto& mark : scene.marks) {
            ofSetColor(mark.motif == controlledchance::Motif::ring
                           ? colorFrom(design_.accent) : colorFrom(design_.ink));
            if (mark.motif == controlledchance::Motif::dot) {
                ofFill();
                ofDrawRectangle(mark.center.x - mark.radius * 0.5f,
                                mark.center.y - mark.radius * 0.5f,
                                mark.radius, mark.radius);
            } else if (mark.motif == controlledchance::Motif::dash) {
                ofDrawLine(mark.center.x, mark.center.y - mark.radius,
                           mark.center.x, mark.center.y + mark.radius);
                ofDrawLine(mark.center.x - mark.radius * 0.5f, mark.center.y,
                           mark.center.x + mark.radius * 0.5f, mark.center.y);
            } else {
                ofNoFill();
                ofDrawRectangle(mark.center.x - mark.radius, mark.center.y - mark.radius,
                                mark.radius * 2.0f, mark.radius * 2.0f);
            }
        }
        ofPopMatrix();
    }
}
