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

class Sketch_4
{
public:
    void SetupListeners();
    void SketchSetup();
    void update(vector<ofxLeapMotionSimpleHand> LeapHands);//
    void draw();
    void exit(ofEventArgs &arg);
    void keyPress(ofKeyEventArgs &data);
    void windowResize(ofResizeEventArgs& data);
    void SketchQuit();
    
    ofxUISuperCanvas *guiSwarm;
    void guiEvent(ofxUIEventArgs &e);

private:
    bool currentSketch = false;
};


#endif /* defined(__LeapAlpha__Sketch_4__) */
