void ofApp::draw(){
    
    ofBackground(0);
    ofSetColor(255);
    
    int w = grayImg.getWidth();
    int h = grayImg.getHeight();
    
    ofRectangle r(0, 0, w, h);
    r.scaleTo(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
    grayImg.draw(r);
    float scale = MIN(r.width/w, r.height/h);
        
    drawCvContour(r.x, r.y, scale);
    drawAddress(r.x+22, r.y+r.height-30);
    
    if(bSaveRequest) takePhoto();
    
    drawInfo(r.x+22, r.y+20);
    drawManual(r.x+r.width-280, r.y+r.height-130);
    drawCommands(r.x+22, r.y+200);
    machine.draw(r.x+r.width-220, r.y+20);
    drawCvMonitor(r.x+r.width-220, r.y+370);
}

void ofApp::drawManual(int x, int y){
    
    if(bShowManual){
        string msg;
        msg += "SPACE  : start/stop sequence\n";
        msg += "H      : homing\n";
        msg += "Arrow  : X, Y move\n";
        msg += "[ ,]   : Z move\n";
        msg += "Return : Pump on/off\n";
        msg += "S      : take photo\n";
        msg += "N      : execute next command\n";
        msg += "F      : fullscreen on/off\n";
        msg += "M      : show/hide this manual\n";
        msg += "<,>    : cv threshold -/+\n";
        
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
    msg += "CV thresh  : " + ofToString(threshold,1) + " \n";
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
            msg += "             .\n";
            msg += "             .";
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

void ofApp::drawCvMonitor(int x, int y){
    if(testImg.size()!=0){
        int w = grayImg.getWidth();
        int h = grayImg.getHeight();
        ofRectangle r(0,0,w,h);
        r.scaleTo(ofRectangle(0,0, 200,200));
        ofSetColor(255);
        colorImg.draw(x,y,r.width,r.height);
        grayThresh.draw(x,y+r.height,r.width,r.height);
    }
}

void ofApp::drawCvContour(int x, int y, float scale){
    
    //contourFinder.draw(x, y); // useful for debug

    vector<ofPolyline> polys;
    glm::vec2 st;
    for(int i=0; i<contourFinder.nBlobs; i++){
        const ofxCvBlob & b = contourFinder.blobs[i];
        polys.push_back(ofPolyline());
        ofPolyline & p = polys.back();
        
        for(int i=0; i<b.nPts; i++){
            float ptx = x + b.pts[i].x*scale;
            float pty = y + b.pts[i].y*scale;
            p.addVertex(ptx, pty);
            if(i==0) st = glm::vec2(ptx, pty);
        }
        p.addVertex(st.x, st.y);
    }
    
    ofNoFill();
    ofSetColor(255,0,0);
    for(auto & p : polys){
        p.draw();
    }
    
    ofFill();
    ofSetColor(255,0,0);
    int n = 50;
    for(auto & p : polys){
        for(int i=0; i<n; i++){
            float perc = (float)i/n;
            glm::vec2 pt = p.getPointAtPercent(perc);
            ofDrawCircle(pt, 2);
        }
    }
}
