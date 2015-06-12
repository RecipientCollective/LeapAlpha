//
//  Sketch_2.h
//  LeapAlpha
//
//  Created by Alessandro Inguglia on 07/06/15.
//
//

#ifndef __LeapAlpha__Sketch_2__
#define __LeapAlpha__Sketch_2__


#include "ofMain.h"
#include "Rope.h"
#include "ofxLeapMotion2.h"
#include "ofxUI.h"
#include "ofEvents.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 8000

class Sketch_2 {
public:
    
    // main
    void SetupListeners();
    void SketchSetup();
    void update(vector<ofxLeapMotionSimpleHand> LeapHands);//
    void draw();
    void exit(ofEventArgs &arg);
    void keyPress(ofKeyEventArgs &data);
    void windowResize(ofResizeEventArgs& data);
    
    void SketchQuit();
   
    //Leap Hands
    float handsDistance = 0.0f;
    ofPoint *leftHandPos = new ofPoint (0,0,0);
    ofPoint *leftHandNormal= new ofPoint (0,0,0);
    ofPoint *leftHandVel= new ofPoint (0,0,0);
    ofPoint *rightHandPos= new ofPoint (0,0,0);
    ofPoint *rightHandNormal= new ofPoint (0,0,0);
    ofPoint *rightHandVel= new ofPoint (0,0,0);
    float *lPitch = new float (0.0f);
    float *lRoll = new float (0.0f);
    float *lYaw = new float (0.0f);
    float *rPitch = new float (0.0f);
    float *rRoll = new float (0.0f);
    float *rYaw = new float  (0.0f);
    
    ofxOscSender sender;
    ofxOscMessage m;
    
    //ROPES
    Rope** ropeVec;
    int Rgroup_1 = 50 ;
    //ADDROPE METHOD
    int addRope(Rope** ropesArray,Rope *nextRope,int startString,int endString);
    
    //GUI ROPES
    ofxUISuperCanvas *guiRopes;
    // GUI EVENTS
    void guiEvent(ofxUIEventArgs &e);
    float GUIForceX;
    float GUIForceY;
    float GUIForceZ;
    ofPoint mouseisHere;
    float bSmooth;
    float p_mass;
    float p_drag;
    float sp_k;
    bool VisCtrlPoints = 1;
    float rotationAccel;
    float rotationAmp;
    
private:
     bool currentSketch=false;
};
#endif