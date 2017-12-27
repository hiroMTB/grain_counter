#include "ofApp.h"
#include "Command.h"

//#define UPLOAD_PHOTO 1

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

void ofApp::setup(){
    ofSetVerticalSync(true);
    
    ofSetFrameRate(30);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    setupCamera();
    
#ifdef UPLOAD_PHOTO
    action_url = "http://ingorandolf.info/sandgraincounter/upload.php";
    ofAddListener(httpUtils.newResponseEvent,this,&ofApp::newResponse);
    httpUtils.start();
#endif
    address = randomIPv6();
    
    machine.init("/dev/cu.usbmodem1411", 115200);
    machine.setRange(200,200,100);
    generateSequence();
    machine.makePath(cmds);
    
    startFrame = ofGetFrameNum() + 30;

    // Homing
    sleep(1);    
    HomeCommand m(machine, 0);
    m.call();
}

void ofApp::generateSequence(){

    glm::vec3 pickPos;
    glm::vec3 dropPos;
    glm::vec3 pos(0,0,0);
    
    float time = 0;
    
    int row = 8;
    int col = 8;
    int xySpeed = 500;
    int zSpeed = 150;
    
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            
            pickPos.x =  50 + j*3;  // 40-80
            pickPos.y = 125 + i*4;  // Y117 - Y157
            pickPos.z = 4;
            
            dropPos.x = 60 + j*15.5;
            dropPos.y = 97 - i*10;
            dropPos.z = 2;
            
            // 1. move Z up
            addMoveZ(time, pos, 20, zSpeed);
            
            // 2. move X to pickPos
            addMoveX(time, pos, pickPos.x, xySpeed);

            // 2. move Y to pickPos
            addMoveY(time, pos, pickPos.y, xySpeed);

            // down to grain
            addMoveZ(time, pos, 10, zSpeed);

            // 5. turn on pump
            addSuck(time, pos, true);

            // 6. more closer to grain
            addMoveZ(time, pos, 4, zSpeed);
            time += 1;
            
            // 7. lift up
            addMoveZ(time, pos, 20, zSpeed);

            // 8. go to drop down position YX
            addMoveY(time, pos, dropPos.y, xySpeed);
            
            // 9. go to drop down position X
            addMoveX(time, pos, dropPos.x, xySpeed);

            // 10. down
            addMoveZ(time, pos, dropPos.z, zSpeed);

            // 11. drop
            addSuck(time, pos, false);
            time+=1;
            
            // 12. move up
            addMoveZ(time, pos, 15, zSpeed);

            // 13. cameara pos X
            addMoveX(time, pos, dropPos.x-40, xySpeed);

            // 14. cameara pos Y
            addMoveY(time, pos, dropPos.y-4, xySpeed);
            
            // 15. move down
            addMoveZ(time, pos, 2, zSpeed);

            // 16. take photo
            addPhoto(time, pos);
        }
    }
    
    // print out sequence for check
    for(auto it : cmds){
        printf("Make command : %05df %s", it->frame, it->cmd.c_str());
    }
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

void ofApp::update(){
    vidGrabber.update();
    machine.update();
    
    if(bRunSequence){
        checkSequence(currentFrame);
        currentFrame++;
    }
}

void ofApp::checkSequence(int now){
    
    int i = 0;
    for(auto it=cmds.begin(); it!=cmds.end(); ++it, i++){
        shared_ptr<Command> command = *it;
        if(command->frame == now){
            command->call();
            if(command->cmd == "take_photo\n"){
                bSaveRequest = true;
            }
            printf("Call command %05df : %s", command->frame, command->cmd.c_str());

            currentCmd = i;
        }
    }
}

void ofApp::draw(){
    
    ofBackground(255);
    ofSetColor(255);

    ofPushMatrix();
    vidGrabber.draw(0,0, camWidth*2, camHeight*2);
    ofPopMatrix();
    
    ofPushMatrix();{
        ofTranslate(18, 0);
        
        // draw info
        ofTranslate( 0, ofGetHeight()-30);
        
        if(machine.serial.isInitialized()){
            string msg1 = "position : " + ofToString(machine.targetPos) + "\n";
            string msg2 = "IPv6     : " + address;
            ofDrawBitmapString(msg1, 0, 0);
            ofDrawBitmapString(msg2, 0, 20);
        }
    } ofPopMatrix();
    
    if(bSaveRequest && bCamInit){
        string fileName(address+ ".png");
        ofSaveScreen( fileName );
#ifdef UPLOAD_PHOTO
        uploadImage( fileName );
#endif
        bSaveRequest = false;
        address = randomIPv6();
    }
    
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
    
    machine.draw(ofGetWidth()-220, 20);
}

void ofApp::keyPressed  (int key){

    if(key == ' '){
        bRunSequence = !bRunSequence;
    }
    
    // machine manual control
    if(machine.serial.isInitialized()){
        
        glm::vec3 tp = machine.targetPos;
        int xySpeed = 300;
        int zSpeed  = 150;
        
        switch(key){
            case 'H':
            {
                HomeCommand m(machine, 0);
                m.call();
                break;
            }
            case OF_KEY_LEFT:
            {
                MoveCommand m(machine, MoveCommand::AXIS_X, 0, tp, tp.x-1, xySpeed);
                m.call();
                break;
            }
                
            case OF_KEY_RIGHT:
            {
                MoveCommand m(machine, MoveCommand::AXIS_X, 0, tp, tp.x+1, xySpeed);
                m.call();
                break;
            }
                
            case OF_KEY_UP:
            {
                MoveCommand m(machine, MoveCommand::AXIS_Y, 0, tp, tp.y-1, xySpeed);
                m.call();
                break;
            }
                
            case OF_KEY_DOWN:
            {
                MoveCommand m(machine, MoveCommand::AXIS_Y, 0, tp, tp.y+1, xySpeed);
                m.call();
                break;
            }

            case '[':
            {
                MoveCommand m(machine, MoveCommand::AXIS_Z, 0, tp, tp.z-1, zSpeed);
                m.call();
                break;
            }
            case ']':
            {
                MoveCommand m(machine, MoveCommand::AXIS_Z, 0, tp, tp.z+1, zSpeed);
                m.call();
                break;
            }

            case OF_KEY_RETURN:
            {
                SuckCommand m(machine, 0, tp, !machine.bSuck);
                m.call();
                break;
            }
            
            case 'n':
            {
                shared_ptr<Command> command = cmds[currentCmd];
                command->call();
                currentFrame = command->frame;
                currentCmd++;
                currentCmd = currentCmd % cmds.size();
                cout << command->cmd << endl;
                break;
            }
        }
    }

    // video
    switch(key){
        case 'S':
            bSaveRequest = true;
            break;
    }
    
}

void ofApp::newResponse(ofxHttpResponse & response){
    cout << response.responseBody << endl;
}

void ofApp::uploadImage(string fileName){
    ofxHttpForm form;
    form.action = action_url;
    form.method = OFX_HTTP_POST;
    //form.addFormField("number", ofToString(counter));
    form.addFile("fileToUpload", fileName);
    httpUtils.addForm(form);
}

void ofApp::exit(){
    httpUtils.stop();
}

//
//  helper func
//
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
