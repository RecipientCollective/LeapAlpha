#ifndef BOID_H
#define BOID_H

#include "ofMain.h"

#define MAX_BOID_SPEED 5.0f
#define MAX_BOID_FORCE 0.5f
#define BOID_RADIUS 3.0f
#define BOID_SEPARATION 50.0f
#define SLOW_DOWN_DISTANCE 100.0f
#define BOID_SEP_WEIGHT 1.0f
#define BOID_ALI_WEIGHT 0.5f
#define BOID_COH_WEIGHT 0.8f
#define BOID_NEIGHBOUR_DIST 120.0f
#define BOID_IS_HIT_ERROR 5.0f;

class Boid {
public:
	Boid();
	Boid(int x, int y);
    Boid(int x, int y, ofImage &texture);
    
	void update(vector<Boid> &boids);
	void draw();
	
    void seek(ofVec2f target);
    void avoid(ofVec2f target);
    void arrive(ofVec2f target);
	
    void flock(vector<Boid> &boids);
    bool isHit(int x,int y, int radius);
    
	ofVec2f steer(ofVec2f target, bool slowdown);
    
	ofVec2f separate(vector<Boid> &boids);
	ofVec2f align(vector<Boid> &boids);
	ofVec2f cohesion(vector<Boid> &boids);
	
	ofVec2f loc,vel,acc;
    
	float r;
	float maxforce;
	float maxspeed;
    
    ofImage texture;

};

#endif