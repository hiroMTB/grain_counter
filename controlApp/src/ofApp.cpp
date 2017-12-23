#include "ofApp.h"

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
    

    gcc.init("/dev/cu.usbmodem1411", 115200);
    gcc.setRange(20,20,10);
    generateSequence();
    
    startFrame = ofGetFrameNum() + 30;

    // Homing
    sleep(1);
    gcc.home();

}

void ofApp::generateSequence(){

    glm::vec3 pickPos;
    glm::vec3 dropPos;
    glm::vec3 pos(0,0,0);
    
    float time = 0;
    
    int row = 8;
    int col = 8;
    int xySpeed = 10000;
    int zSpeed = 150;
    
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            
            pickPos.x =  50 + j*3;  // 40-80
            pickPos.y = 125 + i*3;  // Y117 - Y157
            pickPos.z = 4;
            
            dropPos.x = 60 + j*10;
            dropPos.y = 97 - i*10;
            dropPos.z = 2;
            
            // 1. move Z up
            Sequence s;
            s.z(time, pos, 20, zSpeed);
            seq.push_back(s); time+=s.durSec;
            
            // 2. move X to the middle
            s.x(time, pos, pickPos.x,xySpeed);;
            seq.push_back(s); time+=s.durSec;

            // 3. move Y to the middle
            s.y(time, pos, pickPos.y, xySpeed);;
            seq.push_back(s); time+=s.durSec;
            
            // 4. down to grain
            s.z(time, pos, 10, zSpeed);
            seq.push_back(s); time+=s.durSec;

            // 5. turn on pump
            s.suck(time, true);
            seq.push_back(s); time+=s.durSec;
            
            // 6. more closer to grain
            s.z(time, pos, 4, zSpeed);
            seq.push_back(s); time+=s.durSec;
            time += 1;
            
            // 7. lift up
            s.z(time, pos, 20, zSpeed);
            seq.push_back(s); time+=s.durSec;
            
            // 8. go to drop down position Y
            s.y(time, pos, dropPos.y, xySpeed);
            seq.push_back(s); time+=s.durSec;

            // 9. go to drop down position X
            s.x(time, pos, dropPos.x, xySpeed);
            seq.push_back(s); time+=s.durSec;

            // 10. down
            s.z(time, pos, dropPos.z, zSpeed);
            seq.push_back(s); time+=s.durSec;

            // 11. drop
            s.suck(time, false);
            seq.push_back(s); time+=s.durSec;
            time+=1;
            
            // 12. move up
            s.z(time, pos, 15, zSpeed);
            seq.push_back(s); time+=s.durSec;

            // 13. cameara pos X
            s.x(time, pos, pos.x-40, xySpeed);
            seq.push_back(s); time+=s.durSec;

            // 14. cameara pos Y
            s.y(time, pos, pos.y-4, xySpeed);
            seq.push_back(s); time+=s.durSec;
            
            // 15. move down
            s.z(time, pos, 2, zSpeed);
            seq.push_back(s); time+=s.durSec;

            // 16. take photo
            s.photo(time);
            seq.push_back(s); time+=s.durSec;
        }
    }
    
    // print out sequence for check
    for(Sequence & s : seq){
       // printf("%04df, %03.1fsec : %s", s.frame, s.frame/ofGetTargetFrameRate(), s.cmd.c_str());
    }
    cout << endl;
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
    gcc.update();
    
    if(bRunSequence){
        checkSequence(currentFrame);
        currentFrame++;
    }
}

void ofApp::checkSequence(int now){
    
    for(int i=0; i<seq.size(); i++){

        Sequence & s = seq[i];
        
        float time = s.frame;
        
        if(time == now){
            currentSeq = i;
            string cmd = s.cmd;
            if(cmd == "take_photo\n"){
                bSaveRequest = true;
            }else{
                gcc.excute(s);
            }
            float now = currentFrame/ofGetTargetFrameRate();
            printf("now %03.1fsec, scheduled %03.1fsec : %s", now, s.frame/ofGetTargetFrameRate(), s.cmd.c_str());
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
        
        if(gcc.serial.isInitialized()){
            string msg1 = "position : " + ofToString(gcc.targetPos.x) + ", " + ofToString(gcc.targetPos.y) + ", " + ofToString(gcc.targetPos.z) + "\n";
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
        string msg = "Press SPACE key to start pick & place sequences";
        ofSetColor(255,0,0);
        ofDrawBitmapString(msg, 18, 25);
    }else{
        string msg;
        msg += "state : " + gcc.state + "\n";
        msg += "speed : " + ofToString(gcc.targetSpeed)  + "\n";
        msg += "gcode : " + ofToString(gcc.latestCmd);
        msg += "read  : " + ofToString(gcc.bytesReadString) + "\n";
        ofSetColor(255);
        ofDrawBitmapString(msg, 18, 25);
    }    
}

void ofApp::keyPressed  (int key){

    if(key == ' '){
        bRunSequence = !bRunSequence;
    }
    
    // serial
    if(gcc.serial.isInitialized()){
        
        switch(key){
            case 'H':
                gcc.home();
                break;
            case OF_KEY_LEFT:
                gcc.moveToX(gcc.targetPos.x-1);
                break;
                
            case OF_KEY_RIGHT:
                gcc.moveToX(gcc.targetPos.x+1);
                break;
                
            case OF_KEY_UP:
                gcc.moveToY(gcc.targetPos.y+1);
                break;
                
            case OF_KEY_DOWN:
                gcc.moveToY(gcc.targetPos.y-1);
                break;

            case '[':
                gcc.moveToZ(gcc.targetPos.z+1);
                break;

            case ']':
                gcc.moveToZ(gcc.targetPos.z-1);
                break;

            case OF_KEY_RETURN:
                gcc.suck(!gcc.bSuck);
                break;
            
            case 'n':
            {
                Sequence & s = seq[currentSeq];
                gcc.excute(s);
                currentFrame = s.frame;
                currentSeq++;
                currentSeq = currentSeq % seq.size();
                cout << s.cmd << endl;
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
