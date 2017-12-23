#pragma once

#include "ofMain.h"
#include "GrainCounterController.h"
#include "ofxHttpUtils.h"
#include "Sequence.h"

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

    // camera
    ofVideoGrabber vidGrabber;
    bool bCamInit       = false;
    bool bSaveRequest   = false;
    int camWidth        = 320 * 2;
    int camHeight       = 240 * 2;

    GrainCounterController gcc;
    
    // web
    ofxHttpUtils httpUtils;
    string action_url;
    string address;

    vector<Sequence> seq;
    int startFrame = 0;
    int currentFrame = 0;
    int currentSeq = 0;
    string latestCommand;
    bool bRunSequence = false;
};

