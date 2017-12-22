#pragma once

#include "ofMain.h"
#include "GrainCounterController.h"
#include "ofxHttpUtils.h"

class Sequence{
    
public:
    Sequence(){};
    Sequence(int t, string s)
    :time(t), cmd(s){}
    
    float time = 0;     // sec
    string cmd = "";    // gcode
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void setupSerial();
    void setupCamera();
    void generateSequence();
    
    void update();
    void draw();
    void keyPressed(int key);
    void newResponse(ofxHttpResponse & response);
    void uploadImage(string s);
    void exit();
    
    string command;
    
    ofVideoGrabber vidGrabber;
    ofTexture videoTexture;
    int camWidth;
    int camHeight;

    GrainCounterController gcc;
    
    ofxHttpUtils httpUtils;
    string action_url;
    string address;
    bool bSaveRequest = false;

    
    vector<Sequence> seq;
    
};

