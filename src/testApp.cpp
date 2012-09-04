#include "testApp.h"

#define DEBUG 0
#define SEVA_XML 0

#define REF_TEXTURE_WIDTH 2160
#define TARGET_TEXTURE_WIDTH 540
#define REF_TEXTURE_HEIGHT 1080
#define TARGET_TEXTURE_HEIGHT 360
#define HEADER_PIXEL_NUM 300

#define TOUCH_COUNT_FRAME_NUM 200

//#define GEO_MAG_DEGREE_OFFSET_NORTH 12.0

#define CSV_COL_NUMBER_GEOMAG 4
#define CSV_COL_NUMBER_GEO_COORDINATES 10
#define FLOAT_NORMALISE_VALUE 1.0

#define SCALE_POINT_REPAT_NUM_WIDTH 23
#define SCALE_POINT_REPAT_NUM_HEIGHT 18

#define SENSOR_MAG_MAX 340.0

float iccOffsetDegre = -120.0;

int smootDivNum = 8;

unsigned char *refPix;
unsigned char targetPix[ TARGET_TEXTURE_WIDTH * TARGET_TEXTURE_HEIGHT * 3 ];

double centerOfPixelReadX  = 0;
double centerOfPixelReadY  = 0;

float dividWidthScalLat;
float dividWidthScalLon;

int isTouchedDevice;
int isTouchedDeviceCount;

float localMapAlpha = 0;

bool monoColorMode = false;

int rulerRefernceDegreeX[48] = {90,75,60,45,30,15,0,-15,-30,-45,-60,-75,-90,-105,-120,-135,-150,-165,-180,-165,-150,-135,-120,-105,-90,-75,-60,-45,-30,-15,0,15,30,45,60,75,90,105,120,135,150,165,180,165,150,135,120,105};
int rulerRefernceDegreeY[24] = {75,60,45,30,15,0,-15,-30,-45,-60,-75,-90,-75,-60,-45,-30,-15,0,15,30,45,60,75,90};

float localmapPitch;
float localmapRoll;

float totuchAcelBefoure = 0;
float totuchMagBefoure = 0;

//--------------------------------------------------------------
void testApp::setup(){

    friction = 0.05;
    spring = 0.5;
    
    speedX = 0;
    currentReadPosX = 0;
    nextReadPosX = 0;
    
    speedY = 0;
    currentReadPosY = 0;
    nextReadPosY = 0;
    
    speedInvadorPosture = 0;
    currentInvadorPosture = 0;
    nextInvadorPosture = 0;
    
    speedMagSensorZFlipSmooth = 0;
    currentMagSensorZFlipSmooth = 0;
    nextMagSensorZFlipSmooth = 0;
    
    once = true;
    footerImage.loadImage("GT.tif");
    refImage.loadImage("magFieldImage_smoothed_2160.png");
    localMap.loadImage("localmap.png");
    arrowImage.loadImage("arrow.png");
    targetTex.allocate(TARGET_TEXTURE_WIDTH, TARGET_TEXTURE_HEIGHT, GL_RGB);
    localMapTextuer.allocate(localMap.getWidth(),localMap.getHeight(),GL_RGBA);
    localMapTextuer.loadData(localMap.getPixels(),localMap.getWidth(),localMap.getHeight(),GL_RGBA);
    
    invadorFont24.loadFont("font/AvantGarde-Medium.otf", 24);
    invadorFont12.loadFont("font/AvantGarde-Medium.otf", 16);
    
    xmlPreset.loadFile("settings.xml");
    
    setupCsv();
    
    dividWidthScalLat = ofGetWidth() / 360;
    dividWidthScalLon = ofGetHeight() / 180;
    
    refPix = refImage.getPixels();
    
    isTouchedDevice = 0;
    isTouchedDeviceCount = TOUCH_COUNT_FRAME_NUM;
    
    sensorPastValX = 0;
    sensorPastValY = 0;
    sensorPastValX = 0;
    
    magDegree = 0 ;
    
    for( int i=0; i< SCALE_POINT_REPAT_NUM_WIDTH; i++ ){
        for( int j = 0; j< SCALE_POINT_REPAT_NUM_HEIGHT; j++ ){
            crossPoint.push_back(ofPoint(i*270, j*270));
        }
    }
    
    for(int i = 0 ; i< 48; i++){
        rulerNodeX[i].setup();
    }
    
    for(int i = 0 ; i< 24; i++){
        rulerNodeY[i].setup();
    }
    resetGraphArray();
    
    ofHideCursor();
    
    STO.start();
    
    ofSetFullscreen(true);
    for(int i = 0; i<5; i++){
        moveAverage[i] = 0;
        moveAverageMag[i] = 0;
    }

}

//--------------------------------------------------------------
void testApp::update(){
    
    if( once && ofGetFrameNum() > 100 ){
//        offsetSensorValX = STO.xValue;
//        offsetSensorValY = STO.yValue;
//        offsetSensorValZ = STO.zValue;

        offsetSensorValX = xmlPreset.getValue("sensorMagX", STO.xValue);
        offsetSensorValY = xmlPreset.getValue("sensorMagY", STO.yValue);
        offsetSensorValZ = xmlPreset.getValue("sensorMagZ", STO.zValue);
        accelOffsetSensorValX = xmlPreset.getValue("sensorAccelX", STO.accelValueX);
        accelOffsetSensorValX = xmlPreset.getValue("sensorAccelY", STO.accelValueY);
        accelOffsetSensorValX = xmlPreset.getValue("sensorAccelZ", STO.accelValueZ);
#if SEVA_XML == 1
        offsetSensorValX = xmlPreset.setValue("sensorMagX", STO.xValue);
        offsetSensorValY = xmlPreset.setValue("sensorMagY", STO.yValue);
        offsetSensorValZ = xmlPreset.setValue("sensorMagZ", STO.zValue);
        accelOffsetSensorValX = xmlPreset.setValue( "sensorAccelX", STO.accelValueX );
        accelOffsetSensorValY = xmlPreset.setValue( "sensorAccelY", STO.accelValueY );
        accelOffsetSensorValZ  = xmlPreset.setValue( "sensorAccelZ", STO.accelValueZ );
        xmlPreset.saveFile("settings.xml");
#endif
        //accelOffsetSensorValZ = STO.accelValueZ * (-1);
        once = false;
    }
    
#pragma mark -smooth liner hokan by spring
    
    if(ofGetFrameNum()%smootDivNum==0){
        simReadTime ++;
        nextReadPosX = vlat[simReadTime];
        nextReadPosY = vlon[simReadTime];
        nextInvadorPosture = atan2(ySimValue[simReadTime], xSimValue[simReadTime] )  * (180 / M_PI) + ofGetFrameNum() * 0.025;
    }
    
    float springX;
    springX = (nextReadPosX - currentReadPosX) * spring;
    speedX += springX;
    speedX *= friction;
    currentReadPosX += speedX;
    
    float springY;
    springY = (nextReadPosY - currentReadPosY) * spring;
    speedY += springY;
    speedY *= friction;
    currentReadPosY += speedY;
    
    float springPosture;
    springPosture = (nextInvadorPosture - currentInvadorPosture) * spring;
    speedInvadorPosture += springPosture;
    speedInvadorPosture *= friction;
    currentInvadorPosture += speedInvadorPosture;
    
    float springMagSensorZFlipSmooth;
    springPosture = (nextMagSensorZFlipSmooth - currentMagSensorZFlipSmooth) * spring;
    speedMagSensorZFlipSmooth += springMagSensorZFlipSmooth;
    speedMagSensorZFlipSmooth *= friction;
    currentMagSensorZFlipSmooth += speedMagSensorZFlipSmooth;

    if( simReadTime > MAX_NUM_OF_LOW ){
        simReadTime = 0;
//        if( monoColorMode ){
//            monoColorMode = false;
//        } else {
//            monoColorMode = true;
//        }
    }
    
#pragma mark - increase Touch Sensor
    float currentAccelZ = 0;
    float currentMagX = 0;
    currentAccelZ = STO.accelValueZ *0.2 + totuchAcelBefoure * 0.8;
    currentMagX = STO.xValue *0.2 + totuchMagBefoure * 0.8;
    totuchAcelBefoure = currentAccelZ;
    totuchMagBefoure = currentMagX;
    
    bool isToucheByAccel;
    float average = 0;
    float averageMag = 0;
    for(int i = 4; i >= 1; i--){
        moveAverage[i] = moveAverage[i-1];
        moveAverageMag[i] = moveAverageMag[i-1];
    }
    moveAverage[0] = currentAccelZ;
    moveAverageMag[0] = currentMagX;
    for(int i = 0; i < 5; i++){
        average += moveAverage[i];
        averageMag += moveAverageMag[i];
    }
    average = average / 5;
    averageMag = averageMag / 5;
    if( abs( float(average - currentAccelZ) ) > 14 &&  abs( float(averageMag - currentMagX) ) > 14 ){
        isToucheByAccel = true;
    } else {
        isToucheByAccel = false;
    }
    
    //cout << isToucheByAccel << " " <<   abs( float(averageMag - currentMagX) )  << endl;
    
    if(isToucheByAccel){
        isTouchedDeviceCount = TOUCH_COUNT_FRAME_NUM;
        isTouchedDevice = 1;
    } else {
        isTouchedDeviceCount--;
        if(isTouchedDeviceCount < 0 ){
            isTouchedDeviceCount = 0;
            resetGraphArray();
        };
    }
//    
//    if( STO.touch == 0 ){
//        isTouchedDeviceCount--;
//        if(isTouchedDeviceCount < 0 ){
//            isTouchedDeviceCount = 0;
//            resetGraphArray();
//        };
//    } else {
//        isTouchedDeviceCount = TOUCH_COUNT_FRAME_NUM;
//        isTouchedDevice = 1;
//    };
    
#pragma mark - touched Mode
    if( isTouchedDeviceCount == 0 )isTouchedDevice = 0;
    //isTouchedDevice=true;
    if(isTouchedDevice){
        
        /////////////////////////////////////
        
        sensorControllX = ( STO.xValue - offsetSensorValX ) * 0.15 + sensorPastValX * 0.85; //smoothing
        sensorControllY = ( STO.yValue - offsetSensorValY ) * 0.15 + sensorPastValY * 0.85; //smoothing
        sensorControllZ = ( STO.zValue - offsetSensorValZ ) * 0.15 + sensorPastValZ * 0.85; //smoothing
        
        sensorPastValX = sensorControllX;
        sensorPastValY = sensorControllY;
        sensorPastValZ = sensorControllZ;
        
        accelSensorControllX = ( STO.accelValueX - accelOffsetSensorValX ) * 0.15 + accelSensorPastValX * 0.85; //smoothing
        accelSensorControllY = ( STO.accelValueY - accelOffsetSensorValY ) * 0.15 + accelSensorPastValY * 0.85; //smoothing
        accelSensorControllZ = ( STO.accelValueZ - SENSOR_MAG_MAX * 0.5 ) * 0.15 + accelSensorPastValZ * 0.85; //smoothing
        
        accelSensorPastValX = accelSensorControllX;
        accelSensorPastValY = accelSensorControllY;
        accelSensorPastValZ = accelSensorControllZ;

#pragma mark - calcurate Mag Device Hedding
        //koko 3D nisuru !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        
        nextMagSensorZFlipSmooth = accelSensorControllZ;
        
        double pitch, roll, yaw;
        
//        pitch = atan( accelSensorControllX / currentMagSensorZFlipSmooth );// + ofDegToRad(90);
//        roll = atan( accelSensorControllY / currentMagSensorZFlipSmooth );// + ofDegToRad(90); // roll dake okashii
        
        pitch = atan( accelSensorControllX / sqrt( pow( accelSensorControllY, 2 ) + pow( accelSensorControllZ, 2 )) );// + ofDegToRad(90);
        roll = atan( accelSensorControllY / sqrt(pow( accelSensorControllX, 2 ) + pow( accelSensorControllZ, 2 ))  );// + ofDegToRad(90); // roll dake okashii
        //pitch = asin( -( accelSensorControllX / SENSOR_MAG_MAX ) );
        //roll = asin( ( currentMagSensorYFlipSmooth / SENSOR_MAG_MAX ) / cos(pitch));
//
//        yaw = atan( currentMagSensorZFlipSmooth / sqrt( pow( accelSensorControllX, 2 ) + pow( accelSensorControllY, 2 ))  ) ;
//        ofMatrix3x3 dmc = rMatrixFromEulerAngles( roll, pitch, yaw );
//        
//        magDegree = calculate_heading( dmc ,sensorControllX, sensorControllY, sensorControllZ ) * ( 180 / M_PI );
        //magDegree = calculate_heading( pitch, roll ,sensorControllX, sensorControllY, sensorControllZ )
        magDegree = atan2( sensorControllY , sensorControllX ) * ( 180 / M_PI );
        
        localmapPitch = pitch ;
        localmapRoll = roll ;
        ///////////////////////////////////////////////////////
        
        //magDegree = magDegree * 0.15 + magDegreeBefore * 0.85;//calculate_heading
        //if( 160 < magDegree && magDegree < 200 )magDegree = 180;

        float distDegree;
        distDegree = magDegree + 180.0;
        //cout << localmapRoll<< " "<< localmapRoll  << endl;
        localMapAlpha = 255 - abs ( 255 - distDegree * 255.0 / 180.0 );
        
        //magDegreeBefore = magDegree;

    } else {
        centerOfPixelReadX = ofMap( currentReadPosX , -180.0, 180.0, 0.0, 2160.0);
        centerOfPixelReadY = ofMap( currentReadPosY, 90.0, -90.0, 0.0, 1080.0 );
        
        //centerOfPixelReadX =  -ofGetFrameNum()*1.5;
        //centerOfPixelReadY =  320 * sin( ofGetFrameNum()*0.012 ) + 350;

        int pixelReadX = 0;
        int pixelReadY = 0;
        int pixelPointRead = 0;
        
#pragma mark - update Geo Map Texture
        
        if( refImage.bAllocated() && targetTex.bAllocated() ){
            for( int i = 0; i< TARGET_TEXTURE_WIDTH; i++ ){
                pixelReadX = i + centerOfPixelReadX;
                if( pixelReadX < 0) { pixelReadX = pixelReadX + REF_TEXTURE_WIDTH; }; //doughnuts mawarikomi map
                if( pixelReadX > REF_TEXTURE_WIDTH ){ pixelReadX = pixelReadX - REF_TEXTURE_WIDTH; }; //doughnuts mawarikomi map
                
                for(int j = 0; j< TARGET_TEXTURE_HEIGHT; j++){
                    pixelReadY = j + centerOfPixelReadY;
                    if( pixelReadY < 0) { pixelReadY = - pixelReadY; }; //doughnuts mawarikomi map
                    if( pixelReadY >= REF_TEXTURE_HEIGHT ){
                        pixelReadY = REF_TEXTURE_HEIGHT - ( pixelReadY - REF_TEXTURE_HEIGHT ) - 1;
                        if( 0 == i && 0 == j ){
                            changeDivSpeed();
                        }
                    }; //doughnuts mawarikomi map
                        
                    pixelPointRead = pixelReadY * REF_TEXTURE_WIDTH + pixelReadX;
                    
                    //readpixels
                    int r = refPix[ int(pixelPointRead) * 3 + 0 ];
                    int g = refPix[ int(pixelPointRead) * 3 + 1 ];
                    int b = refPix[ int(pixelPointRead) * 3 + 2 ];
                    
                    //copy Pixels
                    targetPix[ ( j * TARGET_TEXTURE_WIDTH +i ) * 3 + 0 ] = r;
                    targetPix[ ( j * TARGET_TEXTURE_WIDTH +i ) * 3 + 1 ] = g;
                    targetPix[ ( j * TARGET_TEXTURE_WIDTH +i ) * 3 + 2 ] = b;
                }
            }
        }
    }
    
    targetTex.loadData(targetPix, TARGET_TEXTURE_WIDTH, TARGET_TEXTURE_HEIGHT, GL_RGB);
    
    
#pragma mark - update Coordinatioin Pint
    
    for(int i =0; i < SCALE_POINT_REPAT_NUM_WIDTH; i++){
        for(int j = 0; j < SCALE_POINT_REPAT_NUM_HEIGHT; j++){
            double nextPointX = (i * 270 );
            double nextPointY = (j * 270 );
            
            nextPointX -= centerOfPixelReadX * 4;
            nextPointY -= centerOfPixelReadY * 3;
    
            if( nextPointX < 1620.0 ){ nextPointX = nextPointX + 1620.0; };
            if( nextPointX > 1620.0 ){ nextPointX = nextPointX - 1620.0; };
            if( nextPointY < -270.0 ){ nextPointY = nextPointY + 1350.0; };
            if( nextPointY > 1080.0 ){ nextPointY = nextPointY - 1350.0; };
            
            crossPoint[j * 6 + i] .x = nextPointX;
            crossPoint[j * 6 + i] .y = nextPointY + 540.0;
        }
    }
#pragma mark update Array
    if(isTouchedDevice){
        for(int i = 1078; i>=1 ; i--){
            grphMagArrayX[i] = grphMagArrayX[i-1];
            grphMagArrayY[i] = grphMagArrayY[i-1];
            grphMagArrayZ[i] = grphMagArrayZ[i-1];
        }
        grphMagArrayX[0] = sensorControllX * 0.1;
        grphMagArrayY[0] = sensorControllY * 0.1;
        grphMagArrayZ[0] = sensorControllZ * 0.1;
    }
    
}
//--------------------------------------------------------------

#pragma mark - calculate Heading

double testApp::calculate_heading(const ofMatrix3x3 &dcm_matrix,double mag_x, double mag_y, double mag_z) {
    double headX;
    double headY;
    double cos_pitch = sqrt(1-(dcm_matrix.c*dcm_matrix.c));
    double heading;
    
    if (cos_pitch == 0.0) {
        return 0;
    }
    //c.x = c
    //c.y = f
    //c.z = i
    
    // Tilt compensated magnetic field X component:
    headX = mag_x*cos_pitch - mag_y*dcm_matrix.f*dcm_matrix.c/cos_pitch - mag_z*dcm_matrix.i*dcm_matrix.c/cos_pitch;
    // Tilt compensated magnetic field Y component:
    headY = mag_y*dcm_matrix.i/cos_pitch - mag_z*dcm_matrix.f/cos_pitch;
    // magnetic heading
    // 6/4/11 - added constrain to keep bad values from ruining DCM Yaw - Jason S.
    //heading = constrain(atan2(-headY,headX), -3.15, 3.15);
    heading = atan2(-headY,headX);
    return ofDegToRad(360) - heading;
}

double testApp::calculate_heading( double pitch, double roll, double mag_x, double mag_y, double mag_z) {
    
    double xh = mag_x * cos(pitch) + mag_z * sin(pitch);
    double yh = mag_x * sin(roll) * sin(pitch) + mag_y * cos(roll) - mag_z * sin(roll) * cos(pitch);
    //double zh = -mag_x * cos(roll) * sin(pitch) + mag_y * sin(roll) + mag_z * cos(roll) * cos(pitch);
    
    float heading = 180 * atan2(yh, xh)/PI;
    if (yh >= 0)
        return heading;
    else
        return (360 + heading);
}

#pragma mark - draw

//--------------------------------------------------------------
void testApp::draw(){
    ofTranslate(HEADER_PIXEL_NUM, 0);
    if(monoColorMode){
        ofBackground(255);
    } else {
        ofBackground(0);
    }
    //ofSetColor(255, 255, 255);
    ofSetColor(0);
    
    //read texture pixels from geomagnetic map and maek scala map
    
#pragma mark - draw Texture Lines
    
    for( int i = 1; i < TARGET_TEXTURE_WIDTH * 0.16 ; i += 1 ){//108
        for( int j = 1; j < TARGET_TEXTURE_HEIGHT * 0.16; j += 1 ){//72
            ofPoint anchorPoint;
            ofPoint powerPoint;
            
            float xPoint = (float)( targetPix[((int)(j * 6 * TARGET_TEXTURE_WIDTH +  i * 6 ) * 3 + 0) ] - 127) ;
            float yPoint = (float)( targetPix[((int)(j * 6 * TARGET_TEXTURE_WIDTH +  i * 6 ) * 3 + 1) ] - 127) ;
            
            //powerPoint dicribe vector of magnetic value.
            powerPoint = ofPoint( xPoint *0.8, yPoint*0.8 );
            
            ofPushMatrix();
            ofTranslate( (i - 4) * 20 , (j - 2) * 20 );
            
            //rotation makes a effect moire.
            ofRotateZ( currentInvadorPosture );
            ofSetLineWidth(0.5);
            if( monoColorMode ){
                ofSetColor( 0 );
            } else {
                ofSetColor(targetPix[((int)(j*6 *TARGET_TEXTURE_WIDTH + i * 6 ) * 3 + 0 ) ],
                           targetPix[((int)(j*6 *TARGET_TEXTURE_WIDTH + i * 6 ) * 3 + 1 ) ],
                           targetPix[((int)(j*6 *TARGET_TEXTURE_WIDTH + i * 6 ) * 3 + 2 ) ]
                           );
            }
            ofLine( 0, powerPoint );
            ofPopMatrix();
        }
    }
#pragma mark - drawScaleLines
    
    drawScaleLines();
    
#pragma mark - device touched mode
    // device touched mode -----------------------------------------------------------------------------------------------
    ofPushMatrix();
    if(isTouchedDevice){
        ofEnableAlphaBlending();
        ofSetColor(0, 0, 0, 205);
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
        ofPushMatrix();
        ofTranslate( 1620 / 2 , ofGetHeight() / 2 );
        //ofRotate( 180, 0, 0, 1);
        //ofPushMatrix();
        
        ofRotate(ofRadToDeg( localmapRoll ), 1,0,0);
        ofRotate( magDegree, 0, 0, 1);
        ofRotate( ofRadToDeg( localmapPitch ), 0,1,0);
        ofSetColor(255, 255, 255, localMapAlpha);
        //ofSetColor(255, 255, 255, 255);
        //localMap.draw(  -1620 / 2 , -ofGetHeight() / 2 );
        localMapTextuer.draw( -1620 / 2 , -ofGetHeight() / 2, 0);
        //ofPopMatrix();
        ofPopMatrix();
        
            ofRotate( 220, 0, 0, 1);
            
        ofPopMatrix();
        
        ofSetColor(255);
        localMap.draw(0,0, 1620, 1080);
        arrowImage.draw(120, 120 );
        
//        ofPushStyle();
//            ofNoFill();
//            ofSetColor(255, 0, 0);
//            for(int i = 1; i < 1080; i++){
//                ofLine( 1600 - grphMagArrayX[i], i , 1600 - grphMagArrayX[i-1], i - 1);
//            }
//            ofSetColor(255, 255, 0);
//            for(int i = 1; i < 1080; i++){
//                ofLine( 1600 - grphMagArrayY[i], i , 1600 - grphMagArrayY[i-1], i - 1);
//            }
//            ofSetColor(0, 255, 255);
//            for(int i = 1; i < 1080; i++){
//                ofLine( 1600 - grphMagArrayZ[i], i , 1600 - grphMagArrayZ[i-1], i - 1);
//            }
//        ofPopStyle();
        
    }
    ofPopMatrix();
    //---------------------------------------------------------------------------------------------------------------------

#pragma mark - draw Rect Mask
    // wite rect mask
    ofPushMatrix();
    ofTranslate(0, 0);
    ofSetColor(0 ,0, 0);
    ofRect(0, 0, -HEADER_PIXEL_NUM, 1080);
    ofPopMatrix();
    ofSetColor(255, 255, 255);
    
    drawConsole();
    
#if DEBUG == 1
    drawDebugConsole();
#endif

}
//--------------------------------------------------------------

void testApp::drawConsole() {
    ofPushMatrix();
    ofPushStyle();
        ofTranslate(-HEADER_PIXEL_NUM,1080);
        ofRotate(-90, 0, 0, 1);
        footerImage.draw(0,0);

    if(monoColorMode){
        ofSetColor(0);
    }

        invadorFont24.drawString("this is HELEVETICA Neue 24pt 0123456789 +_ * ? / ", 0, 2360); // this is dummy, do not commentout.
    if(!isTouchedDevice){    
        invadorFont12.drawString("Latitude : " + ofToString( currentReadPosX ), 540, 1230);
        invadorFont12.drawString("Longtitude : " + ofToString( (currentReadPosY * 3) ), 540, 1260);
    }
    ofPopStyle();
    ofPopMatrix();

}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if('f' == key){
        ofToggleFullscreen();
    }
    
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

//--------------------------------------------------------------
void testApp::drawScaleLines(){
    
//    //right
//    ofSetColor(255, 255, 255);
//    ofSetLineWidth(1.0);
//    ofLine( 1,1 ,1,10 );
//    for(int i =0; i<90; i++){
//        int xPoint;
//        xPoint = i * 18;
//        ofLine( xPoint,0 ,xPoint,10 );
//    }
//    ofLine( 1620,0 ,1620,10 );
//    
//    //top
//    ofLine( 0,0 ,10,0 );
//    for(int i =0; i<61; i++){
//        int yPoint;
//        yPoint = i * 18;
//        ofLine(0,yPoint,10,yPoint);
//    }
//    ofLine( 0, 1079 ,10, 1079 );
//    
//    //lefl
//    ofSetColor(255, 255, 255);
//    ofSetLineWidth(1.0);
//    ofLine( 1, 1080 ,1, 1070 );
//    for(int i =0; i<90; i++){
//        int xPoint;
//        xPoint = i * 18;
//        ofLine( xPoint, 1080 ,xPoint, 1070 );
//    }
//    ofLine( 1620, 1080 ,1620, 1070 );
    
    //follow with world map scale poit crrosses
    ofPushMatrix();
    ofSetLineWidth( 1.0f );
    for(int i =0; i < SCALE_POINT_REPAT_NUM_WIDTH; i++){
        for(int j = 0; j < SCALE_POINT_REPAT_NUM_HEIGHT; j++){
            drawCross( crossPoint[j * 6 + i ], 540 );
        }
    }
    ofPopMatrix();
    
    ofSetColor( 255, 0, 0 );
    //ofLine(ofMap(currentReadPosX, -180.0, 180.0, 0.0, 2160.0) * 4, 0, ofMap(currentReadPosX, -180.0, 180.0, 0.0, 2160.0) * 4, 1080);
    //ofLine(ofMap(currentReadPosX, -180.0, 180.0, 0.0, 2160.0) * 4, 0, ofMap(currentReadPosX, -180.0, 180.0, 0.0, 2160.0) * 4, 1080);
    
    ofLine(0, ofMap(currentReadPosY, -90, 90, 0, 1080) * 3 + 1620, 1620, ofMap(currentReadPosY, -90, 90, 0, 1080) * 3 + 1620);
    ofLine(0, ofMap(currentReadPosY, -90, 90, 0, 1080) * 3 - 1620, 1620, ofMap(currentReadPosY, -90, 90, 0, 1080) * 3 - 1620);
    
    //draw ruler numbers
    
    for(int i = 0 ; i< 24; i++){
        int currentMajor = ofMap(currentReadPosY, -90,90,0,1080) * 3 - ( 270 * i ) - 306;
        rulerNodeY[i].setPosition(1600, currentMajor % 6480, 0 );
        rulerNodeY[i].customDraw( ofToString( rulerRefernceDegreeY[i] ), monoColorMode);
    }
    
    for(int i = 0 ; i< 48; i++){
        int currentMajor = ofMap( currentReadPosX , -180.0, 180.0, 0.0, 2160.0) * 4 - ( 270 * i ) - 253;
        rulerNodeX[i].setPosition( -currentMajor % 12960, 1046, 0 );
        rulerNodeX[i].customDraw( ofToString( rulerRefernceDegreeX[i] ), monoColorMode);
    }
}

//--------------------------------------------------------------

void testApp::setupCsv(){
    ofBackground( 255 );
    csv.loadFile(ofToDataPath("simulation_data_tempadd120818.csv"));
    for (int i = 0;  i < MAX_NUM_OF_LOW; i++ ) {
        string valueStrX;
        string valueStrY;
        string valueStrZ;
        
        string latStr;
        string lonStr;
        string altStr;
        
        valueStrX = csv.getString( i + 1, CSV_COL_NUMBER_GEOMAG );
        valueStrY = csv.getString( i + 1, CSV_COL_NUMBER_GEOMAG + 1 );
        valueStrZ = csv.getString( i + 1, CSV_COL_NUMBER_GEOMAG + 2 );
        
        latStr = csv.getString( i + 1, CSV_COL_NUMBER_GEO_COORDINATES );
        lonStr = csv.getString( i + 1, CSV_COL_NUMBER_GEO_COORDINATES + 1 );
        altStr = csv.getString( i + 1, CSV_COL_NUMBER_GEO_COORDINATES + 2 );
        
        xSimValue[i] = ofToFloat( valueStrX ) * FLOAT_NORMALISE_VALUE;
        ySimValue[i] = ofToFloat( valueStrY ) * FLOAT_NORMALISE_VALUE;
        zSimValue[i] = ofToFloat( valueStrZ ) * FLOAT_NORMALISE_VALUE;
        
        vlat[i] = ofToFloat( latStr );
        vlon[i] = ofToFloat( lonStr );
        valt[i] = ofToFloat( altStr );
    }
}

//--------------------------------------------------------------

void testApp::drawDebugConsole(){
//    ofSetColor(255, 0, 0);
//    ofLine(0, 0, 0, 1080);
//    ofLine(0, 0, 0, 1080);
//    ofLine(1621, 0, 1621, 1080);
//    ofLine(0, 1080, 1620, 1080);
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("framerate ; " + ofToString( ofGetFrameRate() ) ,10,10);
    
    ofDrawBitmapString( ofToString(centerOfPixelReadX) ,10, 30);
    ofDrawBitmapString( ofToString(centerOfPixelReadY) ,10, 40);
    ofDrawBitmapString( ofToString( STO.touch ), 10,50);
    ofDrawBitmapString( "xValue : " + ofToString( sensorControllX ), 10,70);
    ofDrawBitmapString( "yValue : " + ofToString( sensorControllY ), 10,90);
    ofDrawBitmapString( "zValue : " + ofToString( sensorControllZ ), 10,110);
    
    ofDrawBitmapString( "t : " + ofToString( simReadTime ), 10,120);
    
    ofDrawBitmapString( "xSimValue : " + ofToString( xSimValue[simReadTime] ), 10,130);
    ofDrawBitmapString( "ySimValue : " + ofToString( ySimValue[simReadTime] ), 10,140);
    ofDrawBitmapString( "zSimValue : " + ofToString( zSimValue[simReadTime] ), 10,150);
    
    ofDrawBitmapString( "SimLat : " + ofToString( vlat[simReadTime] ), 10,170);
    ofDrawBitmapString( "SimLon : " + ofToString( vlon[simReadTime] ), 10,180);
    ofDrawBitmapString( "SimAlt : " + ofToString( valt[simReadTime] ), 10,190);
    
    ofDrawBitmapString( "headding degree : " + ofToString( magDegree ), 10,210);
    
    ofDrawBitmapString( "accelSensorValueX : " + ofToString( accelSensorControllX ), 10,230);
    ofDrawBitmapString( "accelSensorValueY : " + ofToString( accelSensorControllY ), 10,240);
    ofDrawBitmapString( "accelSensorValueZ : " + ofToString( accelSensorControllZ ), 10,250);
    
    ofDrawBitmapString( "sim Time Count : " + ofToString( localMapAlpha ), 10,270);
}


//--------------------------------------------------------------
void testApp::drawCross(ofPoint point, int crossLnegth){
//    if(monoColorMode){
        ofSetColor(0, 0, 0);
//    } else {
//        ofSetColor(255, 255, 255);
//    }
    ofLine( point.x, -1 * crossLnegth + point.y - 540, point.x, crossLnegth + point.y - 540);
    ofLine( -1 * crossLnegth + point.x, point.y - 540, crossLnegth + point.x, point.y - 540);
}


//--------------------------------------------------------------
ofMatrix3x3 testApp:: rMatrixFromEulerAngles(double Roll, double Pitch, double Yaw) {
    ofMatrix3x3 result;
    
    double sr = sin(Roll), cr = cos(Roll);
    double sp = sin(Pitch), cp = cos(Pitch);
    double sy = sin(Yaw), cy = cos(Yaw);
    
    result.a = cp * cy;
    result.b = - cp * sy;
    result.c = sp;
    
    result.d = sr * sp * cy + cr * sy;
    result.e = sr * sp * sy - cr * cy;
    result.f = - sr * cp;
    
    result.g = cr * sp * cy - sr * sy;
    result.h = cr * sp * sy + sr * cy;
    result.i = cr * cp;
    
    return result;
}

//--------------------------------------------------------------
double testApp::constrain(double x, double a, double b) {
    if(x < a) {
        return a;
    }
    else if(b < x) {
        return b;
    }
    else
        return x;
}

//--------------------------------------------------------------

void testApp::changeDivSpeed(){
    int randomInt;
    randomInt = int(ofRandom(4));
    switch (randomInt) {
        case 0:
            smootDivNum = 14;
            break;
        case 1:
            smootDivNum = 10;
            break;
        case 2:
            smootDivNum = 5;
            break;
        default:
            break;
    }
}

void testApp::resetGraphArray(){
    for(int i = 0; i< 1080;i++){
        grphMagArrayX[i] = 0;
        grphMagArrayY[i] = 0;
        grphMagArrayZ[i] = 0;
    }
}
