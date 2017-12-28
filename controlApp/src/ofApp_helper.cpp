// global func
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

//  helper func
void ofApp::addCommand(float &time, shared_ptr<Command> cmd){
    cmds.push_back(cmd);
    time += cmd->durSec;
}

void ofApp::addHome(float & time){
    addCommand(time, make_shared<HomeCommand>(machine, time));
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

void ofApp::setupCamera(){
    
    //get back a list of devices.
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    
    for(size_t i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    
    vidGrabber.setDeviceID(15);
    vidGrabber.setDesiredFrameRate(30);
    bCamInit = vidGrabber.initGrabber(camWidth, camHeight);
}

void ofApp::takePhoto(){
    if(bSaveRequest && bCamInit){
        string fileName(address+ ".png");
        ofSaveScreen( fileName );
        uploadImage( fileName );
        bSaveRequest = false;
        address = randomIPv6();
    }
}

void ofApp::uploadImage(string fileName){
#ifdef UPLOAD_PHOTO
    ofxHttpForm form;
    form.action = action_url;
    form.method = OFX_HTTP_POST;
    //form.addFormField("number", ofToString(counter));
    form.addFile("fileToUpload", fileName);
    httpUtils.addForm(form);
#endif
}

void ofApp::newResponse(ofxHttpResponse & response){
    cout << response.responseBody << endl;
}
