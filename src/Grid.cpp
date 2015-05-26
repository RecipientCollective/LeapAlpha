//
//  Grid.cpp
//  LeapAlpha
//
//  Created by tucano on 22/05/15.
//
//

#include "Grid.h"

void grid::customDraw(float labels, float x, float y, float z)
{
    ofPushStyle();
    ofDrawGrid(1000.0f,20,labels,x,y,z);
    ofPopStyle();
}