#include "ofMain.h"
#include "ofApp.h"

int main() {
    ofGLFWWindowSettings settings;
    settings.setSize(960, 640);
    settings.setGLVersion(3, 2);
    return ofRunApp(new ofApp);
}
