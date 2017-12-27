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
    void makeVbo(vector<shared_ptr<Command>> cmds);
    void update();
    void draw(int x, int y);
    void scene();

    enum State{IDLE, HOME, MOVE_X, MOVE_Y, MOVE_Z, SUCK, PHOTO};
    map<State, string> stateStr{
        {IDLE,  "idle"},
        {HOME,  "home"},
        {MOVE_X,"move X"},
        {MOVE_Y,"move Y"},
        {MOVE_Z,"move Z"},
        {SUCK,  "pump"},
        {PHOTO, "photo"}
    };
    
    State state{IDLE};
    ofSerial serial;
    
    glm::vec3 targetPos;
    glm::vec3 currentPos;
    glm::vec3 originalPos;
    
    float targetSpeed;
    bool bSuck{false};
    string latestCmd;
    
    ofRange xRange, yRange, zRange;

    ofVboMesh path;
    ofVboMesh dot;
    
    OrthoCam cam;
    
    string errorMsg{""};
    //vector<shared_ptr<Command>> stack;

};
