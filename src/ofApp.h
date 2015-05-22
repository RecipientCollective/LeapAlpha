#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxLeapMotion2.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
        void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofxLeapMotion leap;
        vector <int> fingersFound;
        ofEasyCam cam;
    
        bool bShowGui;
        bool bShowHelp;
        ofxPanel gui;
        ofxLabel guiTitle;
        ofxColorSlider color;
        ofxToggle debugDraw;
        ofxToggle bShowOrigin;
        ofxToggle bShowGridYZ;
        ofxToggle bShowGridXZ;
        ofxToggle bShowGridXY;
        ofxToggle bShowGridLabels;
        ofxToggle bMirrorImage;
        ofxLabel camPositionLabel;
        ofxToggle bSetMapping;
        ofxFloatSlider leapXrange;
        ofxFloatSlider leapZrange;
        ofxFloatSlider leapYmin;
        ofxFloatSlider leapYmax;
    
        float outputZrange;
    
        vector <ofxLeapMotionSimpleHand> simpleHands;
    
        ofIcoSpherePrimitive icoSphere;
    
        // GUI EVENTS
        void setMappingChanged(bool & bmapping);
        void leapXRangeChanged(float & range);
        void leapZRangeChanged(float & range);
        void leapYMinChanged(float & min);
        void leapYMaxChanged(float & max);
    
        // CUSTOM METHODS
        void drawInteractionArea();
        void setLeapMapping();
};
