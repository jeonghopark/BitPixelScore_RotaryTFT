#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    
    ofGLFWWindowSettings settings;
    settings.windowMode = OF_FULLSCREEN;
    ofVec2f _v = ofVec2f(1280, 0);
    settings.setPosition(_v);
    
    ofCreateWindow(settings);
    
	ofRunApp( new ofApp());
    
}
