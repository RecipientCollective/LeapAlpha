#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetBackgroundAuto(false);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    // z mapped to a cube 200,200
    outputZrange = 200;
    
#ifdef DEBUG
    ofSetLogLevel(OF_LOG_NOTICE);
#else
    ofSetLogLevel(OF_LOG_ERROR);
#endif
    
    leap.open();
    
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    bShowGui = true;
    bShowHelp = true;
    
    bSetMapping.addListener(this, &ofApp::setMappingChanged);
    leapXrange.addListener(this, &ofApp::leapXRangeChanged);
    leapZrange.addListener(this, &ofApp::leapZRangeChanged);
    leapYmin.addListener(this, &ofApp::leapYMinChanged);
    leapYmax.addListener(this, &ofApp::leapYMaxChanged);
    
    gui.setup(); // most of the time you don't need a name
    gui.add(guiTitle.setup("leap connection: ",""));
    gui.add(camPositionLabel.setup("cam X,Y,Z: ","null"));
    gui.add(bMirrorImage.setup(" mirror", false));
    gui.add(debugDraw.setup(" debug draw hands", false));
    gui.add(bShowOrigin.setup(" XYZ origin", false));
    gui.add(bShowGridYZ.setup(" YZ grid", false));
    gui.add(bShowGridXZ.setup(" XZ grid", false));
    gui.add(bShowGridXY.setup(" XY grid", false));
    gui.add(bShowGridLabels.setup(" grid labels", false));
    gui.add(bSetMapping.setup(" mapping zone", false));
    gui.add(leapXrange.setup("work area range x", 230, 0, ofGetWidth()/2));
    gui.add(leapZrange.setup("work area range z", 150, 0, outputZrange));
    gui.add(leapYmin.setup("work area min y", 90, 0, ofGetHeight()/2));
    gui.add(leapYmax.setup("work area max y", 490, ofGetHeight()/2, ofGetHeight()));
}

void ofApp::leapYMinChanged(float &min)
{
    if (bSetMapping)
    {
        leap.setMappingY(leapYmin, leapYmax, -ofGetHeight()/2, ofGetHeight()/2);
    }
}

void ofApp::leapYMaxChanged(float &min)
{
    if (bSetMapping)
    {
        leap.setMappingY(leapYmin, leapYmax, -ofGetHeight()/2, ofGetHeight()/2);
    }
}

void ofApp::leapXRangeChanged(float & range)
{
    if (bSetMapping)
    {
        leap.setMappingX(-range, range, -ofGetWidth()/2, ofGetWidth()/2);
    }
}

void ofApp::leapZRangeChanged(float & range)
{
    if (bSetMapping)
    {
        leap.setMappingZ(-range, range, -outputZrange, outputZrange);
    }
}

void ofApp::setMappingChanged(bool & bmapping)
{
    // leap returns data in mm - lets set a mapping to our world space.
    // you can get back a mapped point by using ofxLeapMotion::getMappedofPoint
    // with the Leap::Vector that tipPosition returns
    // If you decrease the range values, a smaller range of physical motion
    // is mapped to the width of the display area, making it easier to move from one side to the other.
    // Y: 90 490
    
    if (bmapping)
    {
        setLeapMapping();
    }
    else
    {
        leap.resetMapping();
    }
}

void ofApp::setLeapMapping()
{
    leap.setMappingX(-leapXrange, leapXrange, -ofGetWidth()/2, ofGetWidth()/2);
    leap.setMappingZ(-leapZrange, leapZrange, -outputZrange, outputZrange);
    leap.setMappingY(leapYmin, leapYmax, -ofGetHeight()/2, ofGetHeight()/2);
}

//--------------------------------------------------------------
void ofApp::update()
{
    // framerate
    ofSetWindowTitle(ofToString(ofGetFrameRate(),0));
    
    // update GUI
    guiTitle = leap.isConnected() ? "YES" : "NO";
    camPositionLabel = ofToString(cam.getX()) + "," + ofToString(cam.getY()) + "," + ofToString(cam.getZ());
    
    fingersFound.clear();
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() )
    {
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



//--------------------------------------------------------------
void ofApp::draw()
{

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    ofBackground(0, 0, 0);
    

    // Leap draw here
    cam.begin();
    
    // Axes are drawn in red (+x), green (+y) and blue (+z), starting from the origin.
    if (bShowOrigin)
    {
        ofDrawAxis(100);
    }
    
    // GRID
    ofPushStyle();
    ofDrawGrid(1000,20,bShowGridLabels,bShowGridXY, bShowGridYZ, bShowGridXZ);
    ofPopStyle();

    
    ofPushMatrix();
    
    if (bMirrorImage)
    {
        ofScale(-1,1,1);
    }
    
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    for(int i = 0; i < simpleHands.size(); i++)
    {
        if (debugDraw)
        {
            simpleHands[i].debugDraw();
        }
        else
        {
            ofSetColor(0, 0, 255);
            ofPoint handPos    = simpleHands[i].handPos;
            ofDrawSphere(handPos.x, handPos.y, handPos.z, 20);
            
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
                ofDrawSphere(mcp.x, mcp.y, mcp.z, 12);
                ofDrawSphere(pip.x, pip.y, pip.z, 12);
                ofDrawSphere(dip.x, dip.y, dip.z, 12);
                ofDrawSphere(tip.x, tip.y, tip.z, 12);
                
                ofSetColor(255, 0, 0);
                ofSetLineWidth(20);
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
        ofSetColor(255, 0, 0);
        ofDrawSphere(center.x, center.y, center.z, 10);
        ofLogNotice() << "FACING HANDS: " << simpleHands[0].handNormal.dot(simpleHands[1].handNormal) << "CENTER: " << center;
    }
    
    
    ofPopMatrix();
    cam.end();
    
    // Gui draw
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_NORMALIZE);
    
    if (bShowGui)
    {
        gui.draw();
    }
    
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

//--------------------------------------------------------------
void ofApp::drawInteractionArea()
{
    ofRectangle vp = ofGetCurrentViewport();
    float r = MIN(vp.width, vp.height) * 0.5f;
    float x = vp.width * 0.5f;
    float y = vp.height * 0.5f;
    
    ofPushStyle();
    ofSetLineWidth(3);
    ofSetColor(255, 255, 0);
    ofNoFill();
    glDepthMask(false);
    ofCircle(x, y, r);
    glDepthMask(true);
    ofPopStyle();
}


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
    
    leapXrange.setMax(ofGetWidth()/2);
    leapYmin.setMax(ofGetHeight()/2);
    leapYmax.setMax(ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit()
{
    // let's close down Leap and kill the controller
    bSetMapping.removeListener(this, &ofApp::setMappingChanged);
    leapXrange.removeListener(this, &ofApp::leapXRangeChanged);
    leapZrange.removeListener(this, &ofApp::leapZRangeChanged);
    leapYmin.removeListener(this, &ofApp::leapYMinChanged);
    leapYmax.removeListener(this, &ofApp::leapYMaxChanged);
    leap.close();
}
