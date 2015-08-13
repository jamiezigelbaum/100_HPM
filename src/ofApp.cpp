#include "ofApp.h"
#include "ofxNodejs.h"

using namespace ofxNodejs;
void ofApp::exit(){
    gui->saveSettings(ofToDataPath("settings/settings.xml"));
}

void ofApp::setup()
{
    
    
    ofSetLogLevel(OF_LOG_SILENT);
    ofxNodejs::init();
    $("var DEBUGGING     = true;"
      "var debug         = require('../../../data/node/libs/debug');"
      "var sha1          = require('../../../data/node/libs/sha1');"
      "var twitter       = require('../../../data/node/libs/api/twitter');"
      "var upload       = require('../../../data/node/libs/api/upload');"
      
      "twitter.start(true);"
      "upload.start(false);"
      );
    
    
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    ofSetLogLevel(OF_LOG_ERROR);
    ofEnableArbTex();
    sender.setup("127.0.0.1", 7779);
    receiver.setup(7778);
    regular.setup(ofToDataPath("font/AvenirNext-Medium.ttf"));
    bold.setup(ofToDataPath("font/AvenirNext-Bold.ttf"));
    texWidth = 640;
    texHeight = 480;
    maxVids = 9;
    scaleTex = 1.0;
    display = "Tweet to @100_HPM with your search query";
    search = "using this hashtag ";
    
    
    gui = new ofxUISuperCanvas("YouTube Search Average");
    gui->addSlider("hue", -1.0, 1.0, &hue);
    gui->addSlider("saturation", 0, 4.0, &saturation);
    gui->addSlider("contrast", -4.0, 4.0, &contrast);
    gui->addSlider("brightness", -1.0, 1.0, &brightness);
    gui->add2DPad("center", ofxUIVec3f(0.0, ofGetWindowWidth(), ofGetWindowWidth()), ofxUIVec3f(0, ofGetWindowHeight(), ofGetWindowHeight()), &center);
    gui->add2DPad("Text Pos", ofxUIVec3f(0.0, ofGetWindowWidth(), ofGetWindowWidth()), ofxUIVec3f(0, ofGetWindowHeight(), ofGetWindowHeight()), &textCenter);
    gui->addIntSlider("SS Width", 0, 2048, &ssWidth);
    gui->addIntSlider("SS Height", 0, 1536, &ssHeight);
    gui->addSlider("Second Screen X", 0, ofGetScreenWidth(), &secondScreenX);
    gui->addSlider("scale", 0.5, 2.0, &scaleTex);
    gui->addIntSlider("numVideos", 2, 10, &maxVids);
    gui->addIntSlider("Recording Dur Scale", 1, 5, &recordingScale);
    //        gui->addTextInput("Display", display);
    gui->addToggle("Debug On Start", &debug);
    gui->loadSettings("settings/settings.xml");
    gui->autoSizeToFitWidgets();
    gui->update();
    
    gui->setVisible(debug);
    if(debug){
        ofShowCursor();
    }else{
        ofHideCursor();
    }
    
    
    
    ofDirectory dir;
    dir.listDir(ofToDataPath("movies/bobross"));
    dir.sort();
    //int j = 0;
    for(int j = 0; j <3; j++){
        stringstream src;
        src<<"/*{\n \"DESCRIPTION\": \"RGB Average\"\n,\"CREDIT\": \"by dantheman\"\n,\"CATEGORIES\": [\"stylized Effect\"]\n,\"INPUTS\": [";
        if(j < 2){
            for(int i = 0; i < maxVids; i++){
                src<<"{\"NAME\": \"image"<<ofToString(i+maxVids*j)<<"\",\n\"TYPE\": \"image\"}";
                if(i < maxVids-1){
                    src<<",\n";
                }
            }
            src<<"]}*/\n\n";
            src << "void main(void) {\n";
            src << "vec2 uv = vv_FragNormCoord.xy;\n";
            src <<"vec4	final = vec4(0.0);\n";
            for(int i = 0; i < maxVids; i++) {
                src <<"vec4	srcPixel"<<ofToString(i+maxVids*j)<<" = IMG_NORM_PIXEL(image"<<ofToString(i+maxVids*j)<<",uv);\n";
                src<<"final+=srcPixel"<<ofToString(i+maxVids*j)<<";\n";
            }
            
            src<<"gl_FragColor = vec4(vec3(final)/"<<maxVids<<".0, 1.0);\n"<<endl;
            src << "}\n";
            
            string shaderFile = ("shaders/avg-"+ofToString(j)+".fs");
            string path = ofToDataPath(shaderFile, true);
            ofstream isfShader;
            isfShader.open(path.c_str());
            isfShader << src.str();
            isfShader.close();
            if( j == 0){
                isf.setup(texWidth, texHeight, GL_RGBA);
                isf.load(shaderFile);
            }else if (j == 1){
                isf2.setup(texWidth, texHeight, GL_RGBA);
                isf2.load(shaderFile);
            }
        }else{
            for(int i = 0; i < 2; i++){
                src<<"{\"NAME\": \"image"<<ofToString(i)<<"\",\n\"TYPE\": \"image\"}";
                if(i < 1){
                    src<<",\n";
                }
            }
            src<<"]}*/\n\n";
            src << "void main(void) {\n";
            src << "vec2 uv = vv_FragNormCoord.xy;\n";
            src <<"vec3	final = vec3(0.0);\n";
            for(int i = 0; i < 2; i++) {
                src <<"vec3	srcPixel"<<ofToString(i)<<" = IMG_NORM_PIXEL(image"<<ofToString(i)<<",uv).xyz;\n";
                src<<"final+=srcPixel"<<ofToString(i)<<";\n";
            }
            src<<"gl_FragColor = vec4((final)*0.5, 1.0);\n"<<endl;
            src << "}\n";
            
            string shaderFile = "shaders/baseAvg.fs";
            string path = ofToDataPath(shaderFile, true);
            ofstream isfShader;
            isfShader.open(path.c_str());
            isfShader << src.str();
            isfShader.close();
            isf4.setup(texWidth, texHeight, GL_RGBA);
            isf4.load(shaderFile);
        }
    }
    isf4.setImage("image0", isf2.getTextureReference());
    isf4.setImage("image1", isf.getTextureReference());
    colorCorrect.setup(texWidth, texHeight, GL_RGBA);
    colorCorrect.load("shaders/Color Controls.fs");
    colorCorrect.setImage(isf4.getTextureReference());
    
    normalized.resize(maxVids*2);
    
    //    if(dir.size() >= maxVids){
    for(int i = 0; i < maxVids*2; i++){
        players.push_back(new ofxAVFVideoPlayer());
        if(i<dir.size()){
            players[i]->loadMovie(dir.getPath(i));
            players[i]->setLoopState(OF_LOOP_NORMAL);
            players[i]->play();
            players[i]->setVolume(0);
            players[i]->setSpeed(1.0);
            currentSearch.files.push_back(dir.getPath(i));
//            players[i]->setUseTexture(true);
        }
        
        
        //
        normalized[i] = new ofFbo();
        normalized[i]->allocate(texWidth, texHeight, GL_RGBA);
        normalized[i]->begin();
        ofClear(0, 0, 0, 0);
        normalized[i]->end();
        
        if(i < maxVids){
            isf.setImage("image"+ofToString(i), normalized[i]->getTextureReference());
        }else if(i >=maxVids){
            isf2.setImage("image"+ofToString(i), normalized[i]->getTextureReference());
        }
    }
    //    }
    
    
    
    
    
    
    //        isf4.getShader(0)->setImage("image2", isf3.getTextureReference());
    
    img.allocate(texWidth, texHeight, OF_IMAGE_COLOR);
    newFiles = 0;
    loopIndex = 0;
    fadeDelay = 1500;
    failDelay = 3000;
    index = players.size()-1;
    activePlayers = index;
    
    
    currentSearch.user = "theDANtheMAN";
    currentSearch.query = "\"Bob Ross\"";
    currentSearch.time = "4:19pm";
    currentSearch.captureDuration = 0;
    currentSearch.sendPath = false;
    currentSearch.bPlaying = true;
    currentSearch.bReady = true;
    youTubeMap[currentSearch.query] = currentSearch;
    nextHashTag = "#osc/newHash";
    currentSearch.startTime = ofGetSystemTime();
    recording = false;
    fade = 0;
}

void ofApp::update()
{
    
    float t = ofGetElapsedTimef();
    
    
    oscWorkHorse();
    
    int foomax = currentSearch.files.size()>players.size()?maxVids:currentSearch.files.size()/2;
    
    for(int i = 0; i < foomax*2; i++){
        players[i]->update();
        if(players[i]->isFrameNew()){
            normalized[i]->begin();
            ofClear(0, 0, 0, 0);
            ofSetColor(255, 255, 255, fade);
            ofPushMatrix();
            float ratio = (texWidth / players[i]->getWidth())<(texHeight/players[i]->getHeight())?(texHeight/players[i]->getHeight()):(texWidth/ players[i]->getWidth());
            players[i]->draw((texWidth-players[i]->getWidth()*ratio)/2, (texHeight-players[i]->getHeight()*ratio)/2, players[i]->getWidth()*ratio, players[i]->getHeight()*ratio);
            ofPopMatrix();
            normalized[i]->end();
        }
        
    }
    if(currentSearch.captureDuration <= 0){
        for(int i = 0; i < players.size(); i++){
            if(currentSearch.captureDuration < players[i]->getDuration()){
                currentSearch.index = i;
            }
            currentSearch.captureDuration = MAX(players[i]->getDuration(), currentSearch.captureDuration)/recordingScale;
        }
    }
    
    
    
    
    isf.update();
    isf2.update();
    isf3.update();
    isf4.update();
    
    colorCorrect.setUniform<float>("bright", brightness);
    colorCorrect.setUniform<float>("contrast", contrast);
    colorCorrect.setUniform<float>("hue", hue);
    colorCorrect.setUniform<float>("saturation", saturation);
    
    colorCorrect.update();
    
    float currentTime = (ofGetSystemTime()-currentSearch.startTime)/1000.0;
    
    if(recording){
        if(!recorder.Stopped() && !recorder.Stopping()){
            if(recorder.TimeToNextFrame() < 1.0){
                recorder.AddFrame(colorCorrect.getTextureReference());
            }
        }
        if(currentTime >=currentSearch.captureDuration){
            if(!recorder.Stopped() && !recorder.Stopping()){
                recorder.Stop();
            }
            if(recorder.MovieSaved()){
                if(!currentSearch.sendPath){
                    ofxOscMessage b;
                    b.setAddress("/upload");
                    b.addStringArg(currentSearch.user);
                    b.addStringArg(currentSearch.query);
                    b.addStringArg(currentSearch.renderPath);
                    sender.sendMessage(b);
                    //recorder = NULL;
                    recording = false;
                    currentSearch.sendPath = true;
                }
                
            }
        }
    }else{
        if(searchQueue.size() > 0 && youTubeMap[searchQueue.front()].bReady && !youTubeMap[searchQueue.front()].bPlaying){
            if((currentTime >= (currentSearch.captureDuration-2)) && !dofade){
                dofade = true;
            }
            if(currentTime > currentSearch.captureDuration && fade <= 0){
                youTubeMap[currentSearch.query].bPlaying = false;
                youTubeMap[searchQueue.front()].bPlaying = true;
                currentSearch = youTubeMap[searchQueue.front()];
                currentSearch.renderPath = currentSearch.user+ofGetTimestampString()+".mov";
                currentSearch.captureDuration = 0;
                currentSearch.index = 0;
                currentSearch.sendPath = false;
                for(int i = 0; i < players.size(); i++){
                    if(players[i]->isPlaying()){
                        players[i]->stop();
                    }
                }
                int foomax = currentSearch.files.size()>players.size()?maxVids:currentSearch.files.size()/2;
                
                for(int i = 0; i < foomax*2; i++){
                    players[i]->loadMovie(ofToDataPath("movies/"+youTubeMap[searchQueue.front()].files[i]));
                    players[i]->setLoopState(OF_LOOP_NORMAL);
                    players[i]->play();
                    players[i]->setVolume(0.0);
                    players[i]->setSpeed(1.0);
                }
                activePlayers = currentSearch.files.size()>players.size()?players.size():youTubeMap[searchQueue.front()].files.size();
                string foo = searchQueue.front();
                loopset.push_back(foo);
                searchQueue.pop_front();
                fade = 0;
                currentSearch.startTime = ofGetSystemTime();
                generateShaders(foomax);
                
                recorder = ofxFLRecorder(ofToDataPath("renders/"+currentSearch.renderPath), texWidth, texHeight, 30, ofxFLRecorder::Normal);
                recording = true;
            }
        }else{
            if(youTubeMap.size() > 0 && loopset.size() > 0 && youTubeMap[loopset[loopIndex]].bReady && !youTubeMap[loopset[loopIndex]].bPlaying){
                if((currentTime >= (currentSearch.captureDuration-2)) && !dofade){
                    dofade = true;
                }
                if(currentTime > currentSearch.captureDuration && fade <= 0){
                    youTubeMap[currentSearch.query].bPlaying = false;
                    youTubeMap[loopset[loopIndex]].bPlaying = true;
                    currentSearch = youTubeMap[loopset[loopIndex]];
                    currentSearch.renderPath = currentSearch.user+ofGetTimestampString()+".mov";
                    currentSearch.captureDuration = 0;
                    currentSearch.index = 0;
                    currentSearch.sendPath = false;
                    
                    for(int i = 0; i < players.size(); i++){
                        if(players[i]->isPlaying()){
                            players[i]->stop();
                        }
                    }
                    
                    int foomax = currentSearch.files.size()>players.size()?maxVids:currentSearch.files.size()/2;
                    
                    for(int i = 0; i < foomax*2; i++){
                        players[i]->loadMovie(ofToDataPath("movies/"+currentSearch.files[i]));
                        players[i]->setLoopState(OF_LOOP_NORMAL);
                        players[i]->play();
                        players[i]->setVolume(0.0);
                        players[i]->setSpeed(1.0);
                    }
                    activePlayers = currentSearch.files.size()>players.size()?players.size():currentSearch.files.size();
                    fade = 0;
                    currentSearch.startTime = ofGetSystemTime();
                    generateShaders(foomax);
                    
                    loopIndex++;
                    if(loopIndex >= loopset.size()){
                        loopIndex = 0;
                    }
                }
            }
        }
    }
    
    
    if(loopset.size() > 10){
        youTubeMap.erase(loopset.front());
        loopset.pop_front();
    }
    if(fade < 255 && !dofade){
        if(!recording && fade > 0){
            
        }
        fade+=10;
    }else if (dofade){
        fade-=10;
        if(fade <= 0){
            fade = 0;
            dofade = false;
        }
    }else if(fade > 255){
        fade = 255;
    }
}

void ofApp::draw()
{
    float currentTime = (ofGetSystemTime()-currentSearch.startTime)/1000.0;
    
    ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofSetColor(255, 255, 255, fade);
    ofPushMatrix();
    float ratio = (ofGetWindowWidth() / texWidth) > (ofGetWindowHeight()/ texHeight)?ofGetWindowHeight()/ texHeight:ofGetWindowWidth() / texWidth;
    colorCorrect.draw(center.x-(texWidth*ratio*scaleTex)/2.0, center.y-(texHeight*ratio*scaleTex)/2.0, texWidth*ratio*scaleTex, texHeight*ratio*scaleTex);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(secondScreenX+textCenter.x-(ssWidth)/2.0, textCenter.y-ssHeight/2.0);
    ofSetColor(255, 0, 255);
    if(debug){
        ofNoFill();
        ofRect(0, 0, ssWidth, ssHeight);
        ofFill();
    }
    
    float x = ssWidth*0.10/2.0;
    float y = 1*ssHeight/20;
    ofSetColor(255, 255, 255);
    int numLines = 0;
    bool wordsWereCropped;
    ofRectangle column;
    
    
    
    // Secondary Screen text
    
    // TITLE
    string foo = "100 Hours per Minute";
    column = bold.drawMultiLineColumn(foo, 80,
                                      x, 50,
                                      ssWidth-ssWidth*0.10,
                                      numLines, false, 6, true, &wordsWereCropped);
    

    // DESCRIPTION
    foo = "This installation is interactive, you can use Twitter to search for anything you like on YouTube. When your search comes up in the queue below, 100 Hours per Minute will play the first 14 results all-at-once. Then it will upload your averaged video back to YouTube (where every averaged video is archived) and reply to your tweet with the link.";
    column = regular.drawMultiLineColumn(foo, 40,
                                         x, column.y+column.height+60,
                                         ssWidth-ssWidth*0.10,
                                         numLines, false, 5, true, &wordsWereCropped);
    
    // HORIZONTAL LINE
    ofLine(column.x, column.y+column.height+85/4, column.x+(ssWidth-column.x*2.0), column.y+column.height+85/4);
    
    // INSTRUCTIONS
    numLines = 0;
    wordsWereCropped = false;
    foo = "INSTRUCTIONS";
    column = bold.drawMultiLineColumn(foo, 40,
                                      x, column.y+column.height+60,
                                      ssWidth-ssWidth*0.10,
                                      numLines, false, 5, true, &wordsWereCropped);
    
    numLines = 0;
    wordsWereCropped = false;
    foo = "Use Twitter on your phone and send a tweet to @100_HPM with your search query and the following, single-use hashtag: ";
    column = regular.drawMultiLineColumn(foo, 40,
                                         x, column.y+column.height+60,
                                         ssWidth-ssWidth*0.10,
                                         numLines, false, 5, true, &wordsWereCropped);
    
    ofSetColor(229, 24, 139);
    numLines = 0;
    wordsWereCropped = false;
    foo = nextHashTag;
    column = bold.drawMultiLineColumn(foo, 40,
                                         x+40, column.y+column.height+60,
                                         ssWidth-ssWidth*0.10,
                                         numLines, false, 5, true, &wordsWereCropped);
    ofSetColor(255, 255, 255);
    
    numLines = 0;
    wordsWereCropped = false;
    foo = "For example, if you wanted to search for \"Bob Ross\" you would tweet: \n\"@100_HPM Bob Ross "+nextHashTag +"\"";
    column = regular.drawMultiLineColumn(foo, 40,
                                      x, column.y+column.height+60,
                                      ssWidth-ssWidth*0.10,
                                      numLines, false, 5, true, &wordsWereCropped);
    // HORIZONTAL LINE
    ofLine(column.x, column.y+column.height+40/4, column.x+(ssWidth-column.x*2.0), column.y+column.height+40/4);
    
    
    // PLAYING
    numLines = 0;
    wordsWereCropped = false;
    foo = "PLAY QUEUE";
    column = bold.drawMultiLineColumn(foo, 40,
                                      x, column.y+column.height+90,
                                      ssWidth-ssWidth*0.10,
                                      numLines, false, 5, true, &wordsWereCropped);
    
    numLines = 0;
    wordsWereCropped = false;
    foo = "NOW PLAYING: " +currentSearch.query+" by @"+currentSearch.user+" at "+currentSearch.time;
    column = regular.drawMultiLineColumn(foo, 30,
                                         x, column.y+column.height+50,
                                         ssWidth-ssWidth*0.10,
                                         numLines, false, 5, true, &wordsWereCropped);
    
    // PROGRESS BAR
    ofEnableBlendMode(OF_BLENDMODE_ADD);

    ofSetColor(100, 100, 100, 100);
    ofRect(column.x, column.y+column.height+25, ssWidth-(column.x*2.0), 10);
    
    ofSetColor(255, 255, 255, 100);
    ofRect(column.x, column.y+column.height+25, ofMap(currentTime, 0, currentSearch.captureDuration, 0, ssWidth-(column.x*2.0), true), 10);
    
    
    // QUEUED SEARCHES
    column = ofRectangle(x, column.y+column.height+15, 10, 50);
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofSetColor(255, 255, 255);
    numLines = 0;
    wordsWereCropped = false;
//    foo = "QUEUED SEARCHES: ";
//    column = bold.drawMultiLineColumn(foo, 40,
//                                      x, column.y+column.height+50,
//                                      ssWidth-ssWidth*0.10,
//                                      numLines, false, 5, true, &wordsWereCropped);
    
    for(int i = 0; i < searchQueue.size(); i++){
        ofSetColor(255, 255, 255, ofMap(youTubeMap[searchQueue[i]].downloaded.size(), 0, youTubeMap[searchQueue[i]].files.size(), 100, 255, true));
        foo = "QUEUED: " +youTubeMap[searchQueue[i]].query+" by @"+youTubeMap[searchQueue[i]].user+" -- "+ofToString(ofMap(youTubeMap[searchQueue[i]].downloaded.size(), 0, youTubeMap[searchQueue[i]].files.size(), 0, 100, true))+"% Downloaded";
        column = regular.drawMultiLineColumn(foo, 30,
                                             x, column.y+column.height+50,
                                             ssWidth-ssWidth*0.10,
                                             numLines, false, 5, true, &wordsWereCropped);
    }
    ofPopMatrix();
    ofDisableAlphaBlending();
    
    
    
    if(debug){
        ofDrawBitmapString(ofToString(ofGetFrameRate()), ofGetWidth()-300, 50);
        ofDrawBitmapString(ofToString(currentTime)+" start fade "+ofToString(currentSearch.captureDuration-255/60), ofGetWidth()-300, 100);
        ofDrawBitmapString(" duration: "+ofToString(currentSearch.captureDuration), ofGetWidth()-300, 150);
        ofDrawBitmapString("fade: "+ofToString(fade)+" record: "+ofToString(recording), ofGetWidth()-300, 200);
        ofShowCursor();
    }
    
}

void ofApp::keyPressed(int key)
{
    if(key == 's'){
        gui->saveSettings(ofToDataPath("settings.xml"));
    }
    if(key == 'd'){
        debug = !debug;
        gui->setVisible(debug);
        if(debug){
            ofShowCursor();
        }else{
            ofHideCursor();
        }
        
    }
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
}
void ofApp::generateShaders(int numVids){
    for(int j = 0; j < 2; j++){
        stringstream src;
        src<<"/*{\n \"DESCRIPTION\": \"RGB Average\"\n,\"CREDIT\": \"by dantheman\"\n,\"CATEGORIES\": [\"stylized Effect\"]\n,\"INPUTS\": [";
        for(int i = 0; i < numVids; i++){
            src<<"{\"NAME\": \"image"<<ofToString(i+numVids*j)<<"\",\n\"TYPE\": \"image\"}";
            if(i < numVids-1){
                src<<",\n";
            }
        }
        src<<"]}*/\n\n";
        src << "void main(void) {\n";
        src << "vec2 uv = vv_FragNormCoord.xy;\n";
        src <<"vec4	final = vec4(0.0);\n";
        for(int i = 0; i < numVids; i++) {
            src <<"vec4	srcPixel"<<ofToString(i+numVids*j)<<" = IMG_NORM_PIXEL(image"<<ofToString(i+numVids*j)<<",uv);\n";
            src<<"final+=srcPixel"<<ofToString(i+numVids*j)<<";\n";
        }
        
        src<<"gl_FragColor = vec4(vec3(final)/"<<numVids<<".0, 1.0);\n"<<endl;
        src << "}\n";
        
        string shaderFile = ("shaders/avg-"+ofToString(j)+".fs");
        string path = ofToDataPath(shaderFile, true);
        ofstream isfShader;
        isfShader.open(path.c_str());
        isfShader << src.str();
        isfShader.close();
        if( j == 0){
            isf.setup(texWidth, texHeight, GL_RGBA);
            isf.load(shaderFile);
        }else if (j == 1){
            isf2.setup(texWidth, texHeight, GL_RGBA);
            isf2.load(shaderFile);
        }
    }
    
    normalized.resize(numVids*2);
    
    for(int i = 0; i < numVids*2; i++){
        normalized[i] = new ofFbo();
        normalized[i]->allocate(texWidth, texHeight, GL_RGBA);
        normalized[i]->begin();
        ofClear(0, 0, 0, 0);
        normalized[i]->end();
        if(i < maxVids){
            isf.setImage("image"+ofToString(i), normalized[i]->getTextureReference());
        }else if(i >=maxVids){
            isf2.setImage("image"+ofToString(i), normalized[i]->getTextureReference());
        }
    }
    
    string shaderFile = "shaders/baseAvg.fs";
    string path = ofToDataPath(shaderFile, true);
    isf4.setup(texWidth, texHeight, GL_RGBA);
    isf4.load(shaderFile);
    
    isf4.setImage("image0", isf2.getTextureReference());
    isf4.setImage("image1", isf.getTextureReference());
    
    colorCorrect.setup(texWidth, texHeight, GL_RGBA);
    colorCorrect.load("shaders/Color Controls.fs");
    colorCorrect.setImage(isf4.getTextureReference());
}
void ofApp::oscWorkHorse(){
    while(receiver.hasWaitingMessages()){
        ofxOscMessage b;
        receiver.getNextMessage(&b);
        if(b.getAddress() == "/newvideo"){
            ofxJSONElement json;
            if(json.parse(b.getArgAsString(0))){
                YouTubeSearch & newSearch = youTubeMap[json["search"].asString()];
                newSearch.downloaded.push_back(json["video"].asString());
                if(newSearch.downloaded.size() == players.size()){
                    newSearch.bReady = true;
                }
            }
        }
        if(b.getAddress() == "/error"){
            
            ofxJSONElement json;
            if(json.parse(b.getArgAsString(0))){
                ofLog(OF_LOG_ERROR)<<"download error"<<" "<<json["search"].asString()<<" "<<json["video"].asString()<<endl;
                YouTubeSearch & newSearch = youTubeMap[json["search"].asString()];
                vector<string>::iterator removeIndex = find(newSearch.files.begin(), newSearch.files.end(), json["video"].asString());
                if(removeIndex != newSearch.files.end()){
                    newSearch.files.erase(removeIndex);
                }
            }
        }
        if(b.getAddress() == "/display"){
            display = b.getArgAsString(0);
        }
        if(b.getAddress() == "/newHash"){
            nextHashTag = b.getArgAsString(0);
        }
        if(b.getAddress() == "/log"){
            string fooLog = b.getArgAsString(0);
            ofLog()<<fooLog<<endl;
        }
        if(b.getAddress() == "/data"){
            ofxJSONElement json;
            if(json.parse(b.getArgAsString(0))){
                YouTubeSearch newSearch;
                newSearch.query = json["search"].asString();
                newSearch.user = json["username"].asString();
                newSearch.bReady = false;
                newSearch.bPlaying = false;
                newSearch.captureDuration = 0;
                string mins = ofGetMinutes()<10?"0"+ofToString(ofGetMinutes()):ofToString(ofGetMinutes());
                string hours = ofGetHours()>12?ofToString(ofGetHours()-12):ofToString(ofGetHours());
                string ending = ofGetHours()>12?"pm":"am";
                newSearch.time = hours+":"+mins+" "+ending;
                for(int i = 0; i < json["videos"].size(); i++){
                    newSearch.files.push_back(json["videos"][i]["id"].asString()+".mp4");
                }
                
                if(newSearch.files.size() > 5){
                    youTubeMap[newSearch.query] = newSearch;
                    searchQueue.push_back(newSearch.query);
                }
                json.save(ofToDataPath("json/"+newSearch.user+ofGetTimestampString()+".json"));
            }
        }
    }
}
