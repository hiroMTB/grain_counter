#pragma once

#include "ofMain.h"

using namespace std;

class PPMachine;

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

    HomeCommand(PPMachine & _m, float _timeSec)
    :Command(timeSec = _timeSec), m(_m){
        durSec = 10;
        cmd = "G28\n";
        return true;
    }
    
    PPMachine & m;
    bool call() const override{
        m.execute(cmd);
        m.targetPos = glm::vec3(0,0,0);
        m.currentPos = glm::vec3(0,0,0);
        m.originalPos = glm::vec3(0,0,0);
        m.targetSpeed = 0;
        m.state = PPMachine::State::HOME;
        return true;
    }
};

struct SuckCommand : Command{
    SuckCommand(PPMachine & _m, float _timeSec, glm::vec3 _pos, bool _on)
    :Command(_timeSec),m(_m),pos(_pos),on(_on){
        durSec = 0.5;
        cmd = on ? "M104 S190 T0\n" : "M104 S0 T0\n";
    }
    
    PPMachine & m;
    glm::vec3 pos;
    bool on;
    bool call() const override{
        if(m.bSuck != on){
            m.execute(cmd);
            m.bSuck = on;
            m.state = PPMachine::State::SUCK;
            return true;
        }
        return false;
    }
};

struct PhotoCommand : Command{

    PhotoCommand(PPMachine & _m, float _timeSec, glm::vec3 _pos)
    :Command(_timeSec), m(_m), pos(_pos){
        durSec = 1;
        cmd = "take_photo\n";
    }
    PPMachine & m;
    glm::vec3 pos;
    bool call() const override{
        // m.takephoto();
        m.state = PPMachine::State::PHOTO;
        return true;
    }
};

struct MoveCommand : Command{

    enum AxisType{AXIS_X, AXIS_Y, AXIS_Z};
    
    MoveCommand(PPMachine & _m, AxisType _type, float _timeSec, glm::vec3 _stPos, int _endVal, int _speed)
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

    PPMachine & m;
    AxisType type;
    glm::vec3 stPos;
    glm::vec3 endPos;

    int endVal;
    int speed;
    float dist;
    
    bool call() const override {

        m.execute(cmd);
        m.targetSpeed = speed;
        
        m.originalPos = stPos;
        m.targetPos = stPos;

        switch(type){
            case AXIS_X:
                m.targetPos.x = endVal;
                m.state = PPMachine::State::MOVE_X;
                break;
            case AXIS_Y:
                m.targetPos.y = endVal;
                m.state = PPMachine::State::MOVE_Y;
                break;
                
            case AXIS_Z:
                m.targetPos.z = endVal;
                m.state = PPMachine::State::MOVE_Z;
                break;
        }
        
        return true;
    }
};
