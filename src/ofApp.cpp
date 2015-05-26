#include "ofApp.h"
#include <Poco/Path.h>

ofPoint mouseisHere;
float bSmooth;
float p_mass;
float p_drag;
float sp_k;
bool VisCtrlPoints;
int nPoints;
bool handsSketch_1;
bool handsSketch_2;
bool handsSketch_3;
bool handsSketch_4;
bool handsSketch_5;
bool handsSketch_6;
bool handsSketch_7;
ofPoint velocityL;
ofPoint velocityR;

//SETUP
//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetBackgroundAuto(false);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofNoFill();
    
    VisCtrlPoints = 0;
    
#ifdef DEBUG
    ofSetLogLevel(OF_LOG_NOTICE);
#else
    ofSetLogLevel(OF_LOG_ERROR);
#endif
    
    //leap init
    leap.open();
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    //GUI setup and init
    guiSetup();
    
    // working area
    box.set(ofGetWidth(), ofGetHeight(), outputZrange*2);
    box.setPosition(0.0f, 0.0f, 0.0f);
    
    // Add Group of Ropes
    ropeVec = new Rope*[Rgroup_1];
    int ropesLenght = 200;
    for(int i=0; i<Rgroup_1; i++)
    {
        addRope(ropeVec,new Rope(ofPoint(300,200,0),ofPoint(300,500,0),40,ropesLenght,ofPoint(0,0,0),0.01f,true),i,Rgroup_1);
    }
}

//--------------------------------------------------------------
void ofApp::update()
{
    //Update Ropes A and B Position
    for (int j=0;j<Rgroup_1;j++)
    {
        ropeVec[j]->update();
    }
    
    //Update Leap Motion Datas
    LeapUpdate();
    
    //HANDS CONTROL SKETCH #1
    if (handsSketch_1)
    {
        handsControlSketch1();
    }
    
    //Mouse Follow (Ropes Point B)
    else if (MouseFollow)
    {
        mouseisHere = ofPoint(ofGetMouseX(),ofGetMouseY(),0);
        for (int  i = 0;i<Rgroup_1;i++)
        {
            //ropeVec[i]->smoothMoveB=0.06f;
            ropeVec[i]->b = mouseisHere;
        }
    }
}
//--------------------------------------------------------------
void ofApp::draw()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    ofBackground(0, 0, 0);
    
    cam.begin();
    
    // FPS BAR
    ofSetWindowTitle(ofToString(ofGetFrameRate(),0));
    
    // UPDATE Camera Label
    ofxUILabel *label = (ofxUILabel*) guiLeap->getWidget("leapOn");
    label->setLabel(leap.isConnected() ? "connected" : "disconnected");
    label = (ofxUILabel*) guiLeap->getWidget("Cam X,Y,Z: ");
    label->setLabel("Cam Pos: "+ofToString(cam.getX()) + "," + ofToString(cam.getY()) + "," + ofToString(cam.getZ()));
    
    // Axes are drawn in red (+x), green (+y) and blue (+z), starting from the origin.
    if (bShowOrigin)
    {
        ofDrawAxis(100);
    }
    if (bShowWorkingArea)
    {
        ofSetColor(0, 255, 0);
        box.drawWireframe();
    }
    
    nodeGrid.customDraw(bShowGridLabels,bShowGridXY, bShowGridYZ, bShowGridXZ);
    
    //////////////////////////////////////////////////////////
    //PushMatrix (Hands and Ropes)
    ofPushMatrix();
    if (bMirrorImage)
    {
        ofScale(-1,1,1);
    }
    
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    //Draw Hands
    if (debugDraw)
    {
        for(int i = 0; i < simpleHands.size(); i++)
        {
            //simpleHands[0].debugDraw();
            ofSetColor(0, 0, 255);
            ofPoint handPos = simpleHands[i].handPos;
            ofDrawSphere(handPos.x, handPos.y, handPos.z, 5);
            
            ofPoint handNormal = simpleHands[i].handNormal;
            ofSetColor(255, 255, 0);
            ofDrawArrow(handPos, handPos + 100*handNormal);
            
            for (int f=0; f<5; f++)
            {
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip
                
                ofSetColor(0, 255, 0);
                ofDrawSphere(mcp.x, mcp.y, mcp.z, 2);
                ofDrawSphere(pip.x, pip.y, pip.z, 2);
                ofDrawSphere(dip.x, dip.y, dip.z, 2);
                ofDrawSphere(tip.x, tip.y, tip.z, 2);
                
                ofSetColor(255, 0, 0);
                ofSetLineWidth(2);
                ofLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
                ofLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
                ofLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
            }
        }
    }
    
    // draw center point: if dot < 0 hands are facing
    if (simpleHands.size() == 2 && simpleHands[0].handNormal.dot(simpleHands[1].handNormal) < 0)
    {
        ofPoint center = simpleHands[0].handPos.getMiddle(simpleHands[1].handPos);
        if (debugDraw)
        {
            ofSetColor(255, 0, 0);
            ofDrawSphere(center.x, center.y, center.z, 5);
        }
        ofLogNotice() << "FACING HANDS: " << simpleHands[0].handNormal.dot(simpleHands[1].handNormal) << "CENTER: " << center;
    }
    
    
    //////////////////////////////////////////////////////////
    //Draw Ropes
    ofSetLineWidth(0.9f);
    for (int j=0;j<Rgroup_1;j++)
    {
        ropeVec[j]->draw();
    }
    //PopMatrix(Hands and Ropes)
    ofPopMatrix();
    
    cam.end();
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_NORMALIZE);

    // print help
    if (bShowHelp)
    {
        string msg = string("Using mouse inputs to navigate (press 'c' to toggle): ") + (cam.getMouseInputEnabled() ? "YES" : "NO");
        msg += string("\nShowing help (press 'h' to toggle)");
        msg += string("\nShowing gui  (press 'g' to toggle)");
        msg += string("\nFULLSCREEN:  (press 'F' to toggle)");
        msg += "\n\nLEFT MOUSE BUTTON DRAG:\nStart dragging INSIDE the yellow circle -> camera XY rotation .\nStart dragging OUTSIDE the yellow circle -> camera Z rotation (roll).\n\n";
        msg += "LEFT MOUSE BUTTON DRAG + TRANSLATION KEY (" + ofToString(cam.getTranslationKey()) + ") PRESSED\n";
        msg += "OR MIDDLE MOUSE BUTTON (if available):\n";
        msg += "move over XY axes (truck and boom).\n\n";
        msg += "RIGHT MOUSE BUTTON:\n";
        msg += "move over Z axis (dolly)";
        msg += "LEFT MOUSE DOUBLE CLICK:\n";
        msg += "reset to default position/rotation";
        ofDrawBitmapStringHighlight(msg, 10, ofGetHeight() - 220);
    }
    
    if (cam.getMouseInputEnabled())
    {
        drawInteractionArea();
    }
}

void ofApp::LeapUpdate()
{
    //leap Update
    fingersFound.clear();
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    simpleHands = leap.getSimpleHands();
    if(leap.isFrameNew() && simpleHands.size() ){
        
        //Hands Distance
        if (simpleHands.size()>1)
        handsDistance = simpleHands[0].handPos.distance(simpleHands[1].handPos);
        
        //Hands Vel
        cout<<"L: "<<simpleHands[0].palmVel<< endl;
        cout<<"R: "<<simpleHands[1].palmVel<< endl;
        
        //get and assign Fingers
        for(int i = 0; i < simpleHands.size(); i++)
        {
            for (int f=0; f<5; f++)
            {
                int id = simpleHands[i].fingers[ fingerTypes[f] ].id;
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp; // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip; // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip; // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip; // fingertip
                fingersFound.push_back(id);
            }
        }
    }
    //IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
    leap.markFrameAsOld();
}

//handsControlSketch1
//--------------------------------------------------------------
void ofApp::handsControlSketch1()
{
    //Update Ropes A and B Position
    for (int j=0;j<Rgroup_1;j++)
    {
        float randomOffSet = ofRandom(40)-20;
        //Assign Hands to A & B
        if (simpleHands.size()==1)
        {
            //LeftHand
            if (simpleHands[0].isLeft)//&&simpleHands[0].handPos.distance(ropeVec[j]->b)<=ropeVec[j]->WireLength)
            {
                ropeVec[j]->a.interpolate(simpleHands[0].handPos+randomOffSet,0.1);
                //ropeVec[j]->a = simpleHands[0].handPos;
            }
            //RightHand
            else if(!simpleHands[0].isLeft)//&&simpleHands[0].handPos.distance(ropeVec[j]->a)<=ropeVec[j]->WireLength)
            {
                ropeVec[j]->b.interpolate(simpleHands[0].handPos+randomOffSet,0.1);
                //ropeVec[j]->b = simpleHands[0].handPos;
            }
        }
        //BOTH HANDS
        else if(simpleHands.size()==2)//&&handsDistance<=ropeVec[j]->WireLength)
        {
            if (simpleHands[0].isLeft) {
                ropeVec[j]->a.interpolate(simpleHands[0].handPos+randomOffSet,0.1);
                ropeVec[j]->b.interpolate(simpleHands[1].handPos+randomOffSet,0.1);
//                ropeVec[j]->a = simpleHands[0].handPos;
//                ropeVec[j]->b = simpleHands[1].handPos;
            }
            else
            {
                ropeVec[j]->b.interpolate(simpleHands[0].handPos+randomOffSet,0.1);
                ropeVec[j]->a.interpolate(simpleHands[1].handPos+randomOffSet,0.1);
                //ropeVec[j]->b = simpleHands[0].handPos;
                //ropeVec[j]->a = simpleHands[1].handPos;
            }
        }
    }
}

//ADDROPE Method
//--------------------------------------------------------------
int ofApp::addRope(Rope** ropesVector,Rope *nextRope,int startString,int endString){
    int i = startString%endString;
    ropesVector[i] = nextRope;
}


//Set Leap Re-Mapping
//--------------------------------------------------------------
void ofApp::setLeapMapping()
{
    leap.setMappingX(-leapXrange, leapXrange, -ofGetWidth()/2, ofGetWidth()/2);
    leap.setMappingZ(-leapZrange, leapZrange, -outputZrange, outputZrange);
    leap.setMappingY(leapYmin, leapYmax, -ofGetHeight()/2, ofGetHeight()/2);
}

//DRAW INTERACTION AREA
//--------------------------------------------------------------
void ofApp::drawInteractionArea()
{
    ofRectangle vp = ofGetCurrentViewport();
    float r = MIN(vp.width, vp.height) * 0.5f;
    float x = vp.width * 0.5f;
    float y = vp.height * 0.5f;
    
    ofPushStyle();
    ofSetLineWidth(1);
    ofSetColor(255, 255, 255);
    ofNoFill();
    glDepthMask(false);
    ofCircle(x, y, r);
    glDepthMask(true);
    ofPopStyle();
}
//KEY PRESSED
//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch (key)
    {
        case '0': //MOUSEFOLLOW
            MouseFollow = 1;
            handsSketch_1 = 0;
            break;
        case '1'://HANDS SKETCH #1
            MouseFollow = 0;
            handsSketch_1 = 1;
            break;
        
        case '2'://HANDS SKETCH #2
            MouseFollow = 0;
            handsSketch_1 = 0;
            handsSketch_2 = 1;
            break;

        case '3'://HANDS SKETCH #3
            MouseFollow = 0;
            break;
        
        case '4'://HANDS SKETCH #4
            MouseFollow = 0;
            break;
        
        case '5'://HANDS SKETCH #5
            MouseFollow = 0;
            break;
        
        case '6'://HANDS SKETCH #6
            MouseFollow = 0;
            break;
            
        case '7'://HANDS SKETCH #7
            break;
            
        case 'C':
        case 'c':
            if(cam.getMouseInputEnabled()) cam.disableMouseInput();
            else cam.enableMouseInput();
            break;
        case 'G':
        case 'g':
            bShowGui = !bShowGui;
            break;
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
        case 'H':
        case 'h':
            bShowHelp = !bShowHelp;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
    if (bSetMapping)
    {
        // reset mapping on new output
        setLeapMapping();
    }
    ofxUISlider *slider = (ofxUISlider*) guiLeap->getWidget("WorkArea X range");
    slider->setMax(ofGetWidth()/2);
    slider = (ofxUISlider*) guiLeap->getWidget("WorkArea min Y");
    slider->setMax(ofGetHeight()/2);
    slider = (ofxUISlider*) guiLeap->getWidget("WorkArea max Y");
    slider->setMax(ofGetHeight());
    guiRopes->setPosition(ofGetWidth()-210,0);
    box.set(ofGetWidth(), ofGetHeight(), outputZrange*2);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//GUI METHODS AND EVENTS
//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
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
    else if (name == "Debug Draw Hands")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        debugDraw = toggle->getValue();
    }
    else if (name == "Mirror")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bMirrorImage = toggle->getValue();
    }
    else if (name == "Origin")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bShowOrigin = toggle->getValue();
    }
    else if (name == "X Grid")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bShowGridYZ = toggle->getValue();
    }
    else if (name == "Y Grid")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bShowGridXZ = toggle->getValue();
    }
    else if (name == "Z Grid")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bShowGridXY = toggle->getValue();
    }
    else if (name == "Grid Labels")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bShowGridLabels = toggle->getValue();
    }
    else if (name == "MappingZone")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        
        // leap returns data in mm - lets set a mapping to our world space.
        // you can get back a mapped point by using ofxLeapMotion::getMappedofPoint
        // with the Leap::Vector that tipPosition returns
        // If you decrease the range values, a smaller range of physical motion
        // is mapped to the width of the display area, making it easier to move from one side to the other.
        // Y: 90 490
        if (bSetMapping)
        {
            setLeapMapping();
        }
        else
        {
            leap.resetMapping();
        }
        
    }
    else if (name == "Z range(output)")
    {
        //ofxUISlider *slider = (ofxUISlider *) e.widget; ;
        //outputZrange = slider->getValue();
        ofxUISlider *sliderWAreaX = (ofxUISlider *)guiLeap->getWidget("WorkArea Z range");
        sliderWAreaX->setMax(outputZrange);
        if (bSetMapping)
        {
            leap.setMappingZ(-outputZrange, outputZrange, -outputZrange, outputZrange);
        }
        box.set( ofGetWidth(), ofGetHeight(), outputZrange*2);
    }
    else if (name == "Show WorkArea")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        //bShowWorkingArea = toggle->getValue();
    }
    else if (name == "WorkArea X range")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        //leapXrange = slider->getValue();
        if (bSetMapping)
        {
            leap.setMappingX(-leapXrange, leapXrange, -ofGetWidth()/2, ofGetWidth()/2);
        }
    }
    else if (name == "WorkArea Z range")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        //leapZrange = slider->getValue();
        if (bSetMapping)
        {
            leap.setMappingZ(-leapZrange, leapXrange, -outputZrange, outputZrange);
        }
    }
    else if (name == "WorkArea min Y")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        //leapYmin = slider->getValue();
        if (bSetMapping)
        {
            leap.setMappingY(leapYmin, leapYmax, -ofGetHeight()/2, ofGetHeight()/2);
        }
    }
    else if (name == "WorkArea max Y") {
        if (bSetMapping)
        {
            leap.setMappingY(leapYmin, leapYmax, -ofGetHeight()/2, ofGetHeight()/2);
        }
    }
}
void ofApp::guiSetup()
{
    // Show/Hide GUI Elements
    bShowGui = true;
    bShowHelp = false;
    
    //GUI SETUP
    guiLeap = new ofxUISuperCanvas("LeapMotion : ");
    guiLeap->addLabel("leapOn","null");
    guiLeap->addSpacer();
    guiLeap->addLabel("Cam X,Y,Z: ","null");
    guiLeap->addSpacer();
    guiLeap->addToggle("Debug Draw Hands", &debugDraw);
    guiLeap->addToggle("Mirror", &bMirrorImage);
    guiLeap->addSpacer();
    guiLeap->addToggle("Origin", &bShowOrigin);
    guiLeap->addToggle("X Grid", &bShowGridYZ);
    guiLeap->addToggle("Y Grid", &bShowGridXZ);
    guiLeap->addToggle("Z Grid", &bShowGridXY);
    guiLeap->addToggle("Grid Labels", &bShowGridLabels);
    guiLeap->addSpacer();
    guiLeap->addToggle("MappingZone", &bSetMapping);
    // z mapped to a cube 200,200
    guiLeap->addSlider("Z range(output)", 0, 500, &outputZrange);
    guiLeap->addToggle("Show WorkArea", &bShowWorkingArea);
    guiLeap->addSlider("WorkArea X range", 0, ofGetWidth()/2, &leapXrange);
    guiLeap->addSlider("WorkArea Z range", 0, outputZrange, &leapZrange);
    guiLeap->addSlider("WorkArea min Y", 0, ofGetHeight()/2, &leapYmin);
    guiLeap->addSlider("WorkArea max Y",  ofGetHeight()/2, ofGetHeight(), &leapYmax);
    guiLeap->addSpacer();
    guiLeap->addLabel("Press 'H' for Help");
    guiLeap->autoSizeToFitWidgets();
    ofAddListener(guiLeap->newGUIEvent,this,&ofApp::guiEvent);//event listener
    
    guiRopes = new ofxUISuperCanvas("Ropes");
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
    guiRopes->addSlider("B_smoothing", .001f, 1.0f, bSmooth=0.001f);
    guiRopes->addSpacer();
    p_mass = 1.0f;
    p_drag = 0.96f;
    sp_k = 0.2f;
    guiRopes->addSlider("Particle_Mass", .001f, 1.0f, p_mass=0.001f);
    guiRopes->addSpacer();
    guiRopes->addSlider("Particle_Drag", 0.0f, 1.0f, p_drag=0.001f);
    guiRopes->addSpacer();
    guiRopes->addSlider("Spring_k", 0.0f, 1.0f, sp_k=0.001f);
    guiRopes->addSpacer();
    guiRopes->addToggle("Ctrl_Points", &VisCtrlPoints);
    guiRopes->addSpacer();
    guiRopes->autoSizeToFitWidgets();
    ofAddListener(guiRopes->newGUIEvent,this,&ofApp::guiEvent);//event listener
    
    //GUI INIT
    debugDraw = true;
    bMirrorImage = false;
    bShowOrigin = false;
    bShowGridYZ = false;
    bShowGridXZ = false;
    bShowGridXY = false;
    bShowGridLabels = false;
    bSetMapping = false;
    outputZrange = 200;
    leapXrange = 230;
    leapZrange = 150;
    leapYmin = 90;
    leapYmax = 490;
}
//--------------------------------------------------------------
void ofApp::exit()
{
    // let's close down Leap and kill the controller
    leap.close();
}
