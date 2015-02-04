//
//  main.cpp
//  100_HPM
//
//  Created by dantheman on 2/4/15.
//
//
#include "ofApp.h"
#include "ofAppGLFWWindow.h"
#include "ofxWatchdog.h"


int main(int argc, const char** argv)
{
    
    //ofxWatchdog::watch(10000, true, true, true);
    ofAppGLFWWindow win;
    win.setMultiDisplayFullscreen(true); //this makes the fullscreen window span across all your monitors
    ofSetupOpenGL(&win, 1920,1080, OF_FULLSCREEN);
    ofRunApp(new ofApp());
    return 0;
}
