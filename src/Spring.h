/*
 *  Spring.h
 *  ofProject
 *
 *  Created by Rick Companje on 2/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "Particle.h"

struct Spring {
	Particle *a, *b;
	float k;
	float rest_length;
	
	Spring(Particle *a, Particle *b, float k = .3) : a(a), b(b), k(k) {
		rest_length = (b->pos - a->pos).length();
	}
	
	void update() {
		ofVec3f dir = b->pos - a->pos;
		float dist = dir.length();
		if (dist == 0.0) dist = 0.0001; // prevent division by zero
		float f = (rest_length - dist) * k; // linear force spring
		dir.normalize();
		a->addForce(dir * -f);
		b->addForce(dir * f);
	}
};
