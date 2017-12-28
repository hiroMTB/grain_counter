void ofApp::draw(){
    
    ofBackground(0);
    ofSetColor(255);
    ofPushMatrix(); {
        
        ofRectangle r(0, 0, camWidth*2, camHeight*2);
        r.scaleTo(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
        vidGrabber.draw(r);
        
        drawAddress(r.x+22, r.y+r.height-30);
        
        takePhoto();
        
        drawInfo(r.x+22, r.y+20);
        drawManual(r.x+r.width-300, r.y+r.height-120);
        drawCommands(r.x+22, r.y+180);
        machine.draw(r.x+r.width-220, r.y+20);
        
    } ofPopMatrix();
}

void ofApp::drawManual(int x, int y){
    
    if(bShowManual){
        string msg;
        msg += "SPACE  key  : start/stop sequence\n";
        msg += "H      key  : homing\n";
        msg += "Arrow  key  : X, Y move\n";
        msg += "[ ,]   key  : Z move\n";
        msg += "Return key  : Pump on/off\n";
        msg += "S      key  : take photo\n";
        msg += "N      key  : execute next command\n";
        msg += "F      key  : fullscreen on/off\n";
        msg += "M      key  : show/hide this manual\n";
        
        ofSetColor(200);
        ofDrawBitmapString(msg, x, y);
    }
}

void ofApp::drawInfo(int x, int y){
    
    string msg;
    msg += "MODE       : " + ofToString(bRunSequence?"AUTO":"MANUAL") + "\n";
    msg += "frame      : " + ofToString(currentFrame) + "\n";
    msg += "sec        : " + ofToString(currentFrame/ofGetTargetFrameRate(),1) + "\n";
    msg += "totalTime  : " + ofToString(totalSec/3600,1) + " h\n";
    msg += "state      : " + machine.stateStr[machine.state] + "\n";
    msg += "error      : " + machine.errorMsg + "\n";
    msg += "pump       : " + ofToString(machine.bSuck ? "ON":"OFF") + "\n";
    msg += "speed      : " + ofToString(machine.targetSpeed)  + "\n";
    msg += "targetPos  : " + ofToString(machine.targetPos,0)  + "\n";
    msg += "currentPos : " + ofToString(machine.currentPos,0)  + "\n";
    msg += "gcode      : " + machine.latestCmd + "\n";
    //msg += "read  : " + ofToString(machine.bytesReadString) + "\n";
    ofSetColor(200);
    ofDrawBitmapString(msg, x, y);
}

void ofApp::drawCommands(int x, int y){
    
    string msg;
    char m[255];
    int i= 0;
    int num = 40;
    for(int i=0; i<num; i++){
        int id = currentCmd + i;
        if(id>=cmds.size()-1){
            msg += "\nEND";
            break;
        }
        shared_ptr<Command> c = cmds[id];
        sprintf(m, "%6.1f sec : %s",c->timeSec, c->cmd.c_str());
        msg += string(m);
        if(i==num-1){
            msg += "             ...";
            break;
        }
    }
    ofSetColor(200);
    ofDrawBitmapString(msg, x, y);
}

void ofApp::drawAddress(int x, int y){
    string msg1 = "position : " + ofToString(machine.targetPos) + "\n";
    string msg2 = "IPv6     : " + address;
    ofSetColor(255);
    ofDrawBitmapString(msg1, x, y);
    ofDrawBitmapString(msg2, x, y+20);
}
