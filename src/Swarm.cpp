//
//  Swarm.cpp
//  Swarm
//
//  Created by tucano on 27/05/15.
//
//

#include "Swarm.h"

void Swarm::update()
{
    int i;
    for(i = 0; i < boids.size(); i++)
    {
        boids[i].update(boids);
    }

}

void Swarm::draw()
{
    int w = ofGetWidth();
    int h = ofGetHeight();
    
    int i;
    for(i = 0; i < boids.size(); i++)
    {
        boids[i].draw();
    }
}

void Swarm::addBoid()
{
    boids.push_back(Boid());
}

void Swarm::addBoid(int x, int y)
{
    boids.push_back(Boid(x, y));
}

void Swarm::addBoid(int x, int y, ofImage texture)
{
    boids.push_back(Boid(x, y, texture));
}

void Swarm::removeBoid(int x, int y, int radius) {
    int i;
    for (i=0; i<boids.size(); i++) {
        if(boids[i].isHit(x, y, radius)) {
            boids.erase(boids.begin()+i);
        }
    }
}

void Swarm::setup(int n)
{
    for (int i = 0; i < n; i++)
    {
        addBoid(ofRandomWidth(),ofRandomHeight());
    }
}

void Swarm::setup(int n, ofImage texture)
{
    for (int i = 0; i < n; i++)
    {
        addBoid(ofRandomWidth(),ofRandomHeight(), texture);
    }
}

void Swarm::resize(int w, int h)
{
    
}

void Swarm::clear()
{
    boids.clear();
}

int Swarm::swarmSize() {
    return boids.size();
}