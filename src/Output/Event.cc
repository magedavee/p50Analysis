#include "Event.hh"

ClassImp(EventPrimaryPtcl)
ClassImp(EventIoniCluster)
ClassImp(EventNCapt)
ClassImp(Event)
ClassImp(Run)

////////////////////////////////////////////
//------------------------------------------
////////////////////////////////////////////

Event::~Event() {
    if(iEvts) delete iEvts;
    if(Primaries) delete Primaries;
}

void Event::Clear(Option_t*) {
    
    if(!Primaries) Primaries = new TClonesArray("EventPrimaryPtcl",100);
    if(!iEvts) iEvts = new TClonesArray("EventIoniCluster",1000);
    if(!nCapts) nCapts = new TClonesArray("EventNCapt",100);
    
    N = 0;
    
    // Clear event arrays
    // "C" option recursively calls "Clear" on sub-objects
    
    Primaries->Clear("C");
    nPrimaries = 0;
    
    iEvts->Clear("C");
    nIoniClusts = 0;
    EIoni = 0;
    
    nCapts->Clear("C");
    nNCapts = 0;
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

void Event::AddNCapt(const EventNCapt& n) {
    assert(nCapts);
    new((*nCapts)[nNCapts++]) EventNCapt(n);
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
        
        
        
        
        