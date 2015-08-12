#pragma once

#include "ofMain.h"
#include "ofxISF.h"
#include "ofxOsc.h"
#include "ofxUI.h"
#include "YouTubeSearch.h"
#include "ofxJSON.h"
#include "ofxFontStash.h"
#include "ofxFLrecorder.h"
#include "ofxAVFVideoPlayer.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    void generateShaders(int foomax);
    void oscWorkHorse();
    void generateShaders();
    void keyPressed(int key);
    
    ofxISF::Shader isf;
    ofxISF::Shader isf2;
    ofxISF::Shader isf3;
    ofxISF::Shader isf4;
    ofxISF::Shader colorCorrect;
    vector<ofxAVFVideoPlayer*> 	players;
    vector<ofFbo*> 	normalized;
    vector<string> filePaths;
    ofVideoGrabber video;
    ofImage img;
    float scale;
    unsigned long long lastInteraction, searchStarted;
    unsigned long long fadeDelay, failDelay;
    string display;
    string search;
    string nextHashTag;
    int newFiles;
    int index;
    int fade;
    float activePlayers;
    ofxOscSender sender;
    ofxOscReceiver receiver;
    ofxUISuperCanvas* gui;
    float brightness, contrast, hue, saturation;
    map<string, YouTubeSearch> youTubeMap;
    deque<string> searchQueue;
    deque<string> loopset;
    YouTubeSearch currentSearch;
    int maxVids;
    float min, mid, max, rate;
    bool dofade;
    bool recording;
    ofxFontStash regular;
    ofxFontStash bold;
    ofxFLRecorder recorder;
    bool debug;
    ofxUIVec3f center;
    int texWidth, texHeight;
    float scaleTex;
    ofxUIVec3f textCenter;
    int ssHeight, ssWidth;
    float secondScreenX;
    int loopIndex;
    int recordingScale;
    
};
