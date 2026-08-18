#include "ofApp.h"

#include <algorithm>

namespace {

ofColor asColor(particles::Color color) {
    return ofColor(color.r, color.g, color.b);
}

void drawTriangleHead(const particles::Particle& particle,
                      const particles::Design& design) {
    const float normalized_age = particle.lifetime > 0.0f
        ? std::clamp(particle.age / particle.lifetime, 0.0f, 1.0f)
        : 1.0f;
    const float outer_size = design.particle_radius;
    const float inner_size = outer_size * (0.35f + 0.45f * normalized_age);
    const ofColor color = asColor(particles::colorForAge(particle, design));

    // The bounded inner size encodes age even when trails are hidden.
    ofFill();
    ofSetColor(color);
    ofDrawTriangle(
        particle.position.x,
        particle.position.y - inner_size,
        particle.position.x - inner_size,
        particle.position.y + inner_size,
        particle.position.x + inner_size,
        particle.position.y + inner_size);

    ofNoFill();
    ofSetLineWidth(1.5f);
    ofSetColor(color);
    ofDrawTriangle(
        particle.position.x,
        particle.position.y - outer_size,
        particle.position.x - outer_size,
        particle.position.y + outer_size,
        particle.position.x + outer_size,
        particle.position.y + outer_size);
}

}  // namespace

void ofApp::setup() {
    ofSetWindowTitle("Section 09 solution: memory constellations");
    ofSetFrameRate(60);
    design_ = makeParticleDesign();
    resetScene();
}

void ofApp::resetScene() {
    const particles::Vec2 center{
        ofGetWidth() * 0.5f,
        ofGetHeight() * 0.5f
    };
    particles::reset(emitter_, center, 90917u);
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
        x -= 20.0f;
    } else if (key == OF_KEY_RIGHT) {
        x += 20.0f;
    } else if (key == OF_KEY_UP) {
        y -= 20.0f;
    } else if (key == OF_KEY_DOWN) {
        y += 20.0f;
    } else {
        return;
    }
    moveOrigin(x, y);
}

void ofApp::draw() {
    ofBackground(asColor(design_.background));
    for (const auto& particle : emitter_.particles) {
        const ofColor color = asColor(particles::colorForAge(particle, design_));
        if (!reduced_motion_) {
            for (std::size_t index = 0;
                 index < particle.history.size();
                 index += 3) {
                const float amount =
                    static_cast<float>(index + 1) / particle.history.size();
                const float size = 1.0f + 3.0f * amount;
                ofFill();
                ofSetColor(color, static_cast<int>(40.0f + 170.0f * amount));
                ofDrawRectangle(
                    particle.history[index].x - size * 0.5f,
                    particle.history[index].y - size * 0.5f,
                    size,
                    size);
            }
        }
        drawTriangleHead(particle, design_);
    }

    ofNoFill();
    ofSetColor(asColor(design_.young_color));
    ofSetLineWidth(1.5f);
    ofDrawRectangle(
        emitter_.origin.x - 7.0f,
        emitter_.origin.y - 7.0f,
        14.0f,
        14.0f);
    ofDrawBitmapString(
        "arrows/pointer move | P pause | R reset | M reduced trails",
        12,
        22);
}
