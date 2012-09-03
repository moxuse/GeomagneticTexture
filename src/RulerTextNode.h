//
//  RulerTextNode.h
//  GeomagneticTexture
//
//  Created by Koichiro Mori on 2012/09/03.
//
//

#ifndef GeomagneticTexture_RulerTextNode_h
#define GeomagneticTexture_RulerTextNode_h

#import "ofMain.h"

class RulerTextNode : public ofNode {
    public:

        void setup() {
            invadorFont16.loadFont("font/AvantGarde-Medium.otf", 16);
        }
        
        void update() {
        
        }

        void customDraw(string text , bool monoColorMode) {
            ofPushMatrix();
                if( monoColorMode ){
                    ofSetColor( 0 );
                } else {
                    ofSetColor( 255 );
                }
                invadorFont16.drawString( text, getX(), getY() );
            ofPopMatrix();
        }
        
    private:
        ofTrueTypeFont invadorFont16;
};

#endif
