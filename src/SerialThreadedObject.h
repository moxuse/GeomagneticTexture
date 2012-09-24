//
//  SerialThreadedObject.h
//  textureReadTest
//
//  Created by Koichiro Mori on 12/08/22.
//
//
#ifndef textureReadTest_SerialThreadedObject_h
#define textureReadTest_SerialThreadedObject_h

#define SERIAL_DEVICE_PATH_NAME "tty.usbserial-A501DG5Z"
//#define SERIAL_DEVICE_PATH_NAME "tty.usbmodem1411"
#define BAUD_RATE 57600

#import "ofmain.h"
//#import "ofxThread.h"

class SerialThreadedObject : public ofThread{

private:
    ofSerial serial;
    string decimalToHex(int value){
        string result("");
        if( value > 0 ){
            stringstream stream;
            stream << hex << value;
            result = stream.str();
        }
        return result;
    }
    
public:
    int touch;
    int xValue;
    int yValue;
    int zValue;
    
    int accelValueX;
    int accelValueY;
    int accelValueZ;
    
    SerialThreadedObject(){
        serial.setup( SERIAL_DEVICE_PATH_NAME ,BAUD_RATE);
        xValue = 0;
        yValue = 0;
        zValue = 0;
        accelValueX = 0;
        accelValueY = 0;
        accelValueZ = 0;
    }

    void start(){
        startThread(true, false);   // blocking, verbose
    }
    
    void stop(){
        stopThread();
    }
    
    void threadedFunction(){
        while( isThreadRunning() != 0 ){
            if( lock() ){
                // Serial Read Loop
                if( serial.available() > 15 ){ //パケット長15バイトバッファにたまったら
                    int checkByte;
                    checkByte = serial.readByte();
                    if( 43 == checkByte ){
                        checkByte = serial.readByte();
                        if( 43 == checkByte ){
                            int touchedByte;
                            touchedByte = serial.readByte();
                            touch = touchedByte;
                                    for(int i=0; i<3;i++){
                                        signed int sensorByte;
                                        int hByte = serial.readByte();
                                        int lByte = serial.readByte();
                                        
                                        sensorByte = (hByte << 8) + lByte; //上位ビットと下位ビット結合
                                        
                                        if( i > 0 ){//y,z軸のみ符号反転
                                            sensorByte = sensorByte - USHRT_MAX;
                                        }
                                        switch(i){
                                            case 0:
                                                xValue = sensorByte;
                                                break;
                                            case 1:
                                                yValue = sensorByte;
                                                break;
                                            case 2:
                                                zValue = sensorByte;
                                                break;
                                        }
                                    }
                            
                                    for(int i=0; i<3;i++){
                                        int accelSensorByte;
                                        int hByte = serial.readByte();
                                        int lByte = serial.readByte();
                                        accelSensorByte = (hByte << 8) + lByte; //上位ビットと下位ビット結合
                                        
                                        switch(i){
                                            case 0:
                                                accelValueX = accelSensorByte;
                                                break;
                                            case 1:
                                                accelValueY = accelSensorByte;
                                                break;
                                            case 2:
                                                accelValueZ = accelSensorByte;
                                                break;
                                        }
                                    }
                    } else {
                        for(int i = 0; i< 13; i++){
                            serial.readByte();
                        }
                    }
                }
                    
                    
                }
                unlock();
                ofSleepMillis(1);
            }
        }
    }
            
    
};

#endif
