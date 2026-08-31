#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("Section 17 starter: depth, light, and populations");
    ofSetSphereResolution(8);
    ofSetVerticalSync(true);
    settings_ = makePopulationSettings();
    palette_ = makePopulationPalette();
    camera_.setDistance(520.0f);
    warm_light_.setPointLight();
    cool_light_.setPointLight();
    warm_light_.setDiffuseColor(ofColor(palette_.warm_r, palette_.warm_g, palette_.warm_b));
    cool_light_.setDiffuseColor(ofColor(palette_.cool_r, palette_.cool_g, palette_.cool_b));
    warm_material_.setDiffuseColor(ofColor(238, 184, 177));
    warm_material_.setShininess(28.0f);
    cool_material_.setDiffuseColor(ofColor(139, 190, 203));
    cool_material_.setShininess(18.0f);
    rebuild();
}
void ofApp::rebuild() { population_ = lightpopulation::generatePopulation(settings_); }
void ofApp::update() {
    if (!paused_ && !reduced_detail_) turn_degrees_ += ofGetLastFrameTime() * 4.0f;
}
void ofApp::keyPressed(int key) {
    if (key == 'r' || key == 'R') { turn_degrees_ = 0.0f; rebuild(); }
    if (key == 'n' || key == 'N') { ++settings_.seed; rebuild(); }
    if (key == 'p' || key == 'P') paused_ = !paused_;
    if (key == 'm' || key == 'M') reduced_detail_ = !reduced_detail_;
    if (key == 'h' || key == 'H') show_help_ = !show_help_;
}
void ofApp::draw() {
    ofBackground(0);
    ofEnableDepthTest();
    camera_.begin();
    ofRotateYDeg(turn_degrees_);
    warm_light_.setPosition(-220.0f, -180.0f, 260.0f);
    cool_light_.setPosition(230.0f, 140.0f, 120.0f);
    ofEnableLighting();
    warm_light_.enable();
    cool_light_.enable();
    for (std::size_t index = 0; index < population_.pebbles.size(); ++index) {
        if (reduced_detail_ && index % 3u != 0u) continue;
        const auto& pebble = population_.pebbles[index];
        ofPushMatrix();
        ofTranslate(pebble.position.x, pebble.position.y, pebble.position.z);
        ofRotateXDeg(pebble.rotation_degrees.x);
        ofRotateYDeg(pebble.rotation_degrees.y);
        ofRotateZDeg(pebble.rotation_degrees.z);
        ofScale(pebble.scale.x, pebble.scale.y, pebble.scale.z);
        if (pebble.material_role == 0) warm_material_.begin(); else cool_material_.begin();
        ofDrawSphere(0.0f, 0.0f, 0.0f, 1.0f);
        if (pebble.material_role == 0) warm_material_.end(); else cool_material_.end();
        ofPopMatrix();
    }
    warm_light_.disable();
    cool_light_.disable();
    ofDisableLighting();
    camera_.end();
    ofDisableDepthTest();
    if (show_help_) {
        ofSetColor(245);
        ofDrawBitmapStringHighlight("drag: orbit   R replay   N next seed   P pause   M reduced detail   H help",
                                    18, 24, ofColor(0, 0, 0, 210), ofColor(245));
    }
}
