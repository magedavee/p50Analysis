#include "Event.hh"
#include <cassert>
#include <iostream>

#include <TClonesArray.h>

ClassImp(ParticleVertex)
ClassImp(ParticleEvent)
ClassImp(IoniCluster)
ClassImp(IoniClusterEvent)
ClassImp(NCapt)
ClassImp(NCaptEvent)
ClassImp(Event)

////////////////////////////////////////////
//------------------------------------------
////////////////////////////////////////////

ParticleEvent::~ParticleEvent() {
    if(particles) delete particles;
}

void ParticleEvent::Clear(Option_t*) {
    if(!particles) { particles = new TClonesArray("ParticleVertex",100); }
    particles->Clear("C");
    nParticles = 0;
}

void ParticleEvent::AddParticle(const ParticleVertex& P) {
    assert(particles);
    new((*particles)[nParticles++]) ParticleVertex(P);
}

////////////////////////////////////////////
//------------------------------------------
////////////////////////////////////////////

IoniClusterEvent::~IoniClusterEvent() {
    if(clusts) delete clusts;
}

void IoniClusterEvent::Clear(Option_t*) {
    if(!clusts) clusts = new TClonesArray("IoniCluster",1000);
    clusts->Clear("C");
    nIoniClusts = 0;
    EIoni = 0;
}

void IoniClusterEvent::AddIoniCluster(const IoniCluster& c) {
    assert(clusts);
    new((*clusts)[nIoniClusts++]) IoniCluster(c);
    EIoni += c.E;
}

////////////////////////////////////////////
//------------------------------------------
////////////////////////////////////////////

NCaptEvent::~NCaptEvent() {
    if(nCapts) delete nCapts;
}

void NCaptEvent::Clear(Option_t*) {
    if(!nCapts) nCapts = new TClonesArray("NCapt",100);
    nCapts->Clear("C");
    nNCapts = 0;
}

void NCaptEvent::AddNCapt(const NCapt& n) {
    assert(nCapts);
    new((*nCapts)[nNCapts++]) NCapt(n);
}

////////////////////////////////////////////
//------------------------------------------
////////////////////////////////////////////
