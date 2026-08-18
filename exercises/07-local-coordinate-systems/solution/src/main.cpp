#include "ofMain.h"
#include "ofApp.h"
int main() {
    ofGLFWWindowSettings settings;
    settings.setSize(960, 360);
    settings.resizable = true;
    auto window = ofCreateWindow(settings);
    ofRunApp(window, std::make_shared<ofApp>());
    ofRunMainLoop();
}
