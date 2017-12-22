#pragma once

#include <stdio.h>
#include "ofMain.h"
#include "ofRange.h"

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

    string home(){
        char c[255];
        int n = sprintf(c, "G28\n");
        serial.writeBytes(c, n);
        currentCommand = string(c);
        targetPosition = glm::vec3(0,0,0);
        return string(c);
    }
    
    // x : mm
    // speed : mm/min
    string moveToX(int x, int speed=500){
        char c[255];
        int n = sprintf(c, "G0 X%d F%d\n", x, speed);
        serial.writeBytes(c, n);
        targetPosition.x = x;
        targetSpeed = speed;
        return string(c);
    }

    string moveToY(int y, int speed=500){
        char c[255];
        int n = sprintf(c, "G0 Y%d F%d\n", y, speed);
        serial.writeBytes(c, n);
        targetPosition.y = y;
        targetSpeed = speed;

        return  string(c);
    }

    string moveToZ(int z, int speed=150){
        char c[255];
        int n = sprintf(c, "G0 Z%d F%d\n", z, speed);
        serial.writeBytes(c, n);
        targetPosition.z = z;
        targetSpeed = speed;

        return string(c);
    }
    
    string suck(bool on ){
        char c[255];
        int v = on ? 190 : 0;
        int n = sprintf(c, "M104 S%d T0\n", v);
        serial.writeBytes(c, n);
        bSuck = on;
        return string(c);
    }
    
    void excute(string cmd){
        serial.writeBytes(cmd.c_str(), cmd.size());
    }
    
    
    ofRange xRange, yRange, zRange;
    glm::vec3 targetPosition;
    float targetSpeed;
    bool bSuck = false;
    
    ofSerial	serial;
    bool		bSendSerialMessage = false;
    char		bytesRead[3];
    char		bytesReadString[4];
    int			nBytesRead;
    int			nTimesRead;
    float		readTime;
    
    string currentCommand;
    
};
