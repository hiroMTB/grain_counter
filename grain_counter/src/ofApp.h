#pragma once

#include "ofMain.h"
#include "GrainCounterController.h"
#include "ofxHttpUtils.h"

class Sequence{
    
public:
    Sequence(){};
    Sequence(float _time, string _cmd){
        frame = _time * ofGetTargetFrameRate();
        cmd = _cmd;
    }
    
    int frame = 0;     // frame
    string cmd = "";    // gcode
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void setupSerial();
    void setupCamera();
    void generateSequence();
    void takePhoto(bool upload=true);
    void checkSequence(int frame);
    float calcWait(float posSt, float posEnd, float speed);
    
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

    
    int startFrame = 0;
};

