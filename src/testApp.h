#pragma once

#include "ofMain.h"
#import "SerialThreadedObject.h"
#include "ofxCsv.h"
#include "ofxXmlSettings.h"

#define MAX_NUM_OF_LOW 19999

class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    double calculate_heading(const ofMatrix3x3 &dcm_matrix,double mag_x, double mag_y, double mag_z);
    double calculate_heading( double pitch, double roll, double mag_x, double mag_y, double mag_z);
    void drawConsole();
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void drawScaleLines();
    void setupCsv();
    void drawDebugConsole();
    void drawCross(ofPoint point, int crossLnegth);
    ofMatrix3x3 rMatrixFromEulerAngles(double Roll, double Pitch, double Yaw);
    double constrain(double x, double a, double b);
    void changeDivSpeed();
    
private:
    wng::ofxCsv csv;
    ofImage localMap;
    ofImage refImage;
    ofTexture targetTex;
    
    SerialThreadedObject STO;
    
    float sensorControllX;
    float sensorControllY;
    float sensorControllZ;
    
    float sensorPastValX;
    float sensorPastValY;
    float sensorPastValZ;
    
    float offsetSensorValX;
    float offsetSensorValY;
    float offsetSensorValZ;
    
    
    float accelSensorControllX;
    float accelSensorControllY;
    float accelSensorControllZ;
    
    float accelSensorPastValX;
    float accelSensorPastValY;
    float accelSensorPastValZ;
    
    float accelOffsetSensorValX;
    float accelOffsetSensorValY;
    float accelOffsetSensorValZ;

    float magDegree;
    float magDegreeBefore;
    
    int simReadTime;
    
    float xSimValue[MAX_NUM_OF_LOW];
    float ySimValue[MAX_NUM_OF_LOW];
    float zSimValue[MAX_NUM_OF_LOW];
    float vlat[MAX_NUM_OF_LOW];
    float vlon[MAX_NUM_OF_LOW];
    float valt[MAX_NUM_OF_LOW];
    
    vector<ofPoint> crossPoint;
    
    
    double friction;
    double spring;
    
    double speedX;
    double currentReadPosX ;
    double nextReadPosX;
    
    double speedY;
    double currentReadPosY;
    double nextReadPosY;
    
    double speedInvadorPosture;
    double currentInvadorPosture;
    double nextInvadorPosture;
    
    double speedMagSensorZFlipSmooth;
    double currentMagSensorZFlipSmooth;
    double nextMagSensorZFlipSmooth;
    
    ofxXmlSettings xmlPreset;
    
    ofTrueTypeFont invadorFont24;
    ofTrueTypeFont invadorFont16;
    
    ofImage footerImage;

};
