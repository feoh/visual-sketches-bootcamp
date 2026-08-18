#include "ofApp.h"

#include <algorithm>
#include <cmath>

namespace {
ofColor asColor(flow::Color color) { return ofColor(color.r, color.g, color.b); }
}

void ofApp::setup() {
    ofSetWindowTitle("Section 11 solution: woven weather");
    ofSetFrameRate(60);
    design_ = makeFlowFieldDesign();
    resetScene();
}

void ofApp::resetScene() {
    control_ = {ofGetWidth() * 0.35f, ofGetHeight() * 0.62f};
    flow::reset(system_,
                {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())},
                design_, 0x71a11u);
    moveControl(control_.x, control_.y);
}

void ofApp::moveControl(float x, float y) {
    const float inset = 16.0f;
    if (ofGetWidth() < inset * 2.0f || ofGetHeight() < inset * 2.0f) return;
    control_ = {std::clamp(x, inset, ofGetWidth() - inset),
                std::clamp(y, inset, ofGetHeight() - inset)};
    flow::setFieldOffset(system_, {
        ofMap(control_.x, inset, ofGetWidth() - inset, -4.0f, 4.0f, true),
        ofMap(control_.y, inset, ofGetHeight() - inset, 4.0f, -4.0f, true)
    });
}

void ofApp::update() {
    flow::advanceFrame(system_, ofGetLastFrameTime(), design_,
                       {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())});
}
void ofApp::mousePressed(int x, int y, int) { moveControl(static_cast<float>(x), static_cast<float>(y)); }
void ofApp::mouseDragged(int x, int y, int) { moveControl(static_cast<float>(x), static_cast<float>(y)); }
void ofApp::windowResized(int, int) { resetScene(); }

void ofApp::keyPressed(int key) {
    if (key == 'p' || key == 'P') flow::setPaused(system_, !system_.paused);
    else if (key == 'r' || key == 'R') resetScene();
    else if (key == 'm' || key == 'M') flow::setReducedMotion(system_, !system_.reduced_motion);
    else if (key == 't' || key == 'T') flow::setTimeFrozen(system_, !system_.time_frozen);
    else {
        float x = control_.x;
        float y = control_.y;
        if (key == OF_KEY_LEFT) x -= 20.0f;
        else if (key == OF_KEY_RIGHT) x += 20.0f;
        else if (key == OF_KEY_UP) y -= 20.0f;
        else if (key == OF_KEY_DOWN) y += 20.0f;
        else return;
        moveControl(x, y);
    }
}

void ofApp::draw() {
    ofBackground(asColor(design_.background));
    const flow::Bounds bounds{static_cast<float>(ofGetWidth()),
                              static_cast<float>(ofGetHeight())};
    if (!flow::boundsAreUsable(bounds, design_.particle_radius)) return;
    if (!system_.reduced_motion) {
        ofSetColor(asColor(design_.trail_color));
        ofSetLineWidth(1.4f);
        for (const auto& particle : system_.particles) {
            for (std::size_t index = 2; index < particle.history.size(); index += 2) {
                const auto previous = particle.history[index - 1];
                const auto point = particle.history[index];
                const float dx = point.x - previous.x;
                const float dy = point.y - previous.y;
                const float length = std::hypot(dx, dy);
                if (length <= 0.0001f) continue;
                const float half_bar = design_.particle_radius * 0.6f;
                const float nx = -dy / length * half_bar;
                const float ny = dx / length * half_bar;
                ofDrawLine(point.x - nx, point.y - ny,
                           point.x + nx, point.y + ny);
            }
        }
    }

    ofSetColor(asColor(design_.head_color));
    ofSetLineWidth(2.2f);
    for (const auto& particle : system_.particles) {
        flow::Vec2 direction{1.0f, 0.0f};
        flow::sampleDirection(system_.field, particle.position, bounds, direction);
        const float angle = static_cast<float>(std::atan2(direction.y, direction.x) *
                                               180.0 / 3.14159265358979323846);
        ofPushMatrix();
        ofTranslate(particle.position.x, particle.position.y);
        ofRotateDeg(angle + 45.0f);
        ofNoFill();
        ofDrawRectangle(-design_.particle_radius * 0.45f,
                        -design_.particle_radius * 0.45f,
                        design_.particle_radius * 0.9f,
                        design_.particle_radius * 0.9f);
        ofPopMatrix();
        ofDrawLine(particle.position.x,
                   particle.position.y,
                   particle.position.x + direction.x * design_.particle_radius * 0.7f,
                   particle.position.y + direction.y * design_.particle_radius * 0.7f);
    }

    if (ofGetWidth() >= 800 && ofGetHeight() >= 48) {
        ofNoFill();
        if (system_.time_frozen) {
            ofDrawLine(control_.x - 10.0f, control_.y - 10.0f,
                       control_.x + 10.0f, control_.y + 10.0f);
            ofDrawLine(control_.x + 10.0f, control_.y - 10.0f,
                       control_.x - 10.0f, control_.y + 10.0f);
        } else {
            ofDrawRectangle(control_.x - 9.0f, control_.y - 9.0f, 18.0f, 18.0f);
        }
        ofSetColor(232, 236, 240);
        ofDrawBitmapString("pointer/arrows move weather coordinates | P pause | R replay | M still heads | T freeze field", 12, 22);
    }
}
