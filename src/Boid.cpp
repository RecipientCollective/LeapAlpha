#include "Boid.h"

Boid::Boid()
{
    loc.set(ofRandomWidth(),ofRandomHeight());
	vel.set(0,0);
	acc.set(0,0);
    r = BOID_RADIUS;
    maxspeed = MAX_BOID_SPEED;
    maxforce = MAX_BOID_FORCE;
}

Boid::Boid(int x, int y)
{
    loc.set(x,y);
	vel.set(0,0);
	acc.set(0,0);
    r = BOID_RADIUS;
    maxspeed = MAX_BOID_SPEED;
    maxforce = MAX_BOID_FORCE;
}

Boid::Boid(int x, int y, ofImage &tex)
{
    loc.set(x,y);
    vel.set(0,0);
    acc.set(0,0);
    r = tex.getWidth();
    maxspeed = MAX_BOID_SPEED;
    maxforce = MAX_BOID_FORCE;
    texture = tex;
}

// Method to update location
void Boid::update(vector<Boid> &boids)
{
   
//    if (ofGetFrameNum() % 4 == 0)
//    {
        flock(boids);
//    }
	
    vel += acc;   // Update velocity
    vel.x = ofClamp(vel.x, -maxspeed, maxspeed);  // Limit speed
	vel.y = ofClamp(vel.y, -maxspeed, maxspeed);  // Limit speed
    loc += vel;
    acc.set(0,0);  // Reset accelertion to 0 each cycle
	
	if (loc.x < -r) loc.x = ofGetWidth()+r;
    if (loc.y < -r) loc.y = ofGetHeight()+r;

    if (loc.x > ofGetWidth()+r) loc.x = -r;
    if (loc.y > ofGetHeight()+r) loc.y = -r;
}

void Boid::seek(ofVec2f target)
{
    acc += steer(target, false);
}

void Boid::avoid(ofVec2f target)
{
    acc -= steer(target, false);
}

void Boid::arrive(ofVec2f target)
{
    acc += steer(target, true);
}

// A method that calculates a steering vector towards a target
// Takes a second argument, if true, it slows down as it approaches the target
ofVec2f Boid::steer(ofVec2f target, bool slowdown)
{
    ofVec2f steer;  // The steering vector
    ofVec2f desired = target - loc;  // A vector pointing from the location to the target
    
	float d = ofDist(target.x, target.y, loc.x, loc.y); // Distance from the target is the magnitude of the vector
	
	
	// If the distance is greater than 0, calc steering (otherwise return zero vector)
    if (d > 0)
    {
		
		desired /= d; // Normalize desired
		// Two options for desired vector magnitude (1 -- based on distance, 2 -- maxspeed)
		if ((slowdown) && (d < SLOW_DOWN_DISTANCE)) {
			desired *= maxspeed * (d/SLOW_DOWN_DISTANCE); // This damping is somewhat arbitrary
		} else {
			desired *= maxspeed;
		}
		// Steering = Desired minus Velocity
		steer = desired - vel;
		steer.x = ofClamp(steer.x, -maxforce, maxforce); // Limit to maximum steering force
		steer.y = ofClamp(steer.y, -maxforce, maxforce); 
		
    }
    
   return steer;
}

void Boid::draw()
{
    // Draw a triangle rotated in the direction of velocity
	float angle = (float)atan2(-vel.y, vel.x);
    float theta =  -1.0*angle;
	float heading2D = ofRadToDeg(theta)+90;
	
	ofPushStyle();
    ofFill();
    ofPushMatrix();
    ofTranslate(loc.x, loc.y);
    ofRotateZ(heading2D);
    if (texture.isAllocated())
    {
        texture.draw(0,0);
    }
    else
    {
        ofBeginShape();
        ofVertex(0, -r*2);
        ofVertex(-r, r*2);
        ofVertex(r, r*2);
        ofEndShape(true);
    }
    ofPopMatrix();
	ofPopStyle();
}

void Boid::flock(vector<Boid> &boids)
{
	ofVec2f sep = separate(boids);
	ofVec2f ali = align(boids);
	ofVec2f coh = cohesion(boids);
	
	// Arbitrarily weight these forces
	sep *= BOID_SEP_WEIGHT;
	ali *= BOID_ALI_WEIGHT;
	coh *= BOID_COH_WEIGHT;
	
	acc += sep + ali + coh;
}

/*
 postscript
 */
bool Boid::isHit(int x, int y, int radius)
{
    int r = 1;
    int range = BOID_IS_HIT_ERROR;//calculation error range
    int dist =r + radius - range;
    if(pow((x-loc.x),2)+pow((y-loc.y),2) < dist * dist) {
        return true;
    }
    return false;
}

// Separation
// Method checks for nearby boids and steers away
ofVec2f Boid::separate(vector<Boid> &boids)
{
    float desiredseparation = BOID_SEPARATION;
    ofVec2f steer;
    int count = 0;
	
    // For every boid in the system, check if it's too close
    for (int i = 0 ; i < boids.size(); i++)
    {
		Boid &other = boids[i];
		
		float d = ofDist(loc.x, loc.y, other.loc.x, other.loc.y);
		
		// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
		if ((d > 0) && (d < desiredseparation))
        {
			// Calculate vector pointing away from neighbor
			ofVec2f diff = loc - other.loc;
			diff /= d;			// normalize
			diff /= d;          // Weight by distance
			steer += diff;
			count++;            // Keep track of how many
		}
    }
    
    // Average -- divide by how many
    if (count > 0)
    {
		steer /= (float)count;
    }
	
	
    // As long as the vector is greater than 0
	//float mag = sqrt(steer.x*steer.x + steer.y*steer.y);
	
	float mag = sqrt(steer.x*steer.x + steer.y*steer.y);
    if (mag > 0)
    {
		// Implement Reynolds: Steering = Desired - Velocity
		steer /= mag;
		steer *= maxspeed;
		steer -= vel;
		steer.x = ofClamp(steer.x, -maxforce, maxforce);
		steer.y = ofClamp(steer.y, -maxforce, maxforce);
    }
    
    return steer;
}

// Alignment
// For every nearby boid in the system, calculate the average velocity
ofVec2f Boid::align(vector<Boid> &boids)
{
    float neighbordist = BOID_NEIGHBOUR_DIST;
    ofVec2f steer;
    int count = 0;
    for (int i = 0 ; i < boids.size(); i++)
    {
		Boid &other = boids[i];
		
		float d = ofDist(loc.x, loc.y, other.loc.x, other.loc.y);
		if ((d > 0) && (d < neighbordist))
        {
			steer += (other.vel);
			count++;
		}
    }
    
    if (count > 0)
    {
		steer /= (float)count;
    }
	
    // As long as the vector is greater than 0
	float mag = sqrt(steer.x*steer.x + steer.y*steer.y);
    
    if (mag > 0) {
		// Implement Reynolds: Steering = Desired - Velocity
		steer /= mag;
		steer *= maxspeed;
		steer -= vel;
		steer.x = ofClamp(steer.x, -maxforce, maxforce);
		steer.y = ofClamp(steer.y, -maxforce, maxforce);
    }
    return steer;
}

// Cohesion
// For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
ofVec2f Boid::cohesion(vector<Boid> &boids)
{
    float neighbordist = BOID_NEIGHBOUR_DIST;
    ofVec2f sum;   // Start with empty vector to accumulate all locations
    int count = 0;
    for (int i = 0 ; i < boids.size(); i++)
    {
		Boid &other = boids[i];
		float d = ofDist(loc.x, loc.y, other.loc.x, other.loc.y);
        
		if ((d > 0) && (d < neighbordist))
        {
			sum += other.loc; // Add location
			count++;
		}
    }
    
    if (count > 0)
    {
		sum /= (float)count;
		return steer(sum, false);  // Steer towards the location
    }
    return sum;
}
