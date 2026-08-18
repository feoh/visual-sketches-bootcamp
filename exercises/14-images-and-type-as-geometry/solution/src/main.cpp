#include "ofApp.h"
#include "ofMain.h"

int main() {
    ofGLFWWindowSettings settings;
    settings.setSize(960, 640);
    settings.setGLVersion(3, 2);
    settings.windowMode = OF_WINDOW;
    auto window = ofCreateWindow(settings);
    ofRunApp(window, std::make_shared<ofApp>());
    ofRunMainLoop();
}
