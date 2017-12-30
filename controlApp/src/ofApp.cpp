//#define UPLOAD_PHOTO 1
//#define USE_CAMERA_DEVICE 1   // otherwise use test image

#include "ofApp.h"
#include "Command.h"
#include "ofApp_helper.cpp"
#include "ofApp_draw.cpp"
#include "ofApp_key.cpp"

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
    
    machine.setRange(200,160,100);
    generateSequence();
    testSequence((totalSec+1)*ofGetTargetFrameRate());
    machine.init("/dev/cu.usbmodem1411", 115200);
    
    currentCmd = 0;
    machine.makeVbo(cmds);
    
    startFrame = ofGetFrameNum() + 30;

    setupCv();

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
    
    float pickW = 45;
    float pickH = 45;
    
    float dropW = 140;
    float dropH = 120;
    
    int row = 8;
    int col = 8;
    int xySpeed = 10000;    // MAX 10000?
    int zSpeed  = 150;      // MAX 150
    float suckWaitSec = 0.2;
    
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            
            pickPos.x =  38 + (j+1)*pickW/(col+1);
            pickPos.y = 120 + (i+1)*pickH/(row+1);
            pickPos.z = 4;
            
            dropPos.x = 41 + (j+1)*dropW/(col+1);
            dropPos.y = 113 - (i+1)*dropH/(row+1);
            dropPos.z = 2;
            
            if(1){
                // pick up
                addMoveZ(time, pos, 20, zSpeed);
                addMoveY(time, pos, pickPos.y, xySpeed);
                addMoveX(time, pos, pickPos.x, xySpeed);
                addMoveZ(time, pos, 4, zSpeed);
                time += suckWaitSec;
                addSuck(time, pos, true);
            }
            
            if(1){
                // drop down
                addMoveZ(time, pos, 20, zSpeed);
                addMoveX(time, pos, dropPos.x, xySpeed);
                addMoveY(time, pos, dropPos.y, xySpeed);
                addMoveZ(time, pos, dropPos.z, zSpeed);
                addSuck(time, pos, false);
                time += suckWaitSec;
            }
            
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
    filesystem::path path("gcode.txt");
    ofstream file(ofToDataPath(path, true));
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
    
    updateCv();
}

void ofApp::updateCv(){

#ifdef USE_CAMERA_DEVICE
    colorImg.setFromPixels(vidGrabber.getPixels());
#else
    colorImg.setFromPixels(testImg[currentImg].getPixels());
#endif
    grayImg = colorImg;
    
    grayThresh = grayImg;
    grayThresh.threshold(threshold);
    grayThresh.flagImageChanged();
    grayThresh.invert();

    int w = colorImg.getWidth();
    int h = colorImg.getHeight();
    int n = contourFinder.findContours(grayThresh, 80*80, w/3*h/3, 10, false);
}

void ofApp::testSequence(int totalFrame){

    if(machine.serial.isInitialized()){
        assert("Dont call this test after Machine::init");
        // if we call this after init(),
        // too many gcode commands will be sent to actual device
    }
    
    for(int i=0; i<totalFrame; i++){
        machine.update();
        checkSequence(i);
    }
    machine.reset();
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
