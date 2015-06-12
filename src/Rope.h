

#ifndef _OF_ROPE
#define _OF_ROPE

#include "ofMain.h"
#include "Spring.h"


class Rope {
    
public:
    // methods
    void update();
    void draw();
    void moveATo(ofPoint dest, float smooth);
    void moveBTo(ofPoint dest, float smooth);
    void meshSetup();
    int getWireLength();
    
    //constructor
    Rope(ofVec3f a, ofVec3f b ,int H = 15, float stretchFactor = 1.0f, ofPoint Forces = ofPoint(0,0,0),float Gravity = 0.08f,bool mesh = false); //, float ForceX = 0, float ForceY = 0.01f, float ForceZ=0);
    
    Rope(ofVec3f a, ofVec3f b ,int H = 15, int hNodesDistance = 10 , ofPoint Forces = ofPoint(0,0,0),float Gravity = 0.08f,bool mesh = false);
    
    Rope(ofVec3f a, ofVec3f b ,int H = 15, ofPoint Forces = ofPoint(0,0,0),int WireLength = 100,float Gravity = 0.08f,bool mesh = false);
    
    ~Rope();
    
    // variables
    vector<Particle> ps;
    vector<Spring> sp;
    ofPolyline lineaSbranella;
    ofPath lineaSbranella2;
    ofMesh ropeMesh;
    bool  meshRender;
    bool AttachedA = true;
    bool AttachedB = true;
    
    int W; //Grid size
    int H;//Mesh Resolution

    ofVec3f a;
    ofVec3f b;
    
    float meshSize;
    int hNodesDistance;
    float Gravity;
    ofPoint Forces;
    float ForceX;
    float ForceY;
    float ForceZ;
    float smoothMoveA;
    float smoothMoveB;
    bool CtrlPointsVisible;
    
private:
    int WireLength;
    float stretchFactor;
    
};
#endif