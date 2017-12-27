#pragma once

#include "ofMain.h"

using namespace std;

class Machine;

struct Command{
    Command(float _timeSec){
        timeSec = _timeSec;
        frame = timeSec * ofGetTargetFrameRate();
    }
    
    virtual ~Command() = default;
    virtual bool call() const = 0;
    //virtual undo() const = 0;

    int frame;
    float timeSec;
    float durSec;
    string cmd;

};

struct HomeCommand : Command{

    HomeCommand(Machine & _m, float _timeSec)
    :Command(timeSec = _timeSec), m(_m){
        durSec = 10;
        cmd = "G28\n";
        return true;
    }
    
    Machine & m;
    bool call() const override{
        m.execute(cmd);
        m.targetPos = glm::vec3(0,0,0);
        m.currentPos = glm::vec3(0,0,0);
        m.originalPos = glm::vec3(0,0,0);
        m.targetSpeed = 0;
        m.state = Machine::State::HOME;
        m.errorMsg = "";
        return true;
    }
};

struct SuckCommand : Command{
    SuckCommand(Machine & _m, float _timeSec, glm::vec3 _pos, bool _on)
    :Command(_timeSec),m(_m),pos(_pos),on(_on){
        durSec = 0.5;
        cmd = on ? "M104 S190 T0\n" : "M104 S0 T0\n";
    }
    
    Machine & m;
    glm::vec3 pos;
    bool on;
    bool call() const override{
        if(m.bSuck != on){
            m.execute(cmd);
            m.bSuck = on;
            m.state = Machine::State::SUCK;
            m.errorMsg = "";
            return true;
        }
        return false;
    }
};

struct PhotoCommand : Command{

    PhotoCommand(Machine & _m, float _timeSec, glm::vec3 _pos)
    :Command(_timeSec), m(_m), pos(_pos){
        durSec = 1;
        cmd = "take_photo\n";
    }
    Machine & m;
    glm::vec3 pos;
    bool call() const override{
        // m.takephoto();
        m.state = Machine::State::PHOTO;
        m.errorMsg = "";
        return true;
    }
};

struct MoveCommand : Command{

    enum AxisType{AXIS_X, AXIS_Y, AXIS_Z};
    
    MoveCommand(Machine & _m, AxisType _type, float _timeSec, glm::vec3 _stPos, int _endVal, int _speed)
    :Command(_timeSec), m(_m), type(_type), stPos(_stPos), endVal(_endVal), speed(_speed)
    {
        // make gcode, calc dist, speed
        endPos = stPos;
        
        char c[255];
        switch(type){
            case AXIS_X:
                sprintf(c, "G0 X%d F%d\n", endVal, speed);
                dist = abs(endVal - stPos.x);
                endPos.x = endVal;
                break;
                
            case AXIS_Y:
                sprintf(c, "G0 Y%d F%d\n", endVal, speed);
                dist = abs(endVal - stPos.y);
                endPos.y = endVal;
                break;
            case AXIS_Z:
                sprintf(c, "G0 Z%d F%d\n", endVal, speed);
                dist = abs(endVal - stPos.z);
                endPos.z = endVal;
                break;
        }
        cmd = string(c);
        durSec = (dist/speed) * 60.0f +  0.1;
    }

    Machine & m;
    AxisType type;
    glm::vec3 stPos;
    glm::vec3 endPos;

    int endVal;
    int speed;
    float dist;
    
    bool call() const override {

        if(!checkRange()){
            m.errorMsg = "target position is out of range";
            ofLogError("MoveCommand", m.errorMsg);
            return false;
        }
        
        if(m.state == Machine::State::MOVE_X
           || m.state == Machine::State::MOVE_Y
           || m.state == Machine::State::MOVE_Z){
            m.errorMsg = "machine is moving, command ignored";
            ofLogNotice("MoveCommand", m.errorMsg);
            return false;
        }
        
        m.execute(cmd);
        m.targetSpeed = speed;
        
        m.originalPos = stPos;
        m.currentPos = stPos;
        m.targetPos = endPos;
        switch(type){
            case AXIS_X: m.state = Machine::State::MOVE_X; break;
            case AXIS_Y: m.state = Machine::State::MOVE_Y; break;
            case AXIS_Z: m.state = Machine::State::MOVE_Z; break;
        }
        
        m.errorMsg = "";
        return true;
    }
    
    bool checkRange() const {
        bool insidex = m.xRange.contains(endPos.x);
        bool insidey = m.yRange.contains(endPos.y);
        bool insidez = m.zRange.contains(endPos.z);
        return insidex && insidey && insidez;
    }
};