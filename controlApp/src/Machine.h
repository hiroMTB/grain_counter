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
    void reset();
    void execute(string cmd);
    void makeVbo(vector<shared_ptr<Command>> cmds);
    void update();
    void draw(int x, int y);
    void scene();

    enum State{IDLE, HOME, MOVE_X, MOVE_Y, MOVE_Z, SUCK, PHOTO, ERROR};
    map<State, string> stateStr{
        {IDLE,  "Idle"},
        {HOME,  "Homing"},
        {MOVE_X,"Move X"},
        {MOVE_Y,"Move Y"},
        {MOVE_Z,"Move Z"},
        {SUCK,  "Pump"},
        {PHOTO, "Photo"},
        {ERROR, "Error"}
    };
    
    State state{IDLE};
    ofSerial serial;
    glm::vec3 targetPos;
    glm::vec3 currentPos;
    glm::vec3 originalPos;
    
    float targetSpeed;
    bool bSuck;
    string latestCmd;
    string errorMsg;
    
    ofRange xRange, yRange, zRange;

    ofVboMesh path;
    ofVboMesh dot;
    
    OrthoCam cam;

};
