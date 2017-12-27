
//  helper func
void ofApp::addCommand(float &time, shared_ptr<Command> cmd){
    cmds.push_back(cmd);
    time += cmd->durSec;
}

void ofApp::addMoveX(float & time, glm::vec3 &pos, int endPos, int speed){
    addCommand(time, make_shared<MoveCommand>(machine, MoveCommand::AXIS_X, time, pos, endPos, speed));
    pos.x = endPos;
}

void ofApp::addMoveY(float &time, glm::vec3 &pos, int endPos, int speed){
    addCommand(time, make_shared<MoveCommand>(machine, MoveCommand::AXIS_Y, time, pos, endPos, speed));
    pos.y = endPos;
}

void ofApp::addMoveZ(float &time, glm::vec3 &pos, int endPos, int speed){
    addCommand(time, make_shared<MoveCommand>(machine, MoveCommand::AXIS_Z, time, pos, endPos, speed));
    pos.z = endPos;
}

void ofApp::addSuck(float &time, glm::vec3 & pos, bool on){
    addCommand(time, make_shared<SuckCommand>(machine, time, pos, on));
}

void ofApp::addPhoto(float &time, glm::vec3 & pos){
    addCommand(time, make_shared<PhotoCommand>(machine, time, pos));
}


void ofApp::drawInfo(){
    if(!bRunSequence){
        string msg = "After HOMING,\nPress SPACE key to start a sequence";
        ofSetColor(255,0,0);
        ofDrawBitmapString(msg, ofGetWidth()/2-120, ofGetHeight()/2);
    }
    
    {
        string msg;
        msg += "frame      : " + ofToString(currentFrame) + "\n";
        msg += "sec        : " + ofToString(currentFrame/ofGetTargetFrameRate(),1) + "\n";
        msg += "state      : " + machine.stateStr + "\n";
        msg += "pump       : " + ofToString(machine.bSuck ? "ON":"OFF") + "\n";
        msg += "speed      : " + ofToString(machine.targetSpeed)  + "\n";
        msg += "targetPos  : " + ofToString(machine.targetPos)  + "\n";
        msg += "currentPos : " + ofToString(machine.currentPos)  + "\n";
        //msg += "originalPos : " + ofToString(machine.originalPos)  + "\n";
        
        msg += "gcode      : " + machine.latestCmd + "\n";
        //msg += "read  : " + ofToString(machine.bytesReadString) + "\n";
        ofSetColor(255);
        ofDrawBitmapString(msg, 18, 25);
    }
    
    machine.draw(ofGetWidth()-220, 30);
}

static std::string randomIPv6() {
    
    std::string output;
    char arr[16];
    size_t length = sizeof(arr);
    
    for (int i=0; i<length; i+=2) {
        arr[i] = (char)ofRandom(-127, 128);
        arr[i+1] = (char)ofRandom(-127, 128);
        
        output += (ofToHex(arr[i]));
        output += (ofToHex(arr[i+1]));
        
        if (i<(length-2)) {
            output += ":";
        }
    }
    return output;
}
