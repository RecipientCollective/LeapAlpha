#include "ofApp.h"


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
    
}

//--------------------------------------------------------------
void ofApp::update()
{
    
    //Update Leap Motion Datas
    LeapUpdate();
    
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
    
    if (name == "Debug Draw Hands")
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
