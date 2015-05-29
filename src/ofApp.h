#pragma once
#include "ofMain.h"
#include "ofxUI.h"
#include "ofxOsc.h"
#include "ofxLeapMotion2.h"
#include "Grid.h"

class ofApp : public ofBaseApp{

	public:
    
    //BASIC METHODS
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
    
    //LEAP
        ofxLeapMotion leap;
        vector <int> fingersFound;
        vector <ofxLeapMotionSimpleHand> simpleHands;
        vector <Hand> myHands; //proper Hands
        float handsDistance;
        Hand LeftHand;
        Hand RightHand;
    
    //CAMERA
        ofEasyCam cam;
    
    
    //GUI
        ofxUISuperCanvas *guiRopes;
        ofxUISuperCanvas *guiLeap;
        bool bShowGui;
        bool bShowHelp;

        string guiTitle;
        bool debugDraw;
        bool bShowOrigin;
        bool bShowGridYZ;
        bool bShowGridXZ;
        bool bShowGridXY;
        bool bShowGridLabels;
        bool bMirrorImage;
        bool bSetMapping;
        float leapXrange;
        float leapZrange;
        float leapYmin;
        float leapYmax;
        float outputZrange;
        bool bShowWorkingArea;
            
    // GUI EVENTS
        void guiEvent(ofxUIEventArgs &e);

    // 3D objects (grid and box)
        grid nodeGrid;
        ofBoxPrimitive box;
    
    // MORE CUSTOM METHODS
        void drawInteractionArea();
        void setLeapMapping();
        void guiSetup();
        void LeapUpdate();
};
