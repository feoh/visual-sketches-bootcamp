#include "ofApp.h"

#include <algorithm>
#include <cmath>

namespace {
ofColor asColor(flow::Color color) {
    return ofColor(color.r, color.g, color.b);
}
}

void ofApp::setup() {
    ofSetWindowTitle("Section 11 starter: coherent current");
    ofSetFrameRate(60);
    design_ = makeFlowFieldDesign();
    resetScene();
}

void ofApp::resetScene() {
    control_ = {ofGetWidth() * 0.5f, ofGetHeight() * 0.5f};
    flow::reset(system_,
                {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())},
                design_, 0x11f10u);
    moveControl(control_.x, control_.y);
}

void ofApp::moveControl(float x, float y) {
    const float inset = 14.0f;
    if (ofGetWidth() < inset * 2.0f || ofGetHeight() < inset * 2.0f) return;
    control_ = {std::clamp(x, inset, ofGetWidth() - inset),
                std::clamp(y, inset, ofGetHeight() - inset)};
    const flow::Vec2 offset{
        ofMap(control_.x, inset, ofGetWidth() - inset, -3.0f, 3.0f, true),
        ofMap(control_.y, inset, ofGetHeight() - inset, -3.0f, 3.0f, true)
    };
    flow::setFieldOffset(system_, offset);
}

void ofApp::update() {
    flow::advanceFrame(system_, ofGetLastFrameTime(), design_,
                       {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())});
}

void ofApp::mousePressed(int x, int y, int) {
    moveControl(static_cast<float>(x), static_cast<float>(y));
}

void ofApp::mouseDragged(int x, int y, int) {
    moveControl(static_cast<float>(x), static_cast<float>(y));
}

void ofApp::windowResized(int, int) {
    resetScene();
}

void ofApp::keyPressed(int key) {
    if (key == 'p' || key == 'P') {
        flow::setPaused(system_, !system_.paused);
        return;
    }
    if (key == 'r' || key == 'R') {
        resetScene();
        return;
    }
    if (key == 'm' || key == 'M') {
        flow::setReducedMotion(system_, !system_.reduced_motion);
        return;
    }
    if (key == 't' || key == 'T') {
        flow::setTimeFrozen(system_, !system_.time_frozen);
        return;
    }
    float x = control_.x;
    float y = control_.y;
    if (key == OF_KEY_LEFT) x -= 18.0f;
    else if (key == OF_KEY_RIGHT) x += 18.0f;
    else if (key == OF_KEY_UP) y -= 18.0f;
    else if (key == OF_KEY_DOWN) y += 18.0f;
    else return;
    moveControl(x, y);
}

void ofApp::draw() {
    ofBackground(asColor(design_.background));
    const flow::Bounds bounds{static_cast<float>(ofGetWidth()),
                              static_cast<float>(ofGetHeight())};
    if (!flow::boundsAreUsable(bounds, design_.particle_radius)) return;

    if (!system_.reduced_motion) {
        ofSetColor(asColor(design_.trail_color));
        ofSetLineWidth(1.5f);
        for (const auto& particle : system_.particles) {
            for (std::size_t index = 1; index < particle.history.size(); ++index) {
                const auto first = particle.history[index - 1];
                const auto second = particle.history[index];
                ofDrawLine(first.x, first.y, second.x, second.y);
            }
        }
    }

    ofSetColor(asColor(design_.head_color));
    ofSetLineWidth(2.0f);
    for (const auto& particle : system_.particles) {
        flow::Vec2 direction{1.0f, 0.0f};
        flow::sampleDirection(system_.field, particle.position, bounds, direction);
        const float angle = static_cast<float>(std::atan2(direction.y, direction.x) *
                                               180.0 / 3.14159265358979323846);
        ofPushMatrix();
        ofTranslate(particle.position.x, particle.position.y);
        ofRotateDeg(angle);
        ofNoFill();
        ofDrawTriangle(design_.particle_radius * 0.6f, 0.0f,
                       -design_.particle_radius * 0.6f,
                       -design_.particle_radius * 0.45f,
                       -design_.particle_radius * 0.6f,
                       design_.particle_radius * 0.45f);
        ofDrawLine(-design_.particle_radius * 0.4f, 0.0f,
                   design_.particle_radius * 0.7f, 0.0f);
        ofPopMatrix();
    }

    if (ofGetWidth() >= 720 && ofGetHeight() >= 48) {
        ofNoFill();
        if (system_.time_frozen) {
            ofDrawRectangle(control_.x - 9.0f, control_.y - 9.0f, 18.0f, 18.0f);
        } else {
            ofDrawCircle(control_.x, control_.y, 10.0f);
        }
        ofSetColor(35, 40, 48);
        ofDrawBitmapString("pointer/arrows shift field | P pause | R replay seed | M short memory | T freeze field", 12, 22);
    }
}
