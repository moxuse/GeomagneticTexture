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
            invadorFont8.loadFont("font/AvantGarde-Medium.otf", 10);
        }
        
        void update() {
            
        }

        void customDraw(string text , bool monoColorMode) {
            
            if( getGlobalPosition().x < 0 ){setPosition( getGlobalPosition().x + 12960 , getY(), getZ());};
            if( getGlobalPosition().x > 12960 ){ setPosition (getGlobalPosition().x -12960, getY(), getZ() ); };
            
            if( getGlobalPosition().y < 0 ){setPosition( getX(), getGlobalPosition().y + 6480 , getZ());};
            if( getGlobalPosition().y > 6480 ){ setPosition (getX(), getGlobalPosition().y -6480, getZ() ); };
            
            ofPushMatrix();
                ofTranslate(getX() + 15, getY() + 25);
                ofRotate(-90, 0, 0, 1);
                if( monoColorMode ){
                    ofSetColor( 0 );
                } else {
                    ofSetColor( 255 );
                }
                invadorFont8.drawString( text, 0, 0 );
            ofPopMatrix();
        }
        
    private:
        ofTrueTypeFont invadorFont8;
    
};

#endif
