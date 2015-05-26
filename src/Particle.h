/*
 *  Particle.h
 *  ofProject
 *
 *  Created by Rick Companje on 2/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

struct Particle {
	ofVec3f pos,vel,forces;
	float mass,inverse_mass,drag;
	
	Particle(ofVec3f pos, float mass, float drag = .96f) : pos(pos), mass(mass), drag(drag) {
		if (mass==0.0f) inverse_mass = 0;
		else if (mass<0.001) mass=0.001;
		if (mass!=0.0f) inverse_mass = 1/mass;
	}
	
	void update() {
        if (mass==0.0f) inverse_mass = 0;
        else if (mass<0.001) mass=0.001;
        if (mass!=0.0f) inverse_mass = 1/mass;
		forces *= inverse_mass;
		vel += forces;
		forces =  ofVec3f(0,0,0);
		vel.limit(15);
		pos += vel;
		vel *= drag;
	}
	
	void addForce(ofVec3f oForce) {
		forces += oForce;
	}
    
   };
