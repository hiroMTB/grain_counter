#include "Machine.h"
#include "Command.h"

Machine::Machine(){
    path.setMode(OF_PRIMITIVE_LINE_STRIP);
    dot.setMode(OF_PRIMITIVE_POINTS);
};

void Machine::init(string name, int baud){
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    serial.setup(name, baud);
    reset();
}

void Machine::setRange(int xMax, int yMax, int zMax){
    xRange.set(0,xMax);
    yRange.set(0,yMax);
    zRange.set(0,zMax);
}

void Machine::reset(){
    latestCmd = "";
    errorMsg = "";
    state = IDLE;
    targetPos = glm::vec3(0,0,0);
    currentPos = glm::vec3(0,0,0);
    originalPos = glm::vec3(0,0,0);
    targetSpeed = 0;
    bSuck = false;
}

void Machine::execute(string cmd){

    if(serial.isInitialized()){
        serial.writeBytes(cmd.c_str(), cmd.size());
    }
    latestCmd = cmd;
}

void Machine::makeVbo(vector<shared_ptr<Command>> cmds){
    
    path.addVertex(glm::vec3(0,0,0));
    path.addColor(ofFloatColor(0,0,0));
    
    for(auto cmd : cmds){
        
        shared_ptr<MoveCommand> m = dynamic_pointer_cast<MoveCommand>(cmd);
        if(m){
            glm::vec3 p = m->endPos;
            //p.y = -p.y;
            //p.z = 0;
            path.addVertex(p);
            path.addColor(ofFloatColor(0,0,0));
        }else{
            shared_ptr<SuckCommand> m = dynamic_pointer_cast<SuckCommand>(cmd);
            if(m){
                
                glm::vec3 p = m->pos;
                //p.y = -p.y;
                //p.z = 0;
                dot.addVertex(p);
                dot.addColor(ofFloatColor(0,0,1));
            }
        }
    }
}

void Machine::update(){
    // simulate current position
    if(state == MOVE_X || state == MOVE_Y || state == MOVE_Z){
        float dist = glm::distance(currentPos, targetPos);
        glm::vec3 dir = targetPos - originalPos;
        dir = glm::normalize(dir);
        float speedPerFrame = targetSpeed/60.0f/ofGetTargetFrameRate();
        glm::vec3 adder = dir * speedPerFrame;
        
        if(speedPerFrame<dist){
            currentPos += adder;
        }else{
            currentPos = targetPos;
            state = IDLE;
        }
    }
}

void Machine::draw(int x, int y){

    ofSetLineWidth(1);
    glPointSize(3);
    float pad = 4;
    
    // draw XY plane
    ofRectangle vp(x, y, xRange.span()+pad, yRange.span()+pad);
    cam.begin(vp);
    ofNoFill();
    ofSetColor(0);
    ofDrawRectangle(pad/2,pad/2,vp.width-pad, vp.height-pad);
    ofScale(1,-1,1);
    ofTranslate(pad/2, -yRange.span()-pad/2);
    scene();
    if(bSuck){
        ofSetColor(255,0,0);
        ofNoFill();
        ofDrawCircle(currentPos, 5);
    }
    cam.end();
    
    // draw XZ plane
    ofRectangle vp2(x, y+yRange.span()+20, xRange.span()+pad, zRange.span()+pad);
    cam.begin(vp2);
    ofNoFill();
    ofSetColor(0);
    ofDrawRectangle(pad/2,pad/2,vp2.width-pad, vp2.height-pad);
    ofRotateXDeg(90);
    ofTranslate(pad/2, 0, -zRange.span()-pad/2);
    scene();
    cam.end();
}

void Machine::scene(){

    // future path
    path.draw();
    
    // pump on/off point
    dot.draw();
    
    ofSetColor(255,0,0);
    ofDrawLine(currentPos, targetPos);
    
    ofFill();
    ofSetColor(255,0,0);
    ofDrawSphere(targetPos, 2);
    
    ofFill();
    ofSetColor(255,0,0);
    ofDrawSphere(currentPos, 2);
    
}
