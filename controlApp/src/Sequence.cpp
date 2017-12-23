#include "Sequence.h"
#include "GrainCounterController.h"

Sequence::Sequence(SeqType _type, float timeSec, int _stVal, int _endVal, int _speed){
    set(_type, timeSec, _stVal, _endVal, _speed);
}

void Sequence::set(SeqType _type, float timeSec, int _stVal, int _endVal, int _speed){
    type = _type;
    frame = timeSec * ofGetTargetFrameRate();
    stVal = _stVal;
    endVal = _endVal;
    speed = _speed;
    dist = abs(endVal - stVal);
    durSec = (dist/speed) * 60.0f +  0.1;
    cmd = GrainCounterController::makeGcode(type, endVal, speed);
    
    switch(type){
        case HOME:  durSec=10; break;
        case SUCK:  durSec=0.5; break;
        case PHOTO: durSec=1.0; break;
    }
}

void Sequence::home(float timeSec){
    set(HOME, timeSec, 0, 0, 0);
}

void Sequence::x(float timeSec, glm::vec3 & pos, int x, int speed){
    set(MOVE_X, timeSec, pos.x, x, speed);
    pos.x = x;
}

void Sequence::y(float timeSec, glm::vec3 & pos, int y, int speed){
    set(MOVE_Y, timeSec, pos.y, y, speed);
    pos.y = y;
}

void Sequence::z(float timeSec, glm::vec3 & pos, int z, int speed){
    set(MOVE_Z, timeSec, pos.z, z, speed);
    pos.z = z;
}

void Sequence::suck(float timeSec, bool on){
    if(on) set(SUCK, timeSec, 0, 1, 0);
    else set(SUCK, timeSec, 1, 0, 0);
}

void Sequence::photo(float timeSec){
    set(PHOTO, timeSec, 0,1,0);
}
