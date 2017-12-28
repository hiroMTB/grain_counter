#include "ofApp.h"
#include "Command.h"
#include "ofApp_helper.cpp"
#include "ofApp_draw.cpp"
#include "ofApp_key.cpp"

//#define UPLOAD_PHOTO 1

void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofSetLogLevel(OF_LOG_NOTICE);
    
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
    testSequence((totalSec+1)*ofGetTargetFrameRate());
    machine.reset();
    currentCmd = 0;
    machine.makeVbo(cmds);
    
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
    
    float pickW = 40;
    float dropW = 100;
    
    int row = 4;
    int col = 4;
    int xySpeed = 100000; // 3000;
    int zSpeed  = 100000; // 150;
    float suckWaitSec = 0.1;
    
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            
            pickPos.x =  50 + j*(pickW/row);  // 40-80
            pickPos.y = 125 + i*(pickW/row);  // Y117 - Y157
            pickPos.z = 4;
            
            dropPos.x = 50 + j*(dropW/row);
            dropPos.y = 97 - i*(dropW/row);
            dropPos.z = 2;
            
            // pick up
            addMoveZ(time, pos, 20, zSpeed);
            addMoveY(time, pos, pickPos.y, xySpeed);
            addMoveX(time, pos, pickPos.x, xySpeed);
            addMoveZ(time, pos, 4, zSpeed);
            time += suckWaitSec;
            addSuck(time, pos, true);
            
            // drop down
            addMoveZ(time, pos, 20, zSpeed);
            addMoveX(time, pos, dropPos.x, xySpeed);
            addMoveY(time, pos, dropPos.y, xySpeed);
            addMoveZ(time, pos, dropPos.z, zSpeed);
            addSuck(time, pos, false);
            time += suckWaitSec;
            
            if(0){
                // shoot photo
                addMoveZ(time, pos, 15, zSpeed);
                addMoveX(time, pos, dropPos.x-40, xySpeed);
                addMoveY(time, pos, dropPos.y-4, xySpeed);
                addMoveZ(time, pos, 2, zSpeed);
                addPhoto(time, pos);
            }
        }
    }
    
    time += 1;
    addHome(time);
    
    totalSec = time;
    
    // save command sequence to /bin/data/gcode.txt
    ofstream file(ofToDataPath("gcode.txt", true));
    for(auto it : cmds){
        file << it->cmd;
    }
    file.close();
}

void ofApp::update(){
    vidGrabber.update();
    machine.update();
    
    if(bRunSequence){
        checkSequence(currentFrame);
        currentFrame++;
    }
}

//
//  Test and find error command before excute
//
void ofApp::testSequence(int totalFrame){

    for(int i=0; i<totalFrame; i++){
        machine.update();
        checkSequence(i);
    }
}

void ofApp::checkSequence(int now){
    
    int i = 0;
    for(auto it=cmds.begin(); it!=cmds.end(); ++it, i++){
        shared_ptr<Command> command = *it;
        if(command->frame == now){
            bool result = command->call();
            if(command->cmd == "take_photo\n"){
                bSaveRequest = true;
            }
            if(!result) cout << now << " : " << machine.errorMsg << endl;
            currentCmd = i;
        }
    }
}

void ofApp::exit(){
    httpUtils.stop();
}

