//
//  Sketch_1.cpp
//  LeapAlpha
//
//  Created by Alessandro Inguglia on 07/06/15.
//
//
#include "Sketch_2.h"

void Sketch_2::SetupListeners()
{
    ofAddListener(ofEvents().windowResized, this, &Sketch_2::windowResize);
    ofAddListener(ofEvents().keyPressed, this, &Sketch_2::keyPress);
    ofAddListener(ofEvents().exit, this, &Sketch_2::exit);
}

void Sketch_2::SketchSetup()
{
    sender.setup(HOST, PORT);
    //guisetup
    guiRopes = new ofxUISuperCanvas("Ropes_Sketch_2");
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
    ofAddListener(Sketch_2::guiRopes->newGUIEvent,this,&Sketch_2::guiEvent);//event listener
    VisCtrlPoints = 1;
    p_mass = 7.0f;
    p_drag = 0.96f;
    sp_k = 4.5f;
    bSmooth = 1;
    
    // Add Group of Ropes
    ropeVec = new Rope*[Rgroup_1];
    for(int i=0; i<Rgroup_1; i++)
    {
        ofPoint a = ofPoint(ofGetWidth()/Rgroup_1*i-ofGetWidth()/2,ofGetHeight()/2,0);
        ofPoint b = ofPoint(ofGetWidth()/Rgroup_1*i-ofGetWidth()/2,-ofGetHeight()/2,0);
        float stretch = ofMap(i, 0, Rgroup_1-1, 0.03, 1); // 0.05f*i;
        cout<<stretch<<endl;
        int hDist = i;//(Rgroup_1-i)/Rgroup_1;
        addRope(ropeVec,new Rope(a,b,20,stretch,ofPoint(0,0,0),0.0f,true),i,Rgroup_1);
        //addRope(ropeVec,new Rope(a,b,20,hDist,ofPoint(0,0,0),0.0f,true),i,Rgroup_1);
        //addRope(ropeVec,new Rope(a,b,hDist,ofPoint(0,0,0),0.0f,true),i,Rgroup_1);
    }
}

void Sketch_2::draw()
{
    ofSetColor(255,255,255);
    //Draw Ropes
    //ofSetLineWidth(1);
    for (int j=0;j<Rgroup_1;j++)
    {
        ropeVec[j]->draw();
    }
}

void Sketch_2::update(vector<ofxLeapMotionSimpleHand> LeapHands)//
{
    ofVec3f MyVec (0,1,0);
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
        for (int j=0;j<Rgroup_1;j++)
        {
            if (LeapHands[0].handPos.distance(ropeVec[j]->a)<1000&&LeapHands[0].isLeft)
            {
            }
            else if (LeapHands[0].handPos.distance(ropeVec[j]->b)<1000&&!LeapHands[0].isLeft)
            {
            }
        }
    }
    //2 Hands
    else if (LeapHands.size()==2)
    {
        handsDistance = LeapHands[0].handPos.distance(LeapHands[1].handPos);
//        if(LeapHands[0].handPos.distance(ropeVec[j]->a)&&LeapHands[1].handPos.distance(ropeVec[j]->b)&&LeapHands[0].isLeft)
//        {
//        }
//        if(LeapHands[1].handPos.distance(ropeVec[j]->a)&&LeapHands[0].handPos.distance(ropeVec[j]->b)&&!LeapHands[0].isLeft)
//        {
//            
//        }
    }
    else if(LeapHands.size()==0)
    {
    }
    
}
//    ADDROPE Method
//    --------------------------------------------------------------
int Sketch_2::addRope(Rope** ropesVector,Rope *nextRope,int startString,int endString){
    int i = startString%endString;
    ropesVector[i] = nextRope;
}
void Sketch_2::guiEvent(ofxUIEventArgs &e)
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
void Sketch_2::windowResize(ofResizeEventArgs& data)
{
    if(guiRopes)
    {
        guiRopes->setPosition(ofGetWidth()-210,0);
    }
}
void Sketch_2::keyPress(ofKeyEventArgs &data)
{
    int pressedKey =  data.key;
    if(pressedKey>=49&&pressedKey<57)
    {
        switch (pressedKey)
        {
            case '2':
                if(!currentSketch)
                {
                    //Re setup gui and everything
                    SketchSetup();
                }
                currentSketch = 1;
                break;
            default:
            if (data.key != '2'&&currentSketch)
            {
                SketchQuit();
                currentSketch=0;
            }
            break;
        }
    }
    //attacca stacca filo
    else if(pressedKey=='n'&&currentSketch)
    {
        for (int  i=0;i<Rgroup_1;i++)
        {
            
            if(ropeVec[i]->AttachedA)
            {
                cout<<"staccato"<<endl;
                ropeVec[i]->AttachedA = false;
                ropeVec[i]->AttachedB = false;
                ropeVec[i]->ps[0].mass=1;
                ropeVec[i]->ps[ropeVec[i]->H-1].mass=1;
            }
            else
            {
                cout<<"attaccato"<<endl;
                ropeVec[i]->AttachedA = true;
                ropeVec[i]->AttachedB = true;
                ropeVec[i]->ps[0].mass=15000;
                ropeVec[i]->ps[ropeVec[i]->H-1].mass=15000;
            }
        }
        
    }
    //
    else if(pressedKey=='m'&&currentSketch)
    {
        for(int j=0;j<Rgroup_1;j++)
        {
            for(int k=0;k<ropeVec[j]->H;k++)
            {
                //ropeVec[j]->ps[k].addForce(ofPoint(100000,0,100000));
            }
            ropeVec[j]->ps[ropeVec[j]->H/2].addForce(ofPoint(1000000,0,1000000));
            //ropeVec[j]->ps[ropeVec[j]->H/2].pos+=ofPoint(1000,0,0);
            //ropeVec[j]->ps[ropeVec[j]->H/2].pos+=ofPoint(200,0,0);
        }
    }
    else if(pressedKey=='z'&&currentSketch)
    {
        m.setAddress( "/test" );
        m.addIntArg( 1 );
        m.addFloatArg( 3.5f );
        m.addStringArg( "hello" );
        m.addFloatArg( ofGetElapsedTimef() );
        sender.sendMessage( m );
        m.clear();
    }
    if (guiRopes)
    {
        guiRopes->setPosition(ofGetWidth()-210,0);
    }

}
void Sketch_2::SketchQuit ()
{
    guiRopes->clearWidgets();
    guiRopes->clearEmbeddedWidgets();
    ofAddListener(Sketch_2::guiRopes->newGUIEvent,this,&Sketch_2::guiEvent);
    
    for(int i=Rgroup_1-1;i==0;i++)
    {
        ropeVec[i] = NULL;
    }

}
void Sketch_2::exit(ofEventArgs &arg)
{
    ofRemoveListener(ofEvents().exit, this, &Sketch_2::exit);
    ofRemoveListener(ofEvents().windowResized, this, &Sketch_2::windowResize);
    ofRemoveListener(ofEvents().keyPressed, this, &Sketch_2::keyPress);
}