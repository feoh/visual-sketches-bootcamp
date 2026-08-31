#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("Section 17 solution: two-light archipelago");
    ofSetSphereResolution(7);
    settings_ = makePopulationSettings();
    palette_ = makePopulationPalette();
    camera_.setDistance(570.0f);
    warm_light_.setDirectional();
    warm_light_.setOrientation({35.0f, -42.0f, 0.0f});
    warm_light_.setDiffuseColor(ofColor(palette_.warm_r, palette_.warm_g, palette_.warm_b));
    cool_light_.setPointLight();
    cool_light_.setDiffuseColor(ofColor(palette_.cool_r, palette_.cool_g, palette_.cool_b));
    warm_material_.setDiffuseColor(ofColor(229, 191, 189));
    warm_material_.setShininess(12.0f);
    cool_material_.setDiffuseColor(ofColor(77, 135, 150));
    cool_material_.setShininess(32.0f);
    rebuild();
}
void ofApp::rebuild() { population_ = lightpopulation::generatePopulation(settings_); }
void ofApp::update() { if (!paused_ && !reduced_detail_) turn_degrees_ -= ofGetLastFrameTime() * 2.6f; }
void ofApp::keyPressed(int key) {
    if (key == 'r' || key == 'R') { turn_degrees_ = 0.0f; rebuild(); }
    if (key == 'n' || key == 'N') { settings_.seed += 11u; rebuild(); }
    if (key == 'p' || key == 'P') paused_ = !paused_;
    if (key == 'm' || key == 'M') reduced_detail_ = !reduced_detail_;
    if (key == 'h' || key == 'H') show_help_ = !show_help_;
}
void ofApp::draw() {
    ofBackground(0);
    ofEnableDepthTest();
    camera_.begin();
    ofRotateYDeg(turn_degrees_);
    cool_light_.setPosition(180.0f, 90.0f, 240.0f);
    ofEnableLighting();
    warm_light_.enable();
    cool_light_.enable();
    for (std::size_t index = 0; index < population_.pebbles.size(); ++index) {
        if (reduced_detail_ && index % 4u != 0u) continue;
        const auto& pebble = population_.pebbles[index];
        ofPushMatrix();
        ofTranslate(pebble.position.x, pebble.position.y, pebble.position.z);
        ofRotateXDeg(pebble.rotation_degrees.x);
        ofRotateYDeg(pebble.rotation_degrees.y);
        ofRotateZDeg(pebble.rotation_degrees.z);
        ofScale(pebble.scale.x, pebble.scale.y, pebble.scale.z);
        if (pebble.material_role == 0) warm_material_.begin(); else cool_material_.begin();
        ofDrawSphere(1.0f);
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
        ofDrawBitmapStringHighlight("drag: orbit   R replay   N seed + 11   P pause   M reduced detail   H help",
                                    18, 24, ofColor(0, 0, 0, 210), ofColor(245));
    }
}
