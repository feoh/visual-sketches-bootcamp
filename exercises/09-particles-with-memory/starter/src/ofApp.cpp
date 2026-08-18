#include "ofApp.h"

#include <algorithm>

namespace {

ofColor asColor(particles::Color color) {
    return ofColor(color.r, color.g, color.b);
}

}  // namespace

void ofApp::setup() {
    ofSetWindowTitle("Section 09 starter: memory ribbons");
    ofSetFrameRate(60);
    design_ = makeParticleDesign();
    resetScene();
}

void ofApp::resetScene() {
    const particles::Vec2 center{
        ofGetWidth() * 0.5f,
        ofGetHeight() * 0.5f
    };
    particles::reset(emitter_, center, 20260818u);
}

void ofApp::moveOrigin(float x, float y) {
    const float radius = design_.particle_radius;
    const float width = ofGetWidth();
    const float height = ofGetHeight();
    const particles::Bounds bounds{width, height};
    if (!particles::boundsAreUsable(bounds, radius)) {
        emitter_.origin = {width * 0.5f, height * 0.5f};
        return;
    }
    emitter_.origin = {
        std::clamp(x, radius, width - radius),
        std::clamp(y, radius, height - radius)
    };
}

void ofApp::update() {
    const particles::Bounds bounds{
        static_cast<float>(ofGetWidth()),
        static_cast<float>(ofGetHeight())
    };
    particles::advanceFrame(emitter_, ofGetLastFrameTime(), design_, bounds);
}

void ofApp::mousePressed(int x, int y, int) {
    moveOrigin(static_cast<float>(x), static_cast<float>(y));
}

void ofApp::mouseDragged(int x, int y, int) {
    moveOrigin(static_cast<float>(x), static_cast<float>(y));
}

void ofApp::windowResized(int, int) {
    resetScene();
}

void ofApp::keyPressed(int key) {
    if (key == 'p' || key == 'P') {
        particles::setPaused(emitter_, !emitter_.paused);
        return;
    }
    if (key == 'r' || key == 'R') {
        resetScene();
        return;
    }
    if (key == 'm' || key == 'M') {
        reduced_motion_ = !reduced_motion_;
        return;
    }

    float x = emitter_.origin.x;
    float y = emitter_.origin.y;
    if (key == OF_KEY_LEFT) {
        x -= 16.0f;
    } else if (key == OF_KEY_RIGHT) {
        x += 16.0f;
    } else if (key == OF_KEY_UP) {
        y -= 16.0f;
    } else if (key == OF_KEY_DOWN) {
        y += 16.0f;
    } else {
        return;
    }
    moveOrigin(x, y);
}

void ofApp::draw() {
    ofBackground(asColor(design_.background));
    if (!reduced_motion_) {
        ofNoFill();
        ofSetLineWidth(2.0f);
        for (const auto& particle : emitter_.particles) {
            ofSetColor(asColor(particles::colorForAge(particle, design_)));
            ofBeginShape();
            for (const auto& point : particle.history) {
                ofVertex(point.x, point.y);
            }
            ofEndShape(false);
        }
    }

    ofFill();
    for (const auto& particle : emitter_.particles) {
        ofSetColor(asColor(particles::colorForAge(particle, design_)));
        ofDrawCircle(particle.position.x,
                     particle.position.y,
                     design_.particle_radius);
    }

    ofNoFill();
    ofSetColor(asColor(design_.young_color));
    ofSetLineWidth(2.0f);
    ofDrawCircle(emitter_.origin.x, emitter_.origin.y, 8.0f);
    ofDrawBitmapString(
        "arrows/pointer move | P pause | R reset | M reduced trails",
        12,
        22);
}
