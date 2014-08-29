#include "Event.hh"
#include <cassert>

#include <TClonesArray.h>

ClassImp(ParticleVertex)
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
    if(!particles) particles = new TClonesArray("ParticleVertex",100);
    particles->Clear("C");
    nParticles = 0;
}

void ParticleEvent::AddParticle(const ParticleVertex& P) {
    assert(particles);
    new((*particles)[nParticles++]) ParticleVertex(P);
}


Event::~Event() {
    if(iEvts) delete iEvts;
    if(nCapts) delete nCapts;
}

void Event::Clear(Option_t*) {
    
    if(!iEvts) iEvts = new TClonesArray("EventIoniCluster",1000);
    if(!nCapts) nCapts = new TClonesArray("EventNCapt",100);
    
    N = 0;
    
    // Clear event arrays
    // "C" option recursively calls "Clear" on sub-objects
    
    iEvts->Clear("C");
    nIoniClusts = 0;
    EIoni = 0;
    
    nCapts->Clear("C");
    nNCapts = 0;
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
