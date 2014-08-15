#include "Event.hh"
#include <cassert>

#include <TClonesArray.h>

ClassImp(EventPrimaryPtcl)
ClassImp(ParticleEvent)
ClassImp(EventIoniCluster)
ClassImp(EventNCapt)
ClassImp(Event)

////////////////////////////////////////////
//------------------------------------------
////////////////////////////////////////////

ParticleEvent::~ParticleEvent() {
    if(particles) delete particles;
}

void ParticleEvent::Clear(Option_t*) {
    if(!particles) particles = new TClonesArray("EventPrimaryPtcl",100);
    particles->Clear("C");
    nParticles = 0;
}

void ParticleEvent::AddParticle(const EventPrimaryPtcl& P) {
    assert(particles);
    new((*particles)[nParticles++]) EventPrimaryPtcl(P);
}

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
