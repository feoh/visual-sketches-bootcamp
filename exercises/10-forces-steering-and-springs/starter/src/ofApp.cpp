#include "ofApp.h"

#include <algorithm>

namespace {
constexpr float minimum_control_inset = 13.0f;

ofColor asColor(forces::Color color) {
    return ofColor(color.r, color.g, color.b);
}
}

void ofApp::setup() {
    ofSetWindowTitle("Section 10 starter: force modes");
    ofSetFrameRate(60);
    design_ = makeForceDesign();
    resetScene();
}

void ofApp::resetScene() {
    forces::reset(system_,
                  {ofGetWidth() * 0.5f, ofGetHeight() * 0.5f},
                  forces::BehaviorMode::seek,
                  design_,
                  {static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())});
    reduced_motion_ = false;
}

void ofApp::moveControl(float x, float y) {
    const forces::Bounds bounds{static_cast<float>(ofGetWidth()),
                                static_cast<float>(ofGetHeight())};
    const float control_inset =
        std::max(minimum_control_inset, design_.agent_radius);
    forces::Vec2 point{0.0f, 0.0f};
    if (!forces::clampPointToBounds({x, y}, bounds, control_inset, point)) {
        return;
    }
    system_.target = point;
    system_.anchor = point;
}

void ofApp::update() {
    forces::advanceFrame(system_,
                         ofGetLastFrameTime(),
                         design_,
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
    if (key == '1') {
        forces::setMode(system_, forces::BehaviorMode::seek);
        return;
    }
    if (key == '2') {
        forces::setMode(system_, forces::BehaviorMode::spring_chain);
        return;
    }
    if (key == ' ') {
        const auto next = system_.mode == forces::BehaviorMode::seek
            ? forces::BehaviorMode::spring_chain
            : forces::BehaviorMode::seek;
        forces::setMode(system_, next);
        return;
    }
    if (key == 'p' || key == 'P') {
        if (!reduced_motion_) {
            forces::setPaused(system_, !system_.paused);
        }
        return;
    }
    if (key == 'r' || key == 'R') {
        resetScene();
        return;
    }
    if (key == 'm' || key == 'M') {
        reduced_motion_ = !reduced_motion_;
        forces::setPaused(system_, reduced_motion_);
        return;
    }
    float x = system_.target.x;
    float y = system_.target.y;
    if (key == OF_KEY_LEFT) x -= 16.0f;
    else if (key == OF_KEY_RIGHT) x += 16.0f;
    else if (key == OF_KEY_UP) y -= 16.0f;
    else if (key == OF_KEY_DOWN) y += 16.0f;
    else return;
    moveControl(x, y);
}

void ofApp::draw() {
    ofBackground(asColor(design_.background));
    const bool spring = system_.mode == forces::BehaviorMode::spring_chain;
    const auto mode_color = spring ? design_.spring_color : design_.seek_color;
    const float mark_radius = std::max(1.0f, design_.agent_radius - 2.0f);
    const forces::Bounds bounds{static_cast<float>(ofGetWidth()),
                                static_cast<float>(ofGetHeight())};
    const float control_inset =
        std::max(minimum_control_inset, design_.agent_radius);
    const bool control_fits = forces::boundsAreUsable(bounds, control_inset);
    ofSetColor(asColor(mode_color));

    if (!reduced_motion_) {
        ofNoFill();
        ofSetLineWidth(spring ? 3.0f : 1.5f);
        if (spring && !system_.agents.empty()) {
            ofDrawLine(system_.anchor.x, system_.anchor.y,
                       system_.agents.front().position.x,
                       system_.agents.front().position.y);
            for (std::size_t i = 1; i < system_.agents.size(); ++i) {
                ofDrawLine(system_.agents[i - 1].position.x,
                           system_.agents[i - 1].position.y,
                           system_.agents[i].position.x,
                           system_.agents[i].position.y);
            }
        } else {
            for (const auto& agent : system_.agents) {
                ofDrawLine(agent.position.x, agent.position.y,
                           system_.target.x, system_.target.y);
            }
        }
    }

    ofFill();
    for (const auto& agent : system_.agents) {
        if (spring) {
            ofDrawTriangle(agent.position.x, agent.position.y - mark_radius,
                           agent.position.x - mark_radius, agent.position.y + mark_radius,
                           agent.position.x + mark_radius, agent.position.y + mark_radius);
        } else {
            ofDrawCircle(agent.position.x, agent.position.y, mark_radius);
        }
    }
    if (control_fits) {
        ofNoFill();
        ofSetLineWidth(2.0f);
        if (spring) {
            ofDrawRectangle(system_.anchor.x - 10.0f,
                            system_.anchor.y - 10.0f,
                            20.0f,
                            20.0f);
        } else {
            ofDrawCircle(system_.target.x, system_.target.y, 11.0f);
        }
    }
    ofSetColor(35, 40, 45);
    ofDrawBitmapString("pointer/arrows target or anchor | 1 seek circles/ring | 2 spring triangles/square | space switch | P/R/M", 12, 22);
}
