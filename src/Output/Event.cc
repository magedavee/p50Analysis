#include "Event.hh"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

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

Int_t NCapt::Compare(const TObject* obj) const {
    const NCapt* nc = dynamic_cast<const NCapt*>(obj);
    return t < nc->t ? -1 : t > nc->t ? 1 : 0;
}

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

bool compare_hit_times(const IoniCluster& a, const IoniCluster& b) { return a.t < b.t; }

vector<IoniCluster> mergeIoniHits(const vector<IoniCluster>& hts, double dt_max) {
    vector<IoniCluster> vOut;
    if(!hts.size()) return vOut;
    auto it = hts.begin();
    vOut.push_back(*(it++));
    for(; it != hts.end(); it++) {
        if(it->t > vOut.back().t + dt_max)
            vOut.push_back(*it);
        else
            vOut.back() += *it;
    }
    return vOut;
}

map<Int_t, double> mergeIoniHits(TClonesArray* clusts, vector<IoniCluster>& hitHist, double dt_max) {
    
    map<Int_t, double> volIoni;
    map<Int_t, vector<IoniCluster> > volClusts;
    Int_t nIoni = clusts->GetEntriesFast();
    for(Int_t i=0; i<nIoni; i++) {
        IoniCluster* ei = (IoniCluster*)clusts->At(i);
        Int_t V = ei->vol;
        volIoni[V] += ei->E;
        if(!volClusts.count(V)) volClusts[V].push_back(*ei);
        else {
            if(ei->t > volClusts[V].back().t + dt_max)
                volClusts[V].push_back(*ei);
            else
                volClusts[V].back() += *ei;
        }
    }
    
    // merge and sort
    for(auto it = volClusts.begin(); it != volClusts.end(); it++)
        hitHist.insert(hitHist.end(), it->second.begin(), it->second.end());
    std::sort(hitHist.begin(), hitHist.end(), compare_hit_times);
    
    return volIoni;
}

HitTypeID classifyHit(const IoniCluster& h) {
    if(h.vol < 0) return DEAD_HIT;
    if(h.vol >= 1000) return VETO_HIT;
    
    double Eq = h.Equench(); // quenched energy estimate
    
    // fake PSD from dE/dx.
    double psd = h.E/h.EdEdx;
    double psd_cut = 0.1;
    
    if(0.4 < Eq && Eq < 0.6 && psd < psd_cut) return NCAPT_HIT;
    else if(0.1 < Eq && psd < psd_cut) return RECOIL_HIT;
    return IONI_HIT;
}
