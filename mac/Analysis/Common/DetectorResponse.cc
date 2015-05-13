#include "DetectorResponse.hh"
#include <cmath>
#include <cassert>

void DetectorResponse::addIoni(const s_IoniCluster& h) {
    if(!h.E) return; // skip zero-energy neutrino tags
    TimedIoniCluster* tc = new TimedIoniCluster(h);
    if(P20reflectorless && h.vol == 1) tc->vol = 0;
    addSingle(tc);
}

double DetectorResponse::calcQuench(const s_IoniCluster& evt) const {
    const double c_1 = 0.1049;
    const double c_2 = -8.72117e-05;
    return evt.E / (1 + c_1*evt.EdEdx/evt.E + c_2*evt.EdEdx2/evt.E);
}

double DetectorResponse::psd_f(const s_IoniCluster& evt) const {
    return atan(psd_a*evt.EdEdx/evt.E)*2/M_PI;
}

void DetectorResponse::quenchPSD(const s_IoniCluster& evt, double& Equench, double& PSD) const {
    const double u = (evt.PID==11? PSD_gamma0 : psd_f(evt));
    // PSD, scaling from raw variable u to data-like values
    // a = 0.06 -> .01, .89
    // a = 0.08 -> .02, .92
    // a = 0.10 -> .02, .935
    PSD = PSD_gamma + (u-PSD_gamma0)/(PSD_ncapt0-PSD_gamma0)*(PSD_ncapt - PSD_gamma);

    // interpolate between quenched and unquenched energy
    Equench = calcQuench(evt);
    const double u_upper = 0.25;
    const double u_lower = 0.1;
    if(u <= u_lower) Equench = evt.E;
    else if(u < u_upper) Equench = Equench*(u-u_lower)/(u_upper - u_lower) + evt.E*(u_upper-u)/(u_upper - u_lower);
}

s_PhysPulse DetectorResponse::genResponse(const s_IoniCluster& evt) const {
    s_PhysPulse p;
    double PSD, Equench;
    
    p.evt = evt.evt;
    p.seg = evt.vol;
    quenchPSD(evt,Equench,PSD);
    p.E = Equench;
    assert(evt.t == evt.t); // NaN check
    p.t = evt.t;
    p.y = evt.x[cellaxis];
    p.PSD = PSD;
    
    return p;
}

void DetectorResponse::processMid(TimedObject* O) {
    TimedCluster* C = dynamic_cast<TimedCluster*>(O);
    assert(C);
    
    /// sum hits grouped by volume and particle ID
    map<int, map<int, s_IoniCluster> > volHits;
    for(auto it = C->objs().begin(); it != C->objs().end(); it++) {
        TimedIoniCluster* h = dynamic_cast<TimedIoniCluster*>(*it);
        assert(h);
        // special case: combine alpha, triton due to current quenching calculation. TODO: fix quenching calc so these are separate.
        if(h->PID == 1000020040) h->PID = 1000010030;
        
        map<int, s_IoniCluster>& m = volHits[h->vol];
        auto prevHit = m.find(h->PID);
        if(prevHit == m.end()) m.insert(std::pair<int,s_IoniCluster>(h->PID, *h));
        else prevHit->second += *h;
    }
    
    /// quench and merge hits in each volume
    double PSD, Equench;
    for(auto it = volHits.begin(); it != volHits.end(); it++) {
        s_IoniCluster volIoni;  // quenched-energy-summed response from different particles
        s_PhysPulse volPhys;    // detector physics response
        double EqPSD = 0;       // quenched-energy-weighted average PSD
        for(auto ith = it->second.begin(); ith != it->second.end(); ith++) {
            volPhys.evt = ith->second.evt;              // preserve MC event number (should be same for all *ith)
            quenchPSD(ith->second, Equench, PSD);       // calculate individual particle quenching/PSD
            EqPSD += Equench*PSD;
            ith->second.E = Equench;                    // replace energy with quenched energy for averaging different particles
            volIoni += ith->second;
        }
        if(!volIoni.E) continue;        // ignore zero-energy hits
        volPhys.seg = it->first;
        volPhys.E = volIoni.E;          // summed quenched energy
        volPhys.t = volIoni.t;
        volPhys.y = volIoni.x[cellaxis];
        volPhys.PSD = EqPSD/volIoni.E;
        event_response.push_back(volPhys);
    }
}
