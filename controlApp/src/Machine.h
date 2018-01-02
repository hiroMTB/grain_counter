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

    enum class State{IDLE, HOME, MOVE_X, MOVE_Y, MOVE_Z, SUCK, PHOTO, ERROR};

    map<State, string> stateStr{
        {State::IDLE,  "Idle"},
        {State::HOME,  "Homing"},
        {State::MOVE_X,"Move X"},
        {State::MOVE_Y,"Move Y"},
        {State::MOVE_Z,"Move Z"},
        {State::SUCK,  "Pump"},
        {State::PHOTO, "Photo"},
        {State::ERROR, "Error"}
    };
    
    State state{State::IDLE};
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
