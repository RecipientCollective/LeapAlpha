//
//  Sketch_1.cpp
//  LeapAlpha
//
//  Created by Alessandro Inguglia on 07/06/15.
//
//
#include "Sketch_1.h"

void Sketch_1::setup()
{
    // Add Group of Ropes
    ropeVec = new Rope*[Rgroup_1];
    int ropesLenght = 200;
    for(int i=0; i<Rgroup_1; i++)
    {
        //cout<<"addrope: "<<i<<endl;
        //int ropesLenght = ofRandom(200)+100;
        addRope(ropeVec,new Rope(ofPoint(0,0,0),ofPoint(0,0,0),50,ropesLenght,ofPoint(0,0,0),0.0f,true),i,Rgroup_1);
    }
    
    //setup on separate sketch
    for (int j=0;j<Rgroup_1;j++)
    {
        
        ropeVec[j]->a = ofPoint(0,0,0);
        ropeVec[j]->b = ofPoint(ropeVec[j]->WireLength,0,0);
    }
}

void Sketch_1::draw()
{
    ofSetColor(255,255,255);
    //Draw Ropes
    ofSetLineWidth(0.9f);
    for (int j=0;j<Rgroup_1;j++)
    {
        ropeVec[j]->draw();
    }

}

void Sketch_1::update(vector <ofxLeapMotionSimpleHand> simpleHands)
{
    //cout<<<<endl;
    //update
    for (int j=0;j<Rgroup_1;j++)
    {
        //cout<<"update: "<<j<<endl;
        ropeVec[j]->update();
    }
    
    //1 Hand
    if (simpleHands.size()==1)
    {
        for (int j=0;j<Rgroup_1;j++)
        {
            ropeVec[j]->ps[0].mass = 0;
            ropeVec[j]->ps[ropeVec[j]->H-1].mass = 0;
            //ropeVec[j]->smoothMoveA = 1.0f;
            //ropeVec[j]->smoothMoveB = 1.0f;
            
            if (simpleHands[0].handPos.distance(ropeVec[j]->a)<1000&&simpleHands[0].isLeft)
            {
                ropeVec[j]->a.interpolate(simpleHands[0].handPos,0.2);
            }
            else if (simpleHands[0].handPos.distance(ropeVec[j]->b)<1000&&!simpleHands[0].isLeft)
            {
                ropeVec[j]->b.interpolate(simpleHands[0].handPos,0.2);
            }
        }
    }
    //2 Hands
    else if (simpleHands.size()==2)
    {
        handsDistance = simpleHands[0].handPos.distance(simpleHands[1].handPos);
        for (int j=0;j<Rgroup_1;j++)
        {
            ropeVec[j]->ps[0].mass = 0;
            ropeVec[j]->ps[ropeVec[j]->H-1].mass = 0;
            //ropeVec[j]->smoothMoveA = 1.0f;
            //ropeVec[j]->smoothMoveB = 1.0f;
            if(simpleHands[0].handPos.distance(ropeVec[j]->a)<1000&&simpleHands[1].handPos.distance(ropeVec[j]->b)<1000&&simpleHands[0].isLeft)
            {
                for (int k=1;k<ropeVec[j]->H-1;k++)
                {
                    ropeVec[j]->a.interpolate(simpleHands[0].handPos,0.2);
                    ropeVec[j]->b.interpolate(simpleHands[1].handPos,0.2);
                    ofVec3f forceVec = ofVec3f(0,handsDistance/800*sin(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()),-handsDistance/800*cos(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()));
                    //forceVec.rotateRad((*lRoll+*rRoll)/2,(*lPitch+*rPitch)/2,(*rYaw+*lYaw)/2);
                    forceVec.rotateRad((simpleHands[0].roll+simpleHands[1].roll)/2,(simpleHands[0].pitch+simpleHands[1].pitch)/2,(simpleHands[0].yaw+simpleHands[1].yaw)/2);
                    ropeVec[j]->ps[k].addForce(forceVec);
                }
            }
            if(simpleHands[1].handPos.distance(ropeVec[j]->a)<1000&&simpleHands[0].handPos.distance(ropeVec[j]->b)<1000&&!simpleHands[0].isLeft)
            {
                for (int k=1;k<ropeVec[j]->H-1;k++)
                {
                    ropeVec[j]->a.interpolate(simpleHands[1].handPos,0.2);
                    ropeVec[j]->b.interpolate(simpleHands[0].handPos,0.2);
                    ofVec3f forceVec = ofVec3f(0,handsDistance/800*sin(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()),-handsDistance/800*cos(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()));
                    forceVec.rotateRad((*lRoll+*rRoll)/2,(*lPitch+*rPitch)/2,(*rYaw+*lYaw)/2);
                    ropeVec[j]->ps[k].addForce(forceVec);
                }
            }
        }
    }
}
//    ADDROPE Method
//    --------------------------------------------------------------
int Sketch_1::addRope(Rope** ropesVector,Rope *nextRope,int startString,int endString){
    int i = startString%endString;
    ropesVector[i] = nextRope;
}