#include "testApp.h"

unsigned char *pixs;
int r;
float dividWidthScalLat;
float dividWidthScalLon;
//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate( 60 );
    geoMap.loadImage("magFieldImage_t_01.tiff");
    pixs = geoMap.getPixels();
    prceedMap.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
}

//--------------------------------------------------------------
void testApp::update(){
    
    dividWidthScalLat = ofGetWidth()/360;
    dividWidthScalLon = ofGetHeight()/180;
    
    //allocate texture from geomagnetic map to proceedMap. prceedMap makes a contour map with ofTextuer.
    for(int i =1; i<ofGetWidth(); i+=8){
        for(int j =1; j<ofGetHeight(); j+=8){
            
            r = ( pixs[((int)((int)(j*0.1) *geoMap.getWidth()+ (int)(i*0.1)) *3 + 0)]*0.12 * sin(pixs[((int)((int)((j*0.1-1)) * geoMap.getWidth()+ (int)((i*0.1)-1)) * 4 + 0)])+mouseY * 0.2);
            
            pPix[(int)(j *prceedMap.getWidth()+ i) *3 + 0] = r * 5;
            pPix[(int)(j *prceedMap.getWidth()+ i) *3 + 1] = r * 5;
            pPix[(int)(j *prceedMap.getWidth()+ i) *3 + 2] = r * 5;
            
        }
    }
    
    prceedMap.loadData(pPix,prceedMap.getWidth(),prceedMap.getHeight(),GL_RGB);

}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(255);
    
    ofSetColor(255, 255, 255);
    //geoMap.draw(0,0,ofGetWidth(),ofGetHeight());

    prceedMap.draw(0,0,ofGetWidth(),ofGetHeight());
    
    //read texture pixels from geomagnetic map and maek scala map
    for( int i = 1; i < geoMap.getWidth(); i += 1 ){
        for( int j = 1; j < geoMap.getHeight(); j += 1 ){
                ofPoint anchorPoint;
                ofPoint powerPoint;
                float xPoint = (float)( pixs[(int)(j *geoMap.getWidth()+ i) *4 + 0] - 127) ;
                float yPoint = (float)( pixs[(int)(j *geoMap.getWidth()+ i) *4 + 1] - 127) ;
            
                //powerPoint dicribe vector of magnetic value.
                powerPoint = ofPoint( xPoint*0.85 , yPoint*0.85 );

                ofPushMatrix();
                ofTranslate( i * 10 , j * 10);
                
                //rotation makes a effect moire.
                ofRotateZ(mouseX * 0.75);
                //ofNoFill();
                ofSetLineWidth(1);
                ofSetColor( 40, 40, 40 );
                ofLine( 0, powerPoint );
                ofPopMatrix();
        }
     }
     
    ofSetColor(255, 0, 0);
    ofDrawBitmapString( ofToString(ofGetFrameRate()), ofPoint(10,10) );
    
    //draw scale lines
    ofSetColor(255, 255, 255);
    ofSetLineWidth(1);
    
    for(int i =0; i<36; i++){
        int xPoint;
        xPoint = i * 10 * dividWidthScalLat;
        ofLine(xPoint,0,xPoint,10);
    }
    
    for(int i =0; i<18; i++){
        int yPoint;
        yPoint = i * 10 * dividWidthScalLon;
        ofLine(0,yPoint,10,yPoint);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}