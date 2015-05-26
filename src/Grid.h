#pragma once
#include "ofMain.h"

// grid is a custom 3D
// We inherit from ofNode

class grid : public ofNode
{
public:
    void    customDraw(float labels, float x, float y, float z);
};