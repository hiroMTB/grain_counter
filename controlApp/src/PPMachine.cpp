#include "PPMachine.h"
#include "Command.h"

PPMachine::PPMachine(){
    path.setMode(OF_PRIMITIVE_LINE_STRIP);
    dot.setMode(OF_PRIMITIVE_POINTS);
};

void PPMachine::init(string name, int baud){
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    
    serial.setup(name, baud);
}

void PPMachine::setRange(int xMax, int yMax, int zMax){
    xRange.set(0,xMax);
    yRange.set(0,yMax);
    zRange.set(0,zMax);
}

void PPMachine::execute(string cmd){
    if(serial.isInitialized()){
        serial.writeBytes(cmd.c_str(), cmd.size());
    }
    latestCmd = cmd;
}

void PPMachine::makePath(vector<shared_ptr<Command>> cmds){
    
    path.addVertex(glm::vec3(0,0,0));
    path.addColor(ofFloatColor(0,0,0));
    
    for(auto cmd : cmds){
        
        shared_ptr<MoveCommand> m = dynamic_pointer_cast<MoveCommand>(cmd);
        if(m){
            glm::vec3 p = m->endPos;
            p.y = -p.y;
            p.z = 0;
            path.addVertex(p);
            path.addColor(ofFloatColor(0,0,0));
        }else{
            shared_ptr<SuckCommand> m = dynamic_pointer_cast<SuckCommand>(cmd);
            if(m){
                
                glm::vec3 p = m->pos;
                p.y = -p.y;
                p.z = 0;
                dot.addVertex(p);
                dot.addColor(ofFloatColor(0,0,1));
            }
        }
    }
}

void PPMachine::update(){
    // simulate current position
    if(state == MOVE_X || state == MOVE_Y || state == MOVE_Z){
        float dist = glm::distance(currentPos, targetPos);
        glm::vec3 dir = targetPos - originalPos;
        dir = glm::normalize(dir);
        float speedPerFrame = targetSpeed/60.0f/ofGetTargetFrameRate();
        glm::vec3 adder = dir * speedPerFrame;
        
        if(speedPerFrame<=dist){
            currentPos += adder;
        }else{
            currentPos = targetPos;
            state == IDLE;
        }
    }
}

void PPMachine::draw(){
    
    
    ofSetLineWidth(1);
    
    // XY view
    ofSetColor(0);
    ofNoFill();
    ofRectangle rect(glm::vec2(xRange.min, yRange.min), glm::vec2(xRange.max, yRange.max));
    ofDrawRectangle(rect);
    
    ofPushMatrix(); {
        ofTranslate(0, yRange.span());
        
        // future path
        path.draw();
        
        // suck point
        glPointSize(3);
        dot.draw();
        
        ofSetColor(255,0,0);
        ofDrawLine(originalPos.x, -originalPos.y, targetPos.x, -targetPos.y);
        
        ofNoFill();
        ofSetColor(255,0,0);
        ofDrawCircle(targetPos.x, -targetPos.y, 5);
        
        ofFill();
        ofSetColor(255,0,0);
        ofDrawCircle(currentPos.x, -currentPos.y, 3);
    } ofPopMatrix();
    
    
    // XZ view
    ofPushMatrix();{
        ofTranslate(0, yRange.span()+10);
        
        ofNoFill();
        ofSetColor(0);
        ofRectangle rectZ(glm::vec2(xRange.min, zRange.min), glm::vec2(xRange.max, zRange.max));
        ofDrawRectangle(rectZ);

        ofTranslate(0, zRange.span());

        ofSetColor(255,0,0);
        ofDrawLine(originalPos.x, -originalPos.z, targetPos.x, -targetPos.z);
        
        ofNoFill();
        ofSetColor(255,0,0);
        ofDrawCircle(targetPos.x, -targetPos.z, 5);
        
        ofFill();
        ofSetColor(255,0,0);
        ofDrawCircle(currentPos.x, -currentPos.z, 3);
        
    }ofPopMatrix();
}
