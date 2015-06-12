//
//  Sketch_1.cpp
//  LeapAlpha
//
//  Created by Alessandro Inguglia on 07/06/15.
//
//
#include "Sketch_1.h"

void Sketch_1::SetupListeners()
{
    ofAddListener(ofEvents().windowResized, this, &Sketch_1::windowResize);
    ofAddListener(ofEvents().keyPressed, this, &Sketch_1::keyPress);
    ofAddListener(ofEvents().exit, this, &Sketch_1::exit);
}

void Sketch_1::SketchSetup()
{
    //guisetup
    guiRopes = new ofxUISuperCanvas("Ropes_Sketch_1");

    guiRopes->setPosition(ofGetWidth()-210,0);
    guiRopes->addSpacer();
    guiRopes->addLabel("Forces", OFX_UI_FONT_MEDIUM);
    guiRopes->addSpacer();
    guiRopes->addSlider("X", -1.0f, 1.0f, GUIForceX);
    guiRopes->addSlider("Y", -1.0f, 1.0f, GUIForceY);
    guiRopes->addSlider("Z", -1.0f, 1.0f, GUIForceZ);
    guiRopes->addSpacer();
    vector<string> PrimNames;
    PrimNames.push_back("Points");
    PrimNames.push_back("Lines");
    guiRopes->addRadio("Primitives", PrimNames, 1);
    vector<string> PolyMesh;
    PolyMesh.push_back("Mesh");
    PolyMesh.push_back("PolyLine");
    guiRopes->addRadio("PolyLine-Mesh", PolyMesh, 1);
    guiRopes->addSpacer();
    guiRopes->addSlider("B_smoothing", .001f, 1.0f, &bSmooth);
    guiRopes->addSpacer();
    guiRopes->addSlider("Particle_Mass", .001f, 10.0f, &p_mass);
    guiRopes->addSpacer();
    guiRopes->addSlider("Particle_Drag", 0.0f, 10.0f, &p_drag);
    guiRopes->addSpacer();
    guiRopes->addSlider("Spring_k", 0.0f, 10.0f, &sp_k);
    guiRopes->addSpacer();
    guiRopes->addToggle("Ctrl_Points", &VisCtrlPoints);
    guiRopes->addSpacer();
    guiRopes->addSlider("rotation_accel", 0.0f, 10.0f, &rotationAccel);
    guiRopes->addSpacer();
    guiRopes->addSlider("rotation_amp", 0.0f, 5.0f, &rotationAmp);
    guiRopes->addSpacer();
    guiRopes->autoSizeToFitWidgets();
    ofAddListener(Sketch_1::guiRopes->newGUIEvent,this,&Sketch_1::guiEvent);//event listener
    VisCtrlPoints = 1;
    p_mass = 0.97f;
    p_drag = 0.96f;
    sp_k = 0.2f;
    bSmooth = 1;
    
    // Add Group of Ropes
    ropeVec = new Rope*[Rgroup_1];
    float stretch =0.3f;
    int length = 200;
    
    for(int i=0; i<Rgroup_1; i++)
    {
        //int ropesLenght = ofRandom(200)+100;
        addRope(ropeVec,new Rope(ofPoint(0,0,0),ofPoint(0,length,0),20,stretch,ofPoint(0,0,0),0.0f,true),i,Rgroup_1);
    }
    
//    for (int j=0;j<Rgroup_1;j++)
//    {
//        ropeVec[j]->a = ofPoint(0,0,0);
//        ropeVec[j]->b = ofPoint(length,0,0);
//    }
}

void Sketch_1::draw()
{
    ofSetColor(255,255,255);
    //Draw Ropes
    ofSetLineWidth(1);
    for (int j=0;j<Rgroup_1;j++)
    {
        ropeVec[j]->draw();
    }
}

void Sketch_1::update(vector<ofxLeapMotionSimpleHand> LeapHands)//vector<ofxLeapMotionSimpleHand> LeapHands
{
    ofVec3f Znormal(0, 0, 1);
    ofVec3f Xnormal(1, 0, 0);
    ofVec3f Ynormal(1, 0, 1);
    
    //update ropes
    for (int j=0;j<Rgroup_1;j++)
    {
        ropeVec[j]->update();
    }
    
    //1 Hand
    if (LeapHands.size()==1)
    {
        ofQuaternion qrL (ofRadToDeg(LeapHands[0].roll), Znormal); // quat roll.
        ofQuaternion qpL (ofRadToDeg(LeapHands[0].pitch), Xnormal); // quat pitch.
        ofQuaternion qhL (ofRadToDeg(LeapHands[0].yaw), Ynormal); // quat heading or yaw.
        ofQuaternion qtL (ofRadToDeg(LeapHands[0].roll), Znormal); // quat roll.
        
        qtL = qrL*qpL*qhL;
        
        ofVec3f qAxisL;
        float qangleL;
        
        qtL.getRotate(qangleL,qAxisL);

        for (int j=0;j<Rgroup_1;j++)
        {
            ropeVec[j]->ps[0].mass = 15000;
            ropeVec[j]->ps[ropeVec[j]->H-1].mass = 15000;
            ofVec3f forceVec = ofVec3f(0,sin(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()),-cos(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()));
            //ropeVec[j]->smoothMoveA = 1.0f;
            //ropeVec[j]->smoothMoveB = 1.0f;
           
            forceVec.rotate(qangleL,qAxisL);
            
//            ropeVec[j]->ForceX = -forceVec.x;
//            ropeVec[j]->ForceY = -forceVec.y;
//            ropeVec[j]->ForceZ = -forceVec.z;
            
//            ropeVec[j]->ForceX = LeapHands[0].handNormal.x;
//            ropeVec[j]->ForceY = LeapHands[0].handNormal.y;
//            ropeVec[j]->ForceZ = LeapHands[0].handNormal.z;
            
            if (LeapHands[0].isLeft)//&&LeapHands[0].handPos.distance(ropeVec[j]->a)<1000
            {
                
                ropeVec[j]->a.interpolate(LeapHands[0].handPos,0.2);
                for (int k=0;k<=ropeVec[j]->H/2;k++)
                {
                    ropeVec[j]->ps[k].addForce(forceVec.rotate(qangleL,qAxisL));//
                }
            }
            else if (!LeapHands[0].isLeft)//LeapHands[0].handPos.distance(ropeVec[j]->b)<1000&&
            {
                ropeVec[j]->b.interpolate(LeapHands[0].handPos,0.2);
                for (int k=ropeVec[j]->H/2;k>=0;k--)
                {
                    ropeVec[j]->ps[k].addForce(forceVec.rotate(qangleL,qAxisL));//
                }
            }
        }
    }
    //2 Hands
    else if (LeapHands.size()==2)
    {
        
        
        handsDistance = LeapHands[0].handPos.distance(LeapHands[1].handPos);
        //cout<<"distance: "<<handsDistance<<endl;
        
        //Declare the normalized XYZ axis as vector variables
        ofVec3f Znormal(0, 0, 1);
        ofVec3f Xnormal(1, 0, 0);
        ofVec3f Ynormal(1, 0, 1);
        ofQuaternion qrL (ofRadToDeg(LeapHands[0].roll), Znormal); // quat roll.
        ofQuaternion qpL (ofRadToDeg(LeapHands[0].pitch), Xnormal); // quat pitch.
        ofQuaternion qhL (ofRadToDeg(LeapHands[0].yaw), Ynormal); // quat heading or yaw.
        ofQuaternion qrR (ofRadToDeg(LeapHands[0].roll), Znormal); // quat roll.
        ofQuaternion qpR (ofRadToDeg(LeapHands[0].pitch), Xnormal); // quat pitch.
        ofQuaternion qhR (ofRadToDeg(LeapHands[0].yaw), Ynormal); // quat heading or yaw.
        ofQuaternion qtL;     // quat total.
        ofQuaternion qtR;     // quat total.
        
        qtL = qrL*qpL*qhL;
        qtR = qrR*qpR*qhR;
        
        ofVec3f qAxisL,qAxisR;
        float qangleL,qangleR;
        
        qtL.getRotate(qangleL,qAxisL);
        qtR.getRotate(qangleR,qAxisR);
    
        for (int j=0;j<Rgroup_1;j++)
        {
            ropeVec[j]->ps[0].mass = 15000;
            ropeVec[j]->ps[ropeVec[j]->H-1].mass = 15000;
            ropeVec[j]->ForceX = 0;
            ropeVec[j]->ForceY = 0;
            ropeVec[j]->ForceZ = 0;
            
            //ropeVec[j]->smoothMoveA = 1.0f;
            //ropeVec[j]->smoothMoveB = 1.0f;
            
            if(LeapHands[0].handPos.distance(ropeVec[j]->a)&&LeapHands[1].handPos.distance(ropeVec[j]->b)&&LeapHands[0].isLeft)
            {
                for (int k=1;k<ropeVec[j]->H-1;k++)
                {
                    ropeVec[j]->a=LeapHands[0].handPos;
                    ropeVec[j]->b=LeapHands[1].handPos;
                    ofVec3f forceVec = ofVec3f(0,sin(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()*0.3),-cos(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()*0.3));

                    ofVec3f forceVecL = forceVec.rotate(qangleL, qAxisL);
                    ofVec3f forceVecR = forceVec.rotate(qangleR, qAxisR);
                    
                    if(k<=ropeVec[j]->H/2)
                    {
                        ropeVec[j]->ps[k].addForce(-forceVecL);
                    }
                    else
                    {
                        
                        ropeVec[j]->ps[k].addForce(-forceVecR);
                    }
                    
                }
            }
            if(LeapHands[1].handPos.distance(ropeVec[j]->a)&&LeapHands[0].handPos.distance(ropeVec[j]->b)&&!LeapHands[0].isLeft)
            {
                for (int k=1;k<ropeVec[j]->H-1;k++)
                {
                    ropeVec[j]->a.interpolate(LeapHands[1].handPos,0.2);
                    ropeVec[j]->b.interpolate(LeapHands[0].handPos,0.2);
                    ofVec3f forceVec = ofVec3f(0,sin(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()*0.3),-cos(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()*0.3));
                    
                    ofVec3f forceVecL = forceVec.rotate(qangleL, qAxisL);
                    ofVec3f forceVecR = forceVec.rotate(qangleR, qAxisR);

                    if(k<=ropeVec[j]->H/2)
                    {
                        ropeVec[j]->ps[k].addForce(-forceVecR);
                    }
                    else
                    {
                        ropeVec[j]->ps[k].addForce(-forceVecL);
                    }
                }
            }
        }
    }
    else if(LeapHands.size()==0)
    {
        
        for (int j=0;j<Rgroup_1;j++)
        {
            ropeVec[j]->ForceX = 0;
            ropeVec[j]->ForceY = 0;
            ropeVec[j]->ForceZ = 0;
            ropeVec[j]->smoothMoveA = 0.1f;
            ropeVec[j]->smoothMoveB = 0.1f;
            ropeVec[j]->a=ofPoint(-100,0,0);
            ropeVec[j]->b=ofPoint(100,0,ropeVec[j]->getWireLength());
            
            ofVec3f forceVec = ofVec3f(2*sin(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()*0.3),-2*cos(M_TWO_PI*j/Rgroup_1*ofGetElapsedTimef()*0.3),0);
            for (int k=0;k<ropeVec[j]->H;k++)
            {
                if (k<=ropeVec[j]->H/2)
                {
                    ropeVec[j]->ps[k].addForce(forceVec);
                }
                else
                {
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
void Sketch_1::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
    if(name == "X")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        for (int k = 0; k<Rgroup_1; k++)
        {
            ropeVec[k]->ForceX = slider->getScaledValue();
        }
    }
    else if(name == "Y")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        for (int k = 0; k<Rgroup_1; k++)
        {
            ropeVec[k]->ForceY = slider->getScaledValue();
        }
    }
    else if(name == "Z")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        for (int k = 0; k<Rgroup_1; k++)
        {
            ropeVec[k]->ForceZ = slider->getScaledValue();
        }
    }
    else if (name == "Primitives")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        if (radio->getValue() == 0)
        {
            cout<<"Primitive Points"<<endl;
            for (int k = 0; k<Rgroup_1; k++)
            {
                ropeVec[k]->ropeMesh.setMode(OF_PRIMITIVE_POINTS);
            }
        }
        else if (radio->getValue() == 1)
        {
            for (int k = 0; k<Rgroup_1; k++)
            {
                ropeVec[k]->ropeMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
            }
        }
    }
    else if (name == "PolyLine-Mesh")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        if (radio->getValue() == 0)
        {
            for (int k = 0; k<Rgroup_1; k++) {
                ropeVec[k]->meshRender = true;
            }
        }
        else if (radio->getValue() == 1)
        {
            for (int k = 0; k<Rgroup_1; k++)
            {
                ropeVec[k]->meshRender = false;
            }
        }
    }
    else if (name == "B_smoothing")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        for (int i=0;i < Rgroup_1; i++)
        {
            ropeVec[i]->smoothMoveA = slider->getValue();
            ropeVec[i]->smoothMoveB = slider->getValue();
        }
    }
    else if (name == "Particle_Mass")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        for (int i=0;i < Rgroup_1; i++)
        {
            for (int k = 1; k<ropeVec[i]->ps.size()-1;k++)
                ropeVec[i]->ps[k].mass = slider->getValue();
        }
    }
    else if (name == "Particle_Drag")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        for (int i=0;i < Rgroup_1; i++)
        {
            for (int k =1; k<ropeVec[i]->ps.size()-1;k++)
                ropeVec[i]->ps[k].drag = slider->getValue();
        }
    }
    else if (name == "Spring_k")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        for (int i = 0;i < Rgroup_1; i++)
        {
            for (int k=0; k<ropeVec[i]->sp.size();k++)
                ropeVec[i]->sp[k].k = slider->getValue();
        }
    }
    else if (name == "Ctrl_Points")
    {
        for (int  i=0;i<Rgroup_1;i++)
        {
            ropeVec[i]->ropeMesh.clear();
            ropeVec[i]->CtrlPointsVisible = VisCtrlPoints;
            ropeVec[i]->meshSetup();
        }
    }
}
void Sketch_1::windowResize(ofResizeEventArgs& data)
{
    if(guiRopes)
    {
        guiRopes->setPosition(ofGetWidth()-210,0);
    }
}
void Sketch_1::keyPress(ofKeyEventArgs &data)
{
    int pressedKey = data.key;
    if(pressedKey>=49&&pressedKey<57)
    {
        switch (pressedKey)
        {
            case '1':
                if(!currentSketch)
                {
                    //Re setup gui and everything
                    SketchSetup();
                }
                currentSketch = 1;
                break;
            default:
                if (data.key != '1'&&currentSketch)
                {
                    SketchQuit();
                    currentSketch=0;
                }
                break;
        }
    }
    //adjust gui canvas pos
    if (guiRopes)
    {
        guiRopes->setPosition(ofGetWidth()-210,0);
    }
}
void Sketch_1::SketchQuit ()
{
    guiRopes->clearWidgets();
    guiRopes->clearEmbeddedWidgets();
    guiRopes->disable();
    ofRemoveListener(Sketch_1::guiRopes->newGUIEvent,this,&Sketch_1::guiEvent);
    
    for(int i=Rgroup_1-1;i==0;i++)
    {
        //ropeVec[i] = NULL;
    }
}

void Sketch_1::exit(ofEventArgs &arg)
{
    ofRemoveListener(ofEvents().windowResized, this, &Sketch_1::windowResize);
    ofRemoveListener(ofEvents().keyPressed, this, &Sketch_1::keyPress);
    ofRemoveListener(ofEvents().exit, this, &Sketch_1::exit);
}
