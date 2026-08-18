#include "ofApp.h"
#include "ofMain.h"

int main() {
    ofGLWindowSettings settings;
    settings.setSize(800, 600);
    settings.windowMode = OF_WINDOW;
    auto window = ofCreateWindow(settings);
    ofRunApp(window, std::make_shared<ofApp>());
    return ofRunMainLoop();
}
