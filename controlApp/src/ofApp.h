#pragma once

#include "ofMain.h"
#include "ofxHttpUtils.h"
#include "PPMachine.h"

class Command;

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

    // helper func
    void addCommand(float &time, shared_ptr<Command> m);
    void addMoveX(float &time, glm::vec3 &pos, int endPos, int speed);
    void addMoveY(float &time, glm::vec3 &pos, int endPos, int speed);
    void addMoveZ(float &time, glm::vec3 &pos, int endPos, int speed);
    void addSuck (float &time, glm::vec3 &pos, bool on);
    void addPhoto(float &time, glm::vec3 &pos);
    
    // camera
    ofVideoGrabber vidGrabber;
    bool bCamInit       = false;
    bool bSaveRequest   = false;
    int camWidth        = 320 * 2;
    int camHeight       = 240 * 2;
    
    // web
    ofxHttpUtils httpUtils;
    string action_url;
    string address;

    int startFrame = 0;
    int currentFrame = 0;
    int currentCmd = 0;
    bool bRunSequence = false;

    PPMachine machine;
    vector<shared_ptr<Command>> cmds;
};

