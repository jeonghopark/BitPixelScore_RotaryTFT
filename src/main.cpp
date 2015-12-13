#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    
    ofGLFWWindowSettings settings;
    settings.width = 1280;
    settings.height = 720;

    ofCreateWindow(settings);
    
	ofRunApp( new ofApp());
    
}
