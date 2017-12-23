#pragma once

#include <stdio.h>
#include "ofMain.h"
#include "ofRange.h"
#include "Sequence.h"

class GrainCounterController{

public:

    GrainCounterController(){};
    
    void init(string name="/dev/cu.usbmodem1421", int baud=115200){
        serial.listDevices();
        vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
        
        serial.setup(name, baud);
        
        nTimesRead = 0;
        nBytesRead = 0;
        readTime = 0;
        memset(bytesReadString, 0, 4);
    };
    
    void setRange(int xMax, int yMax, int zMax){
        xRange.set(0,xMax);
        yRange.set(0,yMax);
        zRange.set(0,zMax);
    }
    
    void update(){
        
        //  read incoming serial message
        if ( serial.isInitialized() ){
            nTimesRead = 0;
            nBytesRead = 0;
            int nRead  = 0;  // a temp variable to keep count per read
            
            unsigned char bytesReturned[3];
            
            memset(bytesReadString, 0, 4);
            memset(bytesReturned, 0, 3);
            
            while( (nRead = serial.readBytes( bytesReturned, 3)) > 0){
                nTimesRead++;
                nBytesRead = nRead;
            };
            
            memcpy(bytesReadString, bytesReturned, 3);
            
            bSendSerialMessage = false;
            readTime = ofGetElapsedTimef();
        }
    }
    
    static string makeGcode(SeqType type, int val, int speed ){
        char c[255];
        
        switch(type){
            case HOME:   sprintf(c, "G28\n"); break;
            case MOVE_X: sprintf(c, "G0 X%d F%d\n", val, speed); break;
            case MOVE_Y: sprintf(c, "G0 Y%d F%d\n", val, speed); break;
            case MOVE_Z: sprintf(c, "G0 Z%d F%d\n", val, speed); break;
            case SUCK:   sprintf(c, "M104 S%d T0\n",val?190:0);  break;
            case PHOTO:  sprintf(c, "take_photo\n");  break;
        }
        
        return string(c);
    }

    void home(){
        excute(makeGcode(HOME, 0,0));
        targetPos = glm::vec3(0,0,0);
    }
    
    // x : mm
    // speed : mm/min
    void moveToX(int x, int speed=1000){
        excute(makeGcode(MOVE_X, x, speed));
        targetPos.x = x;
    }

    void moveToY(int y, int speed=1000){
        excute(makeGcode(MOVE_Y, y, speed));
        targetPos.y = y;
    }

    void moveToZ(int z, int speed=150){
        excute(makeGcode(MOVE_Z, z, speed));
        targetPos.z = z;
    }
    
    void suck(bool on){
        excute(makeGcode(SUCK, 1, 0));
        bSuck = on;
    }
    
    void excute(const Sequence & s){
        if(serial.isInitialized()){
            serial.writeBytes(s.cmd.c_str(), s.cmd.size());
        
            latestCmd = s.cmd;
            targetSpeed = s.speed;
            
            switch(s.type){
                case HOME:
                    state = "Homing...";
                    break;
                case MOVE_X:
                    state = "moving x";
                    targetPos.x = s.endVal;
                    break;
                case MOVE_Y:
                    state = "moving y";
                    targetPos.y = s.endVal;
                    break;
                case MOVE_Z:
                    state = "moving z";
                    targetPos.z = s.endVal;
                    break;
                case SUCK:
                    bSuck = s.endVal;
                    state = bSuck ? "pump on" : "pump off";
                    break;
                    
                case PHOTO:
                    state = "taking photo";
                    break;
            }
        }
    }
    
    void excute(string cmd){
        if(serial.isInitialized()){
            serial.writeBytes(cmd.c_str(), cmd.size());
        }
    }
    
    ofRange xRange, yRange, zRange;

    glm::vec3 targetPos;
    float targetSpeed;
    bool bSuck = false;
    string state;
    string latestCmd;
    
    ofSerial	serial;
    bool		bSendSerialMessage = false;
    char		bytesRead[3];
    char		bytesReadString[4];
    int			nBytesRead;
    int			nTimesRead;
    float		readTime;
    
};
