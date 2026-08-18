#include "ofApp.h"
#include "ofMain.h"

int main() {
    ofGLFWWindowSettings settings;
    settings.setSize(800, 600);
    settings.resizable = true;
    settings.windowMode = OF_WINDOW;
    auto window = ofCreateWindow(settings);
    ofRunApp(window, std::make_shared<ofApp>());
    ofRunMainLoop();
}
