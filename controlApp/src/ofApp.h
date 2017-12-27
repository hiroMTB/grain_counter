#pragma once

#include "ofMain.h"
#include "ofxHttpUtils.h"
#include "Machine.h"

class Command;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void exit();

    void setupSerial();
    void setupCamera();
    void generateSequence();
    void drawInfo();
    void takePhoto(bool upload=true);
    void checkSequence(int frame);
    void newResponse(ofxHttpResponse & response);
    void uploadImage(string s);

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

    Machine machine;
    vector<shared_ptr<Command>> cmds;
};

