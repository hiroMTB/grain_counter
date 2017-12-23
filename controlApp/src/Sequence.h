#pragma once
#include "ofMain.h"

enum SeqType{
    HOME    = 0,
    MOVE_X  = 1,
    MOVE_Y  = 2,
    MOVE_Z  = 3,
    SUCK    = 4,
    PHOTO   = 5
};

class Sequence{
public:
    Sequence(){}
    Sequence(SeqType _type, float timeSec, int _stVal, int _endVal, int _speed);
    
    void set(SeqType _type, float timeSec, int _stVal, int _endVal, int _speed);

    void home(float timeSec);
    void x(float timeSec, glm::vec3 & pos, int x, int speed);
    void y(float timeSec, glm::vec3 & pos, int y, int speed);
    void z(float timeSec, glm::vec3 & pos, int z, int speed);
    void suck(float timeSec, bool on);
    void photo(float timeSec);
    
    SeqType type;
    int frame;
    int stVal;
    int endVal;
    int speed;
    float dist;
    float durSec;
    string cmd;
};
