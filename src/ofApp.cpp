#include "ofApp.h"


//SETUP
//--------------------------------------------------------------
void ofApp::setup()
{
    ofBackground(0, 0, 0);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
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
    box.set(outputXrange*2, outputYmax - outputYmin, outputZrange*2);
    box.setPosition(0.0f, 0.0f, 0.0f);
    
    swarmSetup();
}

//--------------------------------------------------------------
void ofApp::update()
{
    //Update Leap Motion Datas
    LeapUpdate();

    // Update swarm
    if (avoid)
    {
        for (int i = 0; i<swarm.swarmSize(); i++)
        {
            ofVec2f dist = swarm.boids[i].loc - avoidPoint;
            if (dist.lengthSquared() < (maxDistance*maxDistance) )
            {
                swarm.boids[i].avoid(avoidPoint);
            }
        }
    }
    
    if (seek)
    {
        for (int i = 0; i<swarm.swarmSize(); i++)
        {
            ofVec2f dist = swarm.boids[i].loc - seekPoint;
            if (dist.lengthSquared() < (seekArea*seekArea) )
            {
                swarm.boids[i].arrive(seekPoint);
            }
        }
    }
    
    // update swarm every 4 frame
    swarm.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    
    ofPushMatrix();
    cam.begin();
    
    if (bMirrorImage)
    {
        ofScale(1,-1,1);
    }
    
    
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
    
    nodeGrid.customDraw(bShowGridLabels,bShowGridXY, bShowGridYZ, bShowGridXZ);
    
    if (bShowWorkingArea)
    {
        ofPushMatrix();
        ofPushStyle();
        ofSetColor(0, 255, 0);
        box.drawWireframe();
        ofPopStyle();
        ofPopMatrix();
    }
    
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    //Draw Hands
    if (debugDraw)
    {
        ofPushStyle();
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
        ofPopStyle();
    }
    
    cam.end();
    ofPopMatrix();
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_NORMALIZE);

    // 2D MODE
    ofPushMatrix();
    
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
    
    if (bMirrorImage)
    {
        ofSetRectMode( OF_RECTMODE_CENTER );
        ofTranslate(0, ofGetHeight(),0);
        ofScale(1, -1, 1);
    }
    
    if (eraseMode)
    {
        ofPushStyle();
        ofSetColor(255,0,0);
        ofCircle(erasePoint,4.0f);
        ofNoFill();
        ofCircle(erasePoint, ERASE_RADIUS);
        ofPopStyle();
    }
    
    if (seek && debugDraw)
    {
        ofPushStyle();
        ofSetColor(0,255,0);
        ofCircle(seekPoint,4.0f);
        ofNoFill();
        ofCircle(seekPoint, seekArea);
        ofPopStyle();
    }
    else if (avoid && debugDraw)
    {
        ofPushStyle();
        ofSetColor(255,0,0);
        ofCircle(avoidPoint,4.0f);
        ofNoFill();
        ofCircle(avoidPoint, maxDistance);
        ofPopStyle();
    }
    
    // SWARM
    swarm.draw();
    
    if (bMirrorImage)
    {
        ofSetRectMode( OF_RECTMODE_CORNER );
    }
    ofPopMatrix();
}

void ofApp::LeapUpdate()
{
    //leap Update
    fingersFound.clear();
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    simpleHands = leap.getSimpleHands();
    
    if(leap.isFrameNew() && simpleHands.size() )
    {
        // ONLY FIRST HAND
        ofPoint handPos    = simpleHands[0].handPos;
        ofPoint handNormal = simpleHands[0].handNormal;
        
        if (handNormal.y > 0.8f)
        {
            seek = true;
            avoid = false;
            ofVec3f wp = cam.worldToScreen(handPos);
            seekPoint.set(wp.x, wp.y);
            
        }
        else
        {
            seek = false;
            
            // get hand index fingertip for swarm avoid
            ofPoint index_tip = simpleHands[0].fingers[ fingerTypes[1] ].tip;
            
            // z is negative in front of leap
            if (index_tip.z < 0 && index_tip.z <= avoidZthreshold && !avoid)
            {
                avoid = true;
                ofVec3f wp = cam.worldToScreen(index_tip);
                avoidPoint.set(wp.x, wp.y);
            }
            else
            {
                avoid = false;
            }
        }
    }
    
    if (simpleHands.empty() && !bMouseAvoid)
    {
        seek = false;
        avoid = false;
    }
    
    //IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
    leap.markFrameAsOld();
}

//Set Leap Re-Mapping
//--------------------------------------------------------------
void ofApp::setLeapMapping()
{
    leap.setMappingX(-leapXrange, leapXrange, -outputXrange, outputXrange);
    leap.setMappingZ(-leapZrange, leapZrange, -outputZrange, outputZrange);
    leap.setMappingY(leapYmin, leapYmax, outputYmin, outputYmax);
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
            guiLeap->setVisible(bShowGui);
            guiSwarm->setVisible(bShowGui);
            break;
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
        case 'H':
        case 'h':
            bShowHelp = !bShowHelp;
            break;
        case 'S':
        case 's':
            guiSwarm ->saveSettings(GUISWARM_SETTINGS);
            guiLeap -> saveSettings(GUILEAP_SETTINGS);
            break;
        case 'L':
        case 'l':
            guiSwarm ->loadSettings(GUISWARM_SETTINGS);
            guiLeap -> loadSettings(GUILEAP_SETTINGS);
            break;
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
void ofApp::mouseDragged(int x, int y, int button)
{
    if (bSwarmErase && button == 0)
    {
        eraseMode = true;
        erasePoint.set(x,y);
        swarm.removeBoid(x, y, ERASE_RADIUS);
    }
    else if (bMouseAvoid && button == 0)
    {
        avoid = true;
        avoidPoint.set(x,y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    if (bSwarmErase && button == 0)
    {
        eraseMode = true;
        erasePoint.set(x,y);
        swarm.removeBoid(x, y, ERASE_RADIUS);
    }
    else if (bMouseAvoid && button == 0)
    {
        avoid = true;
        avoidPoint.set(x,y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
    if (bSwarmErase && button == 0)
    {
        eraseMode = false;
        erasePoint.set(0.0f,0.0f);
    }
    else if (bMouseAvoid && button == 0)
    {
        avoid = false;
        avoidPoint.set(0.0f,0.0f);
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
    if (bSetMapping)
    {
        // reset mapping on new output
        setLeapMapping();
    }
    ofxUISlider *slider = (ofxUISlider*) guiLeap->getWidget("X range (output)");
    slider->setMax(ofGetWidth());
    slider = (ofxUISlider*) guiLeap->getWidget("Y min (output)");
    slider->setMax(ofGetHeight());
    slider->setMin(-ofGetHeight());
    slider = (ofxUISlider*) guiLeap->getWidget("Y max (output)");
    slider->setMax(ofGetHeight());
    slider->setMin(-ofGetHeight());
    
    guiSwarm->setPosition(ofGetWidth()-210,10);
    
    ofLogNotice() << "H: " << ofGetHeight() << ", W:" << ofGetWidth();
    swarm.resize(w,h);
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
        box.set( outputXrange, outputYmax-outputYmin, outputZrange*2);
        setLeapMapping();
    }
    else if (name == "X range (output)")
    {
        box.set( outputXrange, outputYmax-outputYmin, outputZrange*2);
        setLeapMapping();
    }
    else if (name == "Y min (output)")
    {
        box.set( outputXrange, outputYmax-outputYmin, outputZrange*2);
        setLeapMapping();
    }
    else if (name == "Y max (output)")
    {
        box.set( outputXrange, outputYmax-outputYmin, outputZrange*2);
        setLeapMapping();
    }
    else if (name == "WorkArea X range")
    {
        setLeapMapping();
    }
    else if (name == "WorkArea Z range")
    {
        setLeapMapping();
    }
    else if (name == "WorkArea min Y")
    {
        setLeapMapping();
    }
    else if (name == "WorkArea max Y")
    {
        setLeapMapping();
    }
    else if (name == "REINIT SWARM")
    {
        swarmSetup();
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
    guiLeap->addSlider("X range (output)", 0, ofGetWidth(), &outputXrange);
    guiLeap->addSlider("Y min (output)", -ofGetHeight(), ofGetHeight(), &outputYmin);
    guiLeap->addSlider("Y max (output)", -ofGetHeight(), ofGetHeight(), &outputYmax);
    guiLeap->addSlider("Z range(output)", 0, 500, &outputZrange);
    guiLeap->addToggle("Show WorkArea", &bShowWorkingArea);
    guiLeap->addSlider("WorkArea X range", 0, outputXrange, &leapXrange);
    guiLeap->addSlider("WorkArea Z range", 0, outputZrange, &leapZrange);
    guiLeap->addSlider("WorkArea min Y", outputYmin, outputYmax, &leapYmin);
    guiLeap->addSlider("WorkArea max Y",  outputYmin, outputYmax, &leapYmax);
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
    outputXrange = ofGetWidth()/2.0f;
    outputYmin = 0.0f;
    outputYmax = ofGetHeight();
    leapXrange = 230;
    leapZrange = 150;
    leapYmin = 90;
    leapYmax = 490;
    
    // SWARM DEFAULT OPTIONS
    avoidPoint = ofPoint(0.0,0.0);
    avoid = false;
    seekPoint = ofPoint(0.0,0.0);
    seek = false;
    startCount = START_COUNT;
    maxDistance = MAX_DISTANCE;
    bSwarmErase = false;
    bMouseAvoid = false;
    eraseMode = false;
    erasePoint = ofPoint(0.0,0.0);
    seekArea = MAXSEEKAREA;
    avoidZthreshold = 0.0f; // position in z to start avoid
    
    // GUI SWARM
    guiSwarm = new ofxUISuperCanvas("Swarm : " );
    guiSwarm->setPosition(ofGetWidth()-210,10);
    guiSwarm->addButton("REINIT SWARM", false);
    guiSwarm->addIntSlider("BOIDS", 1, 1000, &startCount);
    guiSwarm->addSlider("AVOID AREA", 1, 1000, &maxDistance);
    guiSwarm->addSlider("SEEK AREA", 1, 1000, &seekArea);
    guiSwarm->addToggle("ERASE MODE", &bSwarmErase);
    guiSwarm->addToggle("MOUSE AVOID", &bMouseAvoid);
    guiSwarm->addSlider("AVOID Z THRESHOLD", 0, outputZrange, &avoidZthreshold);
    guiSwarm->addFPS();
    guiSwarm->autoSizeToFitWidgets();
    ofAddListener(guiSwarm->newGUIEvent,this,&ofApp::guiEvent);//event listener
}

//--------------------------------------------------------------
void ofApp::swarmSetup()
{
    texture.loadImage(FISH_IMAGE);
    swarm.clear();
    swarm.setup(startCount, texture);
}

//--------------------------------------------------------------
void ofApp::exit()
{
    // let's close down Leap and kill the controller
    leap.close();
    
    delete guiSwarm;
    delete guiLeap;
}
