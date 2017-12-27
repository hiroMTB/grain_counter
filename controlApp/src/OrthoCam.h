#pragma once

#include "ofMain.h"

class OrthoCam{

public:
    
    OrthoCam(){};
    void begin(ofRectangle viewport = ofGetCurrentViewport()){
        ofPushView();
        ofViewport(viewport);
        ofSetupScreenOrtho(viewport.width, viewport.height, -10000, 10000);
        ofPushMatrix();
        //ofRotateXDeg(orientation.x);
        //ofRotateYDeg(orientation.y);        
        //ofTranslate(translation*orientationMatrix);
        //ofScale(1,1 * (bFlipY?-1:1),1);
    }
    
    void end(){
        ofPopMatrix();
        ofPopView();
    }

    //ofVec3f orientation;
    //ofMatrix4x4 orientationMatrix;
  	//ofVec3f translation;
};
