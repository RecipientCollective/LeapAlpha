//
//  Sketch_4.cpp
//  LeapAlpha
//
//  Created by tucano on 15/06/15.
//
//

#include "Sketch_4.h"

void Sketch_4::SetupListeners()
{
    ofAddListener(ofEvents().windowResized, this, &Sketch_4::windowResize);
    ofAddListener(ofEvents().keyPressed, this, &Sketch_4::keyPress);
    ofAddListener(ofEvents().exit, this, &Sketch_4::exit);
    ofAddListener(ofEvents().mouseDragged,this, &Sketch_4::mouseDragged);
    ofAddListener(ofEvents().mousePressed,this, &Sketch_4::mousePressed);
    ofAddListener(ofEvents().mouseReleased,this, &Sketch_4::mouseReleased);
}

void Sketch_4::mouseDragged(ofMouseEventArgs &data)
{
    if (bMouseSwarmControl && currentSketch)
    {
        if (data.button == 0)
        {
            avoid = true;
            avoidPoint.set(data.x,data.y);
        }
        else if (data.button == 2)
        {
            seek = true;
            seekPoint.set(data.x, data.y);
        }
    }
}

void Sketch_4::mousePressed(ofMouseEventArgs &data)
{
    if (bMouseSwarmControl && currentSketch)
    {
        if (data.button == 0)
        {
            avoid = true;
            avoidPoint.set(data.x,data.y);
        }
        else if (data.button == 2)
        {
            seek = true;
            seekPoint.set(data.x, data.y);
        }
    }
}

void Sketch_4::mouseReleased(ofMouseEventArgs &data)
{
    if (bMouseSwarmControl && currentSketch)
    {
        if (data.button == 0)
        {
            avoid = false;
            avoidPoint.set(0.0f,0.0f);
        }
        else if (data.button == 2)
        {
            seek = false;
            seekPoint.set(data.x, data.y);
        }
    }
}

void Sketch_4::SketchSetup()
{
    startCount = START_COUNT;
    bMouseSwarmControl = false;
    bShowControlPoints = false;
    
    avoidPoint = ofPoint(0.0,0.0);
    avoid = false;
    seekPoint = ofPoint(0.0,0.0);
    seek = false;
    startCount = START_COUNT;
    maxDistance = MAX_DISTANCE;
    seekArea = MAXSEEKAREA;
    
    avoidZthreshold = 0.0f; // position in z to start avoid
    
    //guisetup
    guiSwarm = new ofxUISuperCanvas("Swarm Sketch (4)");
    
    guiSwarm->setPosition(ofGetWidth()-210,0);
    guiSwarm->addButton("REINIT SWARM", false);
    guiSwarm->addIntSlider("BOIDS", 1, 1000, &startCount);
    guiSwarm->addToggle("MOUSE CONTROL", &bMouseSwarmControl);
    guiSwarm->addToggle("SHOW CONTROL POINTS", &bShowControlPoints);
    guiSwarm->addSlider("AVOID AREA", 1, 1000, &maxDistance);
    guiSwarm->addSlider("SEEK AREA", 1, 1000, &seekArea);
    guiSwarm->addSlider("AVOID Z THRESHOLD", -500, 500, &avoidZthreshold);
    
    guiSwarm->autoSizeToFitWidgets();
    ofAddListener(Sketch_4::guiSwarm->newGUIEvent,this,&Sketch_4::guiEvent);//event listener
    
    texture.loadImage(FISH_IMAGE);
    swarm.clear();
    swarm.setup(startCount, texture);

}

void Sketch_4::update(vector<ofxLeapMotionSimpleHand> LeapHands, ofCamera cam)
{
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    //1 Hand
    if (LeapHands.size()==1)
    {
        ofPoint handPos    = LeapHands[0].handPos;
        ofPoint handNormal = LeapHands[0].handNormal;
        
        if (handNormal.y > HAND_NORMAL_LIMIT)
        {
            seek = true;
            avoid = false;
            viewPortPoint = cam.worldToScreen(handPos);
            seekPoint.set(viewPortPoint.x, viewPortPoint.y);
        }
        else
        {
            seek = false;

            // get hand index fingertip for swarm avoid
            ofPoint index_tip = LeapHands[0].fingers[ fingerTypes[1] ].tip;
            
            if  (index_tip.z < 0 && index_tip.z <= avoidZthreshold)
            {
                avoid = true;
                viewPortPoint = cam.worldToScreen(index_tip);
                avoidPoint.set(viewPortPoint.x, viewPortPoint.y);
            }
            else
            {
                avoid = false;
            }
        }
    }
    // 2 HANDS
    else if (LeapHands.size()==2)
    {
        avoid = false;
        
        // seek center point
        if (LeapHands[0].handNormal.dot(LeapHands[1].handNormal) < 0)
        {
            seek = true;
            ofPoint center = LeapHands[0].handPos.getMiddle(LeapHands[1].handPos);
            viewPortPoint = cam.worldToScreen(center);
            seekPoint.set(viewPortPoint.x, viewPortPoint.y);
        }
        else
        {
            seek = false;
        }
    }
    
    // 0 HANDS
    if (LeapHands.empty() && !bMouseSwarmControl)
    {
        seek = false;
        avoid = false;
    }
    
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
    
    swarm.update();
}

void Sketch_4::draw()
{
    if (bShowControlPoints)
    {
        if (avoid)
        {
            ofPushStyle();
            ofSetColor(255,0,0);
            ofCircle(avoidPoint,4.0f);
            ofNoFill();
            ofCircle(avoidPoint, maxDistance);
            ofPopStyle();
        }
        else if (seek)
        {
            ofPushStyle();
            ofSetColor(0,255,0);
            ofCircle(seekPoint,4.0f);
            ofNoFill();
            ofCircle(seekPoint, seekArea);
            ofPopStyle();
        }
    }
    swarm.draw();
}

void Sketch_4::exit(ofEventArgs &arg)
{
    ofRemoveListener(ofEvents().windowResized, this, &Sketch_4::windowResize);
    ofRemoveListener(ofEvents().keyPressed, this, &Sketch_4::keyPress);
    ofRemoveListener(ofEvents().exit, this, &Sketch_4::exit);
    ofRemoveListener(ofEvents().mouseDragged,this, &Sketch_4::mouseDragged);
    ofRemoveListener(ofEvents().mousePressed,this, &Sketch_4::mousePressed);
    ofRemoveListener(ofEvents().mouseReleased,this, &Sketch_4::mouseReleased);
    delete guiSwarm;
}

void Sketch_4::keyPress(ofKeyEventArgs &data)
{
    int pressedKey = data.key;
    if(pressedKey>=49&&pressedKey<57)
    {
        switch (pressedKey)
        {
            case '4':
                if(!currentSketch)
                {
                    //Re setup gui and everything
                    SketchSetup();
                }
                currentSketch = 1;
                break;
            default:
                if (data.key != '4'&&currentSketch)
                {
                    SketchQuit();
                    currentSketch=0;
                }
                break;
        }
    }
    
    //adjust gui canvas pos
    if (guiSwarm)
    {
        guiSwarm->setPosition(ofGetWidth()-210,0);
    }
}

void Sketch_4::windowResize(ofResizeEventArgs &data)
{
    if(guiSwarm)
    {
        guiSwarm->setPosition(ofGetWidth()-210,0);
    }
}

void Sketch_4::SketchQuit()
{
    guiSwarm->clearWidgets();
    guiSwarm->clearEmbeddedWidgets();
    guiSwarm->disable();
    swarm.clear();
}

void Sketch_4::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
    if (name == "REINIT SWARM")
    {
        swarm.clear();
        swarm.setup(startCount, texture);
    }
}
