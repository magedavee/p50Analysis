#include "Event.hh"
#include <cassert>
#include <iostream>
#include <cmath>

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

void IoniCluster::operator+=(const IoniCluster& r) {
    Double_t EE = E + r.E;
    dt = ((dt*dt+t*t)*E + (r.dt*r.dt+r.t*r.t)*r.E)/EE;
    t = (t*E+r.t*r.E)/EE;
    dt = sqrt(dt - t*t);
    l += r.l;
    for(unsigned int i=0; i<3; i++) {
        dx[i] = ((dx[i]*dx[i]+x[i]*x[i])*E + (r.dx[i]*r.dx[i]+r.x[i]*r.x[i])*r.E)/EE;
        x[i] = (x[i]*E+r.x[i]*r.E)/EE;
        dx[i] = sqrt(dx[i] - x[i]*x[i]);
    }
    E = EE;
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
