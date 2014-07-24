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
    if(iEvts) delete iEvts;
    if(Primaries) delete Primaries;
}

void Event::Clear(Option_t*) {
    
    if(!iEvts) iEvts = new TClonesArray("EventIoniCluster",1000);
    if(!Primaries) Primaries = new TClonesArray("EventPrimaryPtcl",100);
    
    N = 0;
    
    // "C" option recursively calls "Clear" on sub-objects
    Primaries->Clear("C");
    nPrimaries = 0;
    
    iEvts->Clear("C");
    nIoniClusts = 0;
    EIoni = 0;
}

void Event::AddPrimary(const EventPrimaryPtcl& P) {
    assert(Primaries);
    new((*Primaries)[nPrimaries++]) EventPrimaryPtcl(P);
}

void Event::AddIoniCluster(const EventIoniCluster& tr) {
    assert(iEvts);
    new((*iEvts)[nIoniClusts++]) EventIoniCluster(tr);
    EIoni += tr.E;
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
        
        
        
        
        