#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    
    ofGLFWWindowSettings settings;
    settings.windowMode = OF_WINDOW;
    ofVec2f _v = ofVec2f(1280, 0);
    settings.setPosition(_v);
    settings.width = 1024;
    settings.height = 768;
    
    ofCreateWindow(settings);
    
	ofRunApp( new ofApp());
    
}
