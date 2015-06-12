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
ofVec3f VectorDiff;

Rope::Rope(ofVec3f _a , ofVec3f _b ,int _H, float _stretchFactor, ofPoint _Forces,float _Gravity, bool _mesh)
{
    W = 1;
    H=_H;
    a = _a;
    b = _b;
    //ACTUAL LENGTH OF THE WIRE
    WireLength=_stretchFactor*a.distance(b);
    Forces = _Forces;
    ForceX = Forces.x;
    ForceY = Forces.y;
    ForceZ = Forces.z;
    Gravity = _Gravity;
    meshSize = WireLength/(float)H;  // WireLength/(float)H;
    smoothMoveA = 1;
    smoothMoveB = 1;
    meshRender = _mesh;
    CtrlPointsVisible = 1;
    prevB = b;

    cout<<"DistanceAB: "<<a.distance(b)<<endl;
    cout<<"stretchedWire(wireLength): "<<WireLength<<endl;
    cout<<"messhSize: "<<meshSize<<endl;
    if (meshSize>=1)
    {
        cout<<"scaling\n"<<endl;
    }
    else
    {
        cout<<"not scaling\n"<<endl;
    }

    //cout<<a.distance(ps[H-1].pos)<<endl;

    ofPoint newPPoint;
    //Rope Body
    for (int i=0; i<H; i++) {
        float y = float(i) * meshSize;
        if (meshSize>=1)
        {
            newPPoint= ofPoint (0,meshSize*float(i+1),0);
        }
        else
        {
            newPPoint = ofPoint (0,i+1,0);
        }
        //Particle p(newPPoint, 1, .96);
        //|| i == H-1
        Particle p(newPPoint, i==0||i == H-1 ? 15000 : 1, 0.96);
        ps.push_back(p);
    }
    
    //SPRING INIT
    for (int i=0; i<H; i++) {
        //vertical structural springs
        if (i<H-1) {
            sp.push_back(Spring(&ps[i],&ps[i+1]));
        }
        //bending springs vertical
        if (i<H-3) {
            sp.push_back(Spring(&ps[i],&ps[i+2]));
        }
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

Rope::Rope(ofVec3f _a , ofVec3f _b ,int _H, int _hNodesDistance , ofPoint _Forces,float _Gravity, bool _mesh)
{
    W = 1;
    H=_H;
    a = _a;
    b = _b;
    //ACTUAL LENGTH OF THE WIRE
    WireLength=a.distance(b);
    hNodesDistance = _hNodesDistance;
    
    Forces = _Forces;
    ForceX = Forces.x;
    ForceY = Forces.y;
    ForceZ = Forces.z;
    Gravity = _Gravity;
    meshSize = roundf(WireLength/(float)H);
    smoothMoveA = 1;
    smoothMoveB = 1;
    meshRender = _mesh;
    CtrlPointsVisible = 1;
    prevB = b;
    
    cout<<"DistanceAB: "<<a.distance(b)<<endl;
    cout<<"wireLength(a->b distance): "<<WireLength<<endl;
    cout<<"Hdistance: "<<hNodesDistance<<endl;
    if (meshSize>=1)
    {
        cout<<"scaling\n"<<endl;
    }
    else
    {
        cout<<"not scaling\n"<<endl;
    }
    
    //cout<<a.distance(ps[H-1].pos)<<endl;
    
    ofPoint newPPoint;
    //Rope Body
    for (int i=0; i<H; i++) {
        float y = float(i) * meshSize;
        
            newPPoint= ofPoint (0,hNodesDistance*(i+1),0);
        //Particle p(newPPoint, 1, .96);
        //|| i == H-1
        Particle p(newPPoint, i==0||i == H-1 ? 15000 : 1, 0.96);
        ps.push_back(p);
    }
    
    //SPRING INIT
    for (int i=0; i<H; i++) {
        //vertical structural springs
        if (i<H-1) {
            sp.push_back(Spring(&ps[i],&ps[i+1]));
        }
        //bending springs vertical
        if (i<H-3) {
            sp.push_back(Spring(&ps[i],&ps[i+2]));
        }
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

Rope::Rope(ofVec3f _a , ofVec3f _b , int _H,ofPoint _Forces,int _WireLength,float _Gravity, bool _mesh)
{
    W = 1;
    a = _a;
    b = _b;
    //ACTUAL LENGTH OF THE WIRE
    WireLength=_WireLength;
    H  = _H;
    Forces = _Forces;
    ForceX = Forces.x;
    ForceY = Forces.y;
    ForceZ = Forces.z;
    Gravity = _Gravity;
    meshSize = roundf(WireLength/(float)H);
    smoothMoveA = 1;
    smoothMoveB = 1;
    meshRender = _mesh;
    CtrlPointsVisible = 1;
    prevB = b;
    
    ofPoint newPPoint;
    //Rope Body
    
    for (int i=0; i<H; i++)
    {
        newPPoint= ofPoint (0,hNodesDistance*(i+1),0);
        Particle p(newPPoint, i==0||i == H-1 ? 15000 : 1, 0.96);
        ps.push_back(p);
    }
    
    //SPRING INIT
    for (int i=0; i<H; i++) {
        //vertical structural springs
        if (i<H-1) {
            sp.push_back(Spring(&ps[i],&ps[i+1]));
        }
        //bending springs vertical
        if (i<H-3) {
            sp.push_back(Spring(&ps[i],&ps[i+2]));
        }
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
    for (int i=0; i<ps.size(); i++)
    {
        //gravity
        ps[i].addForce(ofVec3f(0,Gravity,0));
        //sine animation
        //ps[i].addForce(ofVec3f(sin(ofGetElapsedTimef() * 100),0,0));
        
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
            lineaSbranella.curveTo(ofPoint(ps[0].pos.x, ps[0].pos.y, ps[0].pos.z),25);
            for (int i=0; i<ps.size(); i++)
            {
                ofPoint thisPoint = ofPoint(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z);
                lineaSbranella.curveTo(thisPoint,15);
            }
            lineaSbranella.curveTo(ofPoint(ps[H-1].pos.x, ps[H-1].pos.y, ps[H-1].pos.z),25);
        }
        else
        {
            lineaSbranella.curveTo(ofPoint(ps[1].pos.x, ps[1].pos.y, ps[1].pos.z),25);
            for (int i=1; i<ps.size()-1; i++)
            {
                ofPoint thisPoint = ofPoint(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z);
                lineaSbranella.curveTo(thisPoint,15);
            }
            lineaSbranella.curveTo(ofPoint(ps[H-2].pos.x, ps[H-2].pos.y, ps[H-2].pos.z),25);
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
    if (AttachedA)
    {
        ps[0].pos.interpolate(a, smoothMoveA);
    }
    if (AttachedB)
    {
        ps[H-1].pos.interpolate(b, smoothMoveB);
    }
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
int Rope::getWireLength()
{
    return WireLength;
}
void Rope::moveATo(ofPoint dest, float smooth)
{
    //a.interpolate(dest, 0.1f);
}

void Rope::moveBTo(ofPoint dest, float smooth)
{
    //b.interpolate(dest, 0.1f);
}

