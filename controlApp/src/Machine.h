#pragma once 

#include "ofMain.h"
#include "ofRange.h"
#include "OrthoCam.h"

class Command;

class Machine{
    
public:
    Machine();
    
    void init(string name="/dev/cu.usbmodem1421", int baud=115200);
    void setRange(int xMax, int yMax, int zMax);
    void execute(string cmd);
    void makePath(vector<shared_ptr<Command>> cmds);
    void update();
    void draw(int x, int y);
    void scene();

    enum State{IDLE, HOME, MOVE_X, MOVE_Y, MOVE_Z, SUCK, PHOTO};

    State state{IDLE};
    ofSerial serial;
    
    glm::vec3 targetPos;
    glm::vec3 currentPos;
    glm::vec3 originalPos;
    
    float targetSpeed;
    bool bSuck{false};

    ofRange xRange, yRange, zRange;

    string stateStr;
    string latestCmd;
    
    bool bRunSequence = false;
    
    ofVboMesh path;
    ofVboMesh dot;
    
    OrthoCam cam;
};
