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
}

void Sketch_4::SketchSetup()
{
    //guisetup
    guiSwarm = new ofxUISuperCanvas("Swarm Sketch (4)");
    
    guiSwarm->setPosition(ofGetWidth()-210,0);

    guiSwarm->autoSizeToFitWidgets();
    ofAddListener(Sketch_4::guiSwarm->newGUIEvent,this,&Sketch_4::guiEvent);//event listener
}

void Sketch_4::update(vector<ofxLeapMotionSimpleHand> LeapHands)
{
    
}

void Sketch_4::draw()
{
    
}

void Sketch_4::exit(ofEventArgs &arg)
{
    ofRemoveListener(ofEvents().windowResized, this, &Sketch_4::windowResize);
    ofRemoveListener(ofEvents().keyPressed, this, &Sketch_4::keyPress);
    ofRemoveListener(ofEvents().exit, this, &Sketch_4::exit);
    ofRemoveListener(Sketch_4::guiSwarm->newGUIEvent,this,&Sketch_4::guiEvent);
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
                if (data.key != '1'&&currentSketch)
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
}

void Sketch_4::guiEvent(ofxUIEventArgs &e)
{
    
}
