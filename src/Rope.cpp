//
//  Rope.cpp
//  emptyExample
//
//  Created by Alessandro Inguglia on 15/04/15.
//
//
#include "Rope.h"

// variables
vector<Particle> ps;
vector<Spring> sp;
int W;
ofVec3f prevB;
ofPoint averageVelB[10];
ofVec3f VectorDiff;

Rope::Rope(ofVec3f _a , ofVec3f _b ,int _H, float _WireLength, ofPoint _Forces,float _Gravity, bool _mesh)
{
    W = 1;
    H=_H;
    a = _a;
    b = _b;
    WireLength=_WireLength;
    Forces = _Forces;
    ForceX = Forces.x;
    ForceY = Forces.y;
    ForceZ = Forces.z;
    Gravity = _Gravity;
    meshSize = WireLength/(float)H;
    smoothMoveA = 1;
    smoothMoveB = 1;
    meshRender = _mesh;
    CtrlPointsVisible = 1;
    
    float diffX= (b.x-a.x)/(float)H;
    float diffY= (b.y-a.y)/H;//meshSize;
    float diffZ= (b.z-a.z)/(float)H;
    VectorDiff = b-a;
    float VecLength = VectorDiff.length();
    prevB = b;
    
    //Rope Body
    for (int i=0; i<H; i++) {
        float y = int(i) * meshSize;
        ofPoint newPPoint = ofPoint (a.x+diffX*i,y,a.z+diffZ*i);
        //Particle p(newPPoint, 1, .96);
        //|| i == H-1
        Particle p(newPPoint, i==0||i == H-1 ? 50 : 0.2, 0.96);
        ps.push_back(p);
    }
    //projected particle
    //Particle b(ofPoint(a.x+diffX*H,H*meshSize,a.z+diffZ*H), 0, .96);
    //ps.push_back(b);
    
    //SPRING INIT
    for (int i=0; i<H; i++) {
        //vertical structural springs
        if (i<H-1) {
            sp.push_back(Spring(&ps[i],&ps[i+1]));
            //sp.push_back(Spring(&ps[i],&ps[H-1]));
        }
        //bending springs vertical
        if (i<H-3) {
            sp.push_back(Spring(&ps[i],&ps[i+2]));
        }
        //sp.push_back(Spring(&ps[0],&ps[H-1]));
    }
    
    if (meshRender)
    {
        meshSetup();
        ropeMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    }
    else
    {
        lineaSbranella2.setMode(ofPath::POLYLINES);
    }
}
void Rope::update()
{
    //VectorDiff = ps[H-1].pos-ps[H-2].pos;//(b-a)/meshSize;
    
    for (int i=0; i<ps.size(); i++)
    {
        //gravity
        ps[i].addForce(ofVec3f(0,Gravity,0));
        //sine animation
        //ps[i].addForce(ofVec3f(0,0,0.3f*sin(ofGetElapsedTimef() * 1)));
        
        //forces control
        ps[i].addForce(ofVec3f(ForceX,ForceY,ForceZ));
        ps[i].addForce(ofVec3f(Forces.x,Forces.x,Forces.z));
    }
    
    //update springs
    for (int i=0; i<sp.size(); i++)
    {
        sp[i].update();
    }
    
    //update particles
    for (int i=0; i<ps.size(); i++)
    {
        ps[i].update();
    }
    
    
    // RENDER ROPES
    if (!meshRender)
    {
        if (CtrlPointsVisible)
        {
            //_POLYLINE_ROPE_//
            //Make first anchor Point Visible
            lineaSbranella.curveTo(ofPoint(ps[0].pos.x, ps[0].pos.y, ps[0].pos.z),15);
            for (int i=0; i<ps.size(); i++)
            {
                ofPoint thisPoint = ofPoint(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z);
                lineaSbranella.curveTo(thisPoint,15);
            }
            lineaSbranella.curveTo(ofPoint(ps[H-1].pos.x, ps[H-1].pos.y, ps[H-1].pos.z),15);
        }
        else
        {
            lineaSbranella.curveTo(ofPoint(ps[1].pos.x, ps[1].pos.y, ps[1].pos.z),15);
            for (int i=1; i<ps.size()-1; i++)
            {
                ofPoint thisPoint = ofPoint(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z);
                lineaSbranella.curveTo(thisPoint,15);
            }
            lineaSbranella.curveTo(ofPoint(ps[H-2].pos.x, ps[H-2].pos.y, ps[H-2].pos.z),15);
        }
    }
    else
    {
        if (CtrlPointsVisible)
        {
            for (int i=0; i<ropeMesh.getNumVertices(); i++)
            {
                ropeMesh.setVertex(i,ofPoint(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z));
            }
        }
        else
        {
            for (int i=0; i<ropeMesh.getNumVertices(); i++)
            {
                ropeMesh.setVertex(i,ofPoint(ps[i+1].pos.x, ps[i+1].pos.y, ps[i+1].pos.z));
            }
        }
    }
    //Assign Rope Start and End Point and move with linear interpolation
    ps[0].pos.interpolate(a, smoothMoveA);
    ps[H-1].pos.interpolate(b, smoothMoveB);
}

void Rope::draw()
{
    //POLYLINE DRAW
    if (meshRender)
    {
        ropeMesh.draw();
    }
    else
    {
        lineaSbranella.draw();
        lineaSbranella.clear();
    }
}

void Rope::moveATo(ofPoint dest, float smooth)
{
    //a.interpolate(dest, 0.1f);
}

void Rope::moveBTo(ofPoint dest, float smooth)
{
    //b.interpolate(dest, 0.1f);
}

void Rope::meshSetup() {
    
    for (int i=0;  i<H; i++)
    {
        if ((i==0||i==H-1)&&CtrlPointsVisible)
        {
            ropeMesh.addVertex(ofPoint(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z));
            ropeMesh.addColor(ofColor(0, 255, 255));
        }
        else if ((i==0||i==H-1)&&!CtrlPointsVisible)
        {
            //don't render control points
            //ropeMesh.addVertex(ofPoint(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z));
            //ropeMesh.addColor(ofColor(255, 255, 255));
        }
        else
        {
            ropeMesh.addVertex(ofPoint(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z));
            ropeMesh.addColor(ofColor(255, 255, 255));
        }
    }
}