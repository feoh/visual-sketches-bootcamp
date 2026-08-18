#include "ofApp.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace {
constexpr std::array<float, 6> kRecordedAmplitudes{{0.0f, 0.05f, 0.4f, 1.0f, 0.0f, 0.2f}};
ofColor asColor(embodied::Color color) { return ofColor(color.r, color.g, color.b); }
}

void ofApp::setup() {
    ofSetWindowTitle("Section 15 starter: louder makes a larger sun");
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
    pending_live_value_.store(false);
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
    } else if (state_.source == embodied::InputSource::live_microphone &&
               pending_live_value_.exchange(false)) {
        embodied::consumeAmplitude(state_, pending_amplitude_.load(), design_);
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
    pending_live_value_.store(true);
}

const char* ofApp::sourceLabel() const {
    if (state_.source == embodied::InputSource::recorded) return "RECORDED AMPLITUDE (NO AUDIO)";
    if (state_.source == embodied::InputSource::live_microphone) return "LIVE MICROPHONE";
    return "NO DEVICE: KEYBOARD FALLBACK";
}

void ofApp::draw() {
    ofBackground(asColor(design_.background));
    const float center_x = ofGetWidth() * 0.5f;
    const float center_y = ofGetHeight() * 0.52f;
    const auto& geometry = state_.geometry;
    const ofColor mark = geometry.active ? asColor(design_.active_color)
                                         : asColor(design_.quiet_color);
    ofSetColor(mark);
    ofSetLineWidth(3.0f);
    const int rays = reduced_motion_ ? design_.minimum_rays : geometry.ray_count;
    for (int ray = 0; ray < rays; ++ray) {
        const float angle = TWO_PI * static_cast<float>(ray) / static_cast<float>(rays);
        const float inner = geometry.radius + 10.0f;
        const float outer = inner + 13.0f;
        ofDrawLine(center_x + std::cos(angle) * inner,
                   center_y + std::sin(angle) * inner,
                   center_x + std::cos(angle) * outer,
                   center_y + std::sin(angle) * outer);
    }
    ofNoFill();
    ofDrawCircle(center_x, center_y, geometry.radius);
    ofFill();
    ofDrawRectangle(center_x - geometry.radius,
                    center_y + geometry.radius + 28.0f,
                    geometry.radius * 2.0f * geometry.normalized_level, 10.0f);

    ofSetColor(asColor(design_.quiet_color));
    ofDrawBitmapString("LOUDER  ->  BIGGER CIRCLE + MORE RAYS", 24, 30);
    ofDrawBitmapString(sourceLabel(), 24, 54);
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
