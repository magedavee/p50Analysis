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
    EdEdx += r.EdEdx;
    EdEdx2 += r.EdEdx2;
    E = EE;
}

Double_t IoniCluster::dxtot() const {
    return sqrt(dx[0]*dx[0]+dx[1]*dx[1]+dx[2]*dx[2]);
}

Double_t IoniCluster::Equench() const {
    const double c_1 = 0.1049;
    const double c_2 = -8.72117e-05;
    return E / (1 + c_1*EdEdx/E + c_2*EdEdx2/E);
    
    //const double c_1 = -0.0686308;
    //const double c_2 = 0.0077532;
    //const double c_3 = -6.6839e-05;
    //return E / (1 + c_1*EdEdx/E + c_2*EdEdx2/E + c_3*EdEdx/E*EdEdx2/E);
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
