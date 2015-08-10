//
//  YouTubeSearch.h
//  example
//
//  Created by dantheman on 1/9/15.
//
//

#pragma once
#include "ofMain.h"
struct YouTubeSearch{
    string user;
    string query;
    string renderPath;
    vector<string> files;
    vector<string> downloaded;
    bool bReady;
    ofVec2f drawPos;
    string time;
    bool sendPath;
    float captureDuration;
    float startTime;
    int index;
    bool bPlaying;
    float timeout;
};
