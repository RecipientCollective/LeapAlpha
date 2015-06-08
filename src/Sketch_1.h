//
//  Sketch_1.h
//  LeapAlpha
//
//  Created by Alessandro Inguglia on 07/06/15.
//
//

#ifndef __LeapAlpha__Sketch_1__
#define __LeapAlpha__Sketch_1__
#endif

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "Rope.h"

class Sketch_1 {
public:
    
//    Sketch_1::Sketch_1{
//    
//    
//    
//    }
    
    // main
    void setup();
    void update(vector<ofxLeapMotionSimpleHand> simpleHands);
    void draw();
    void exit();
    
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

    //ROPES
    Rope** ropeVec;
    int Rgroup_1 = 1 ;
    //ADDROPE METHOD
    int addRope(Rope** ropesArray,Rope *nextRope,int startString,int endString);
};