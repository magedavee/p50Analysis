//---------------------------------------------------
#include <iostream>
#include "TDirectory.h"
#include "TProcessID.h"
#include <vector>

#include "Event.hh"
using namespace std;

ClassImp(EventIoniCluster)
ClassImp(EventPrimaryPtcl)
ClassImp(Event)
ClassImp(Run)

Event::~Event() {
    if(myIoniClusts) delete myIoniClusts;
    if(myPrimaries) delete myPrimaries;
}

void Event::Clear(Option_t*) {
    
    if(!myIoniClusts) myIoniClusts = new TClonesArray("EventIoniCluster",1000);
    if(!myPrimaries) myPrimaries = new TClonesArray("EventPrimaryPtcl",100);
    
    N = 0;
    
    // "C" option recursively calls "Clear" on sub-objects
    myPrimaries->Clear("C");
    nPrimaries = 0;
    
    myIoniClusts->Clear("C");
    nIoniClusts = 0;
}

////////////////////////////////////////////
//------------------------------------------
////////////////////////////////////////////

void Run::Clear(Option_t*) {
    AirGap=0;
    WrapGap=0;
    WrapThickness=0;	
    AcrylThickness=0;        
    SegBuffer=0;
    ScintLength=0;
    ScintHeight=0;
    ScintWidth=0;
    SegLength=0;
    SegWidth=0;
    SegHeight=0;	
    PMTscale=0;               
    ShieldLead=0;
    ShieldPolyB=0;
    ShieldPolyLi=0;
    Vertical=0;
    Scint=0;
    Shielded=0;
    Reflectivity=0;
}
        
        
        
        
        