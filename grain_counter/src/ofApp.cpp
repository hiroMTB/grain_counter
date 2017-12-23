#include "ofApp.h"

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
    
    ofBackground(255);
    ofSetFrameRate(30);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    gcc.init("/dev/cu.usbmodem1411", 115200);
    gcc.setRange(20,20,10);
    setupCamera();
    
    action_url = "http://ingorandolf.info/sandgraincounter/upload.php";
    ofAddListener(httpUtils.newResponseEvent,this,&ofApp::newResponse);
    httpUtils.start();
    
    generateSequence();
    
    address = randomIPv6();
    
    startFrame = ofGetFrameNum() + 30;
    
}

void ofApp::generateSequence(){

    glm::vec3 dropPos;
    
    float time = 3;
    
    // Homing
    seq.push_back(Sequence(time, gcc.home()));
    time = 10;
    
    int row = 8;
    int col = 8;
    int wait = 5;
    
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            
            dropPos.x = 60 + (i*10);
            dropPos.y = 137 - 40 - (j*10);
            dropPos.z = 4;
            
            
            // 1. move Z up
            seq.push_back(Sequence(time, gcc.moveToZ(20)) );
            time += wait;
            
            // 2. move X to the middle
            seq.push_back(Sequence(time, gcc.moveToX(60)) ); // 40-80
            time += wait;

            // 3. move Y to the middle
            seq.push_back(Sequence(time, gcc.moveToY(137)) ); // Y117 - Y157
            time += wait;

            // 4. down to grain
            seq.push_back(Sequence(time, gcc.moveToZ(10)) ); // Z 10
            time += wait;
            
            // 5. turn on pump
            seq.push_back(Sequence(time, gcc.suck(true)) );
            time += 1;
            
            // 6. more closer to grain
            seq.push_back(Sequence(time, gcc.moveToZ(4)) );
            time += 2;
            
            // 7. lift up
            seq.push_back(Sequence(time, gcc.moveToZ(20)) );
            time += wait;
            
            // 8. go to drop down position
            seq.push_back(Sequence(time, gcc.moveToX(dropPos.x)) );   // ? - ?
            time += wait;

            // 9. go to drop down position
            seq.push_back(Sequence(time, gcc.moveToY(dropPos.y)) );   // ? - ?
            time += wait;
            
            // 10. down
            seq.push_back(Sequence(time, gcc.moveToZ(2)) );   // ? - ?
            time += wait;

            // 11. drop
            seq.push_back(Sequence(time, gcc.suck(false)) );
            time += 1;

            // 12. move up
            seq.push_back(Sequence(time, gcc.moveToZ(15)) );
            time += wait;

            // 13. cameara pos X
            seq.push_back(Sequence(time, gcc.moveToX(gcc.targetPosition.x-40)) );
            time += wait;

            // 14. cameara pos Y
            seq.push_back(Sequence(time, gcc.moveToY(gcc.targetPosition.y-4)) );
            time += wait;
            
            // 15. move down
            seq.push_back(Sequence(time, gcc.moveToZ(2)) );
            time += wait+4;

            // 16. take photo
            seq.push_back(Sequence(time, "take_photo") );
            time += 5;
            
        }
    }
}

float ofApp::calcWait(float posSt, float posEnd, float speed){
    
    float dist = posEnd - posSt;
    float waitTime = dist / speed;
    waitTime += 0.5;
    return waitTime;
    
}


void ofApp::setupCamera(){
    camWidth  = 320 * 2;
    camHeight = 240 * 2;
    
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
    vidGrabber.initGrabber(camWidth, camHeight);
}

void ofApp::update(){
    vidGrabber.update();
    gcc.update();
    
    float now = ofGetFrameNum() - startFrame;
    checkSequence(now);
    
}

void ofApp::checkSequence(int now){
    
    for(int i=0; i<seq.size(); i++){

        Sequence & s = seq[i];
        
        float time = s.frame;
        
        if(time == now){
            string cmd = s.cmd;
            if(cmd == "take_photo"){
                bSaveRequest = true;
            }else{
                gcc.excute(cmd);
            }
            cout << cmd << endl;
        }
    }
}


void ofApp::draw(){
    
    ofBackground(255);
    
    ofPushMatrix();
    vidGrabber.draw(0,0, camWidth*2, camHeight*2);
    ofPopMatrix();
    
    ofTranslate(18, 0);
    // send section
    //ofDrawBitmapString(command, 0, 50);
    
    // receive section
    ofTranslate( 0, ofGetHeight()-30);
    
    
    if(gcc.serial.isInitialized()){
        //msg += "nBytes read " + ofToString(gcc.nBytesRead) + "\n";
        //msg += "nTimes read " + ofToString(gcc.nTimesRead) + "\n";
        //msg += "read: " + ofToString(gcc.bytesReadString) + "\n";
        string msg1 = "position : " + ofToString(gcc.targetPosition.x) + ", " + ofToString(gcc.targetPosition.y) + ", " + ofToString(gcc.targetPosition.z) + "\n";
        string msg2 = "IPv6     : " + address;
        ofDrawBitmapString(msg1, 0, 0);
        ofDrawBitmapString(msg2, 0, 20);
    }else{
        
    }

    if(bSaveRequest){
        string fileName(address+ ".png");
        ofSaveScreen( fileName );
        uploadImage( fileName );
        bSaveRequest = false;
        address = randomIPv6();
    }

}

void ofApp::keyPressed  (int key){

    // serial
    if(gcc.serial.isInitialized()){
        
        switch(key){
            case 'H':
                gcc.home(true);
                break;
            case OF_KEY_LEFT:
                gcc.moveToX(gcc.targetPosition.x-1, true);
                break;
                
            case OF_KEY_RIGHT:
                gcc.moveToX(gcc.targetPosition.x+1, true);
                break;
                
            case OF_KEY_UP:
                gcc.moveToY(gcc.targetPosition.y+1, true);
                break;
                
            case OF_KEY_DOWN:
                gcc.moveToY(gcc.targetPosition.y-1, true);
                break;

            case '[':
                gcc.moveToZ(gcc.targetPosition.z+1, true);
                break;

            case ']':
                gcc.moveToZ(gcc.targetPosition.z-1, true);
                break;

            case OF_KEY_RETURN:
                gcc.suck(!gcc.bSuck, true);
                break;
            
            case 'n':
            {
                Sequence & s = seq[currentSeq];
                string cmd = s.cmd;
                gcc.excute(cmd);
                currentSeq++;
                currentSeq = currentSeq % seq.size();
                cout << cmd << endl;
                break;
            }
        }
    }

    // video
    switch(key){
        case 'S':
        {
            bSaveRequest = true;
            break;
        }
        
        case 'U':
            uploadImage("test.png");
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
