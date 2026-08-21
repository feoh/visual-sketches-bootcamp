#include "ofApp.h"

#include <algorithm>

namespace {

void drawPrimitive(const signature::Primitive& primitive) {
    const float left = primitive.center.x - primitive.half_width;
    const float top = primitive.center.y - primitive.half_height;
    const float width = primitive.half_width * 2.0f;
    const float height = primitive.half_height * 2.0f;
    switch (primitive.kind) {
        case signature::PrimitiveKind::circle:
            ofDrawCircle(primitive.center.x, primitive.center.y,
                         std::min(primitive.half_width, primitive.half_height));
            break;
        case signature::PrimitiveKind::rectangle:
            ofDrawRectangle(left, top, width, height);
            break;
        case signature::PrimitiveKind::triangle:
            ofDrawTriangle(primitive.center.x, top, left, top + height, left + width, top + height);
            break;
        case signature::PrimitiveKind::line:
            ofDrawLine(left, top + height, left + width, top);
            break;
        case signature::PrimitiveKind::ellipse:
            ofDrawEllipse(primitive.center.x, primitive.center.y, width, height);
            break;
    }
}

}  // namespace

void ofApp::setup() {
    ofSetWindowTitle("Five-primitive visual signature — starter");
    ofSetBackgroundColor(248, 245, 236);
    ofSetLineWidth(7.0f);
    palette_ = makePalette();
    rebuild(ofGetWidth(), ofGetHeight());
}

void ofApp::draw() {
    for (const signature::Primitive& primitive : primitives_) {
        // Tests reject invalid color numbers. Use the first color only as a safe
        // drawing fallback if the app is opened before the tests are run.
        const std::size_t color_index = primitive.color_index < palette_.size()
                                            ? primitive.color_index
                                            : 0U;
        const signature::Color& color = palette_[color_index];
        ofSetColor(color.red, color.green, color.blue);
        drawPrimitive(primitive);
    }
}

void ofApp::windowResized(int width, int height) {
    rebuild(width, height);
}

void ofApp::rebuild(int width, int height) {
    primitives_ = signature::buildSignature(makeSignatureSpecs(),
                                             {static_cast<float>(width), static_cast<float>(height)});
}
