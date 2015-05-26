

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
    
    //constructor
    Rope(ofPoint a = ofPoint(300,200,0), ofPoint b = ofPoint(500,200,0),int H = 15, float WireLength = 100.0f, ofPoint Forces = ofPoint(0,0,0),float Gravity = 0.08f,bool mesh = false); //, float ForceX = 0, float ForceY = 0.01f, float ForceZ=0);
    
    // variables
    vector<Particle> ps;
    vector<Spring> sp;
    ofPolyline lineaSbranella;
    ofPath lineaSbranella2;
    ofMesh ropeMesh;
    bool  meshRender;
    
    int W; //Grid size
    int H;//Mesh Resolution
    ofPoint a;
    ofPoint b;
    float WireLength;
    float meshSize;
    float Gravity;
    ofPoint Forces;
    float ForceX;
    float ForceY;
    float ForceZ;
    float smoothMoveA;
    float smoothMoveB;
    bool CtrlPointsVisible;
    
private:

};
#endif 