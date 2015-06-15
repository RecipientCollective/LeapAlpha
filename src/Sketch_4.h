//
//  Sketch_4.h
//  LeapAlpha
//
//  Created by tucano on 15/06/15.
//
//

#ifndef __LeapAlpha__Sketch_4__
#define __LeapAlpha__Sketch_4__

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "ofxUI.h"
#include "ofEvents.h"
#include "Swarm.h"

#define FISH_IMAGE "Fish.png"
#define START_COUNT 400
#define MAX_DISTANCE 400.0f
#define MAXSEEKAREA 400.0f
#define ERASE_RADIUS 30.0f
#define HAND_NORMAL_LIMIT 0.8f

class Sketch_4
{
public:
    void SetupListeners();
    void SketchSetup();
    void update(vector<ofxLeapMotionSimpleHand> LeapHands, ofCamera cam);//
    void draw();
    void exit(ofEventArgs &arg);
    void keyPress(ofKeyEventArgs &data);
    void windowResize(ofResizeEventArgs& data);
    void SketchQuit();
    
    // MOUSE EVENTS
    void mouseDragged(ofMouseEventArgs &data);
    void mousePressed(ofMouseEventArgs &data);
    void mouseReleased(ofMouseEventArgs &data);
    
    ofxUISuperCanvas *guiSwarm;
    void guiEvent(ofxUIEventArgs &e);

    // SWARM
    ofImage texture;
    Swarm swarm;
    int startCount;
    bool bMouseSwarmControl;
    bool bShowControlPoints;
    ofPoint avoidPoint;
    bool avoid;
    ofPoint seekPoint;
    bool seek;
    float seekArea;
    float maxDistance;
    float avoidZthreshold;
    ofVec3f viewPortPoint;
    
private:
    bool currentSketch = false;
};


#endif /* defined(__LeapAlpha__Sketch_4__) */
