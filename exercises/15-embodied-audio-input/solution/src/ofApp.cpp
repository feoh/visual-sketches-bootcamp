#include "ofApp.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>

namespace {
constexpr std::array<float, 6> kRecordedAmplitudes{{0.0f, 0.05f, 0.4f, 1.0f, 0.0f, 0.2f}};
ofColor asColor(embodied::Color color) { return ofColor(color.r, color.g, color.b); }
bool bitmapTextFits(const char* text, int viewport_width) {
    int length = 0;
    while (text[length] != '\0') ++length;
    return length * 8 <= std::max(0, viewport_width - 8);
}
const char* fittedInputStatus(embodied::InputSource source, int viewport_width) {
    return embodied::compactInputStatusForWidth(source, viewport_width);
}
}

void ofApp::setup() {
    ofSetWindowTitle("Section 15 solution: voice-woven horizon");
    ofSetFrameRate(60);
    design_ = makeAudioInstrumentDesign();
    chooseNoDevice();  // Safe default: opening a microphone always requires L.
}

void ofApp::stopMicrophone() {
    if (microphone_open_) {
        sound_stream_.stop();
        sound_stream_.close();
        microphone_open_ = false;
    }
    pending_amplitude_.store(-1.0f);
}

void ofApp::chooseNoDevice() {
    stopMicrophone();
    embodied::reset(state_, design_, embodied::InputSource::no_device);
    applyFallback();
}

void ofApp::chooseRecorded() {
    stopMicrophone();
    embodied::reset(state_, design_, embodied::InputSource::recorded);
    fixture_index_ = 0;
    fixture_hold_ = 0;
}

void ofApp::tryLiveMicrophone() {
    stopMicrophone();
    ofSoundStreamSettings settings;
    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.bufferSize = 256;
    settings.numInputChannels = 1;
    settings.numOutputChannels = 0;
    if (!sound_stream_.setup(settings)) {
        chooseNoDevice();
        return;
    }
    microphone_open_ = true;
    embodied::reset(state_, design_, embodied::InputSource::live_microphone);
}

void ofApp::applyFallback() {
    embodied::setNoDeviceFallback(state_, fallback_amplitude_, design_);
}

void ofApp::update() {
    if (paused_) return;
    if (state_.source == embodied::InputSource::recorded) {
        if (++fixture_hold_ >= 18) {
            fixture_hold_ = 0;
            embodied::consumeAmplitude(state_, kRecordedAmplitudes[fixture_index_], design_);
            fixture_index_ = (fixture_index_ + 1) % kRecordedAmplitudes.size();
        }
    } else if (state_.source == embodied::InputSource::live_microphone) {
        const float amplitude = pending_amplitude_.exchange(-1.0f);
        if (amplitude >= 0.0f)
            embodied::consumeAmplitude(state_, amplitude, design_);
    }
}

void ofApp::audioIn(ofSoundBuffer& input) {
    const std::size_t channels = input.getNumChannels();
    const std::size_t frames = std::min<std::size_t>(input.getNumFrames(), 4096);
    if (channels == 0 || frames == 0) return;
    double sum = 0.0;
    for (std::size_t frame = 0; frame < frames; ++frame) {
        const float sample = input[frame * channels];
        sum += static_cast<double>(sample) * sample;
    }
    pending_amplitude_.store(static_cast<float>(std::sqrt(sum / frames)));
}

const char* ofApp::sourceLabel() const {
    if (state_.source == embodied::InputSource::recorded) return "RECORDED AMPLITUDE (NO AUDIO)";
    if (state_.source == embodied::InputSource::live_microphone) return "LIVE MICROPHONE";
    return "NO DEVICE: KEYBOARD FALLBACK";
}

void ofApp::draw() {
    ofBackground(asColor(design_.background));
    const int viewport_width = ofGetWidth();
    const int viewport_height = ofGetHeight();
    if (viewport_width < 680 || viewport_height < 360) {
        ofSetColor(asColor(design_.quiet_color));
        const char* status = fittedInputStatus(state_.source, viewport_width);
        if (viewport_height >= 12 && status[0] != '\0') {
            if (viewport_height >= 44 && bitmapTextFits("RESIZE TO 680 x 360", viewport_width)) {
                ofDrawBitmapString("RESIZE TO 680 x 360", 4, 18);
                ofDrawBitmapString(status, 4, 38);
            } else {
                const int baseline = std::min(viewport_height - 2,
                                              std::max(10, viewport_height / 2));
                ofDrawBitmapString(status, 4, baseline);
            }
        }
        return;
    }
    const float center_x = ofGetWidth() * 0.5f;
    const float center_y = ofGetHeight() * 0.53f;
    const auto& geometry = state_.geometry;
    const ofColor mark = geometry.active ? asColor(design_.active_color)
                                         : asColor(design_.quiet_color);
    ofSetColor(mark);
    ofSetLineWidth(2.0f);

    // Unlike the starter sun, loudness opens a horizontal woven horizon.
    const int marks = reduced_motion_ ? design_.minimum_rays : geometry.ray_count;
    const float span = geometry.radius * 2.0f;
    for (int index = 0; index < marks; ++index) {
        const float portion = marks == 1 ? 0.5f
            : static_cast<float>(index) / static_cast<float>(marks - 1);
        const float x = center_x - geometry.radius + portion * span;
        const float y = center_y + std::sin(portion * TWO_PI * 2.0f) *
            geometry.normalized_level * 52.0f;
        const float size = 4.0f + geometry.normalized_level * 12.0f;
        ofPushMatrix();
        ofTranslate(x, y);
        ofRotateDeg(45.0f);
        ofNoFill();
        ofDrawRectangle(-size * 0.5f, -size * 0.5f, size, size);
        ofPopMatrix();
    }
    ofDrawLine(center_x - design_.maximum_radius, center_y,
               center_x + design_.maximum_radius, center_y);
    ofFill();
    ofDrawTriangle(center_x + geometry.radius, center_y,
                   center_x + geometry.radius - 12.0f, center_y - 7.0f,
                   center_x + geometry.radius - 12.0f, center_y + 7.0f);

    ofSetColor(asColor(design_.quiet_color));
    ofDrawBitmapString("LOUDER  ->  WIDER HORIZON + MORE DIAMONDS", 24, 30);
    ofDrawBitmapString(sourceLabel(), 24, 54);
    ofDrawBitmapString(geometry.active ? "ACTIVITY: ACTIVE" : "ACTIVITY: QUIET", 24, 78);
    ofDrawBitmapString("N no-device | Up/Down set fallback | F replay fixture | L ask for microphone", 24, ofGetHeight() - 42);
    ofDrawBitmapString("P pause | R reset source | M reduce repeated marks | no sound is stored", 24, ofGetHeight() - 20);
}

void ofApp::keyPressed(int key) {
    if (key == 'n' || key == 'N') chooseNoDevice();
    else if (key == 'f' || key == 'F') chooseRecorded();
    else if (key == 'l' || key == 'L') tryLiveMicrophone();
    else if (key == 'p' || key == 'P') paused_ = !paused_;
    else if (key == 'm' || key == 'M') reduced_motion_ = !reduced_motion_;
    else if (key == 'r' || key == 'R') {
        if (state_.source == embodied::InputSource::recorded) chooseRecorded();
        else if (state_.source == embodied::InputSource::live_microphone) tryLiveMicrophone();
        else chooseNoDevice();
    } else if (key == OF_KEY_UP || key == OF_KEY_DOWN) {
        fallback_amplitude_ = std::clamp(fallback_amplitude_ +
            (key == OF_KEY_UP ? 0.1f : -0.1f), 0.0f, 1.0f);
        chooseNoDevice();
    }
}

void ofApp::exit() { stopMicrophone(); }
