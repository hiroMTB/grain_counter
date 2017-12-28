#pragma once

#include "ofMain.h"
#include "ofxHttpUtils.h"
#include "Machine.h"
#include "ofxOpenCv.h"

class Command;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void updateCv();
    
    void generateSequence();
    void checkSequence(int frame);
    void testSequence(int totalFrame);
    void exit();

    // ofApp_helper.cpp
    void addCommand(float &time, shared_ptr<Command> m);
    void addHome(float &time);
    void addMoveX(float &time, glm::vec3 &pos, int endPos, int speed);
    void addMoveY(float &time, glm::vec3 &pos, int endPos, int speed);
    void addMoveZ(float &time, glm::vec3 &pos, int endPos, int speed);
    void addSuck (float &time, glm::vec3 &pos, bool on);
    void addPhoto(float &time, glm::vec3 &pos);
    void setupCamera();
    void takePhoto();
    void uploadImage(string s);
    void newResponse(ofxHttpResponse & response);
    void setupCv();

    // ofApp_draw.cpp
    void draw();
    void drawInfo(int x, int y);
    void drawManual(int x, int y);
    void drawCommands(int x, int y);
    void drawAddress(int x, int y);
    void drawCvMonitor(int x, int y);
    void drawCvContour(int x, int y, float scale);
    
    // ofApp_key.cpp
    void keyPressed(int key);

    // camera
    ofVideoGrabber vidGrabber;
    bool bCamInit       = false;
    bool bSaveRequest   = false;
    int camWidth        = 640;
    int camHeight       = 480;
    
    // web
    ofxHttpUtils httpUtils;
    string action_url;
    string address;

    // machine & command sequence
    Machine machine;
    vector<shared_ptr<Command>> cmds;
    
    bool bRunSequence = false;
    int startFrame = 0;
    int currentFrame = 0;
    int currentCmd = 0;
    float totalSec = 0;

    bool bShowManual = true;

    // openCv
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImg;
    ofxCvGrayscaleImage grayThresh;
    ofxCvContourFinder contourFinder;

    vector<ofImage> testImg;
    int currentImg = 0;
    int threshold;
};

