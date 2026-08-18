#include "ofApp.h"

namespace {
class MatrixScope {
public:
    MatrixScope() { ofPushMatrix(); }
    ~MatrixScope() { ofPopMatrix(); }
    MatrixScope(const MatrixScope&) = delete;
    MatrixScope& operator=(const MatrixScope&) = delete;
};
ofColor colorFrom(sculpture::Color color) { return ofColor(color.r, color.g, color.b); }
void drawLocalSculpture(const sculpture::Design& design, const sculpture::Scene& scene,
                        float panel_x, float panel_y, int panel_width, int panel_height) {
    MatrixScope panel_scope;
    ofTranslate(panel_x + panel_width * 0.5f, panel_y + panel_height * 0.5f);
    ofScale(scene.model_scale, scene.model_scale);
    ofRotateDeg(scene.root_angle_degrees);
    ofSetColor(colorFrom(design.ink));
    ofDrawLine(0.0f, 0.0f, design.first_length, 0.0f);
    ofFill();
    ofDrawCircle(0.0f, 0.0f, design.ornament_radius * 0.6f);
    {
        MatrixScope child_scope;
        ofTranslate(design.first_length, 0.0f);
        ofRotateDeg(scene.child_angle_degrees);
        ofSetColor(colorFrom(design.accent));
        ofDrawLine(0.0f, 0.0f, design.second_length, 0.0f);
        ofNoFill();
        ofDrawCircle(design.second_length, 0.0f, design.ornament_radius);
    }  // child_scope restores the parent's coordinate system, even on early return.
}
}
void ofApp::setup() {
    ofSetWindowTitle("Section 07 starter: three local-coordinate frames");
    design_ = makeSculptureDesign();
    rebuild();
}
void ofApp::update() {}
void ofApp::windowResized(int, int) { rebuild(); }
void ofApp::rebuild() {
    const sculpture::Viewport panel{ofGetWidth() / 3, ofGetHeight()};
    scenes_[0] = sculpture::makeScene(design_, panel, 0.0f);
    scenes_[1] = sculpture::makeScene(design_, panel, design_.period_seconds * 0.25f);
    scenes_[2] = sculpture::makeScene(design_, panel, design_.period_seconds * 0.5f);
}
void ofApp::draw() {
    ofBackground(colorFrom(design_.background));
    const int panel_width = ofGetWidth() / 3;
    ofSetLineWidth(design_.stroke_width);
    for (std::size_t index = 0; index < scenes_.size(); ++index) {
        if (!scenes_[index].valid) continue;
        drawLocalSculpture(design_, scenes_[index], static_cast<float>(index * panel_width),
                           0.0f, panel_width, ofGetHeight());
    }
}
