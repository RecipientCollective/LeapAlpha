//
//  Swarm.h
//  Swarm
//
//  Created by tucano on 27/05/15.
//
//

#pragma once

#include "ofMain.h"
#include "Boid.h"


class Swarm
{
public:    
    void update();
    void draw();
    void addBoid();
    void addBoid(int x, int y);
    void addBoid(int x, int y, ofImage texture);
    void clear();
    void setup(int n);
    void setup(int n, ofImage texture);
    void resize(int w, int h);
    void removeBoid(int x, int y, int radius);
    int swarmSize();
    
    vector<Boid> boids;
};