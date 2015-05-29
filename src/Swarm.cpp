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
    ofPushStyle();
    int i;
    for(i = 0; i < boids.size(); i++)
    {
        boids[i].draw();
    }
    ofPopStyle();
}

void Swarm::addBoid() {
    boids.push_back(Boid());
}

void Swarm::addBoid(int x, int y) {
    boids.push_back(Boid(x, y));
}

void Swarm::removeBoid(int x, int y, int radius) {
    int i;
    for (i=0; i<boids.size(); i++) {
        if(boids[i].isHit(x, y, radius)) {
            boids.erase(boids.begin()+i);
        }
    }
}

int Swarm::swarmSize() {
    return boids.size();
}