#include "TimeStructure.hh"

void HitCluster::tallyHits() {
    if(!myHits.size()) return;
    for(auto it = myHits.begin(); it != myHits.end(); it++) {
        if(it->PID == IONI_HIT) Eioni += it->Equench();
        hitTypes[HitTypeID(it->PID)].push_back(*it);
    }
    t_median = myHits[myHits.size()/2].t;       // assumes myHits is already time-ordered
}

int HitCluster::isIoni() const {
    if(hitTypes.size() != 1) return 0;
    auto it = hitTypes.find(IONI_HIT);
    return (it==hitTypes.end())? 0 : it->second.size();
}

bool CoincidenceEvent::addHit(IoniCluster h, double prompt_timescale, double delayed_timescale) {
    if(!promptClusters.size() || h.t - promptClusters.back().myHits.back().t < prompt_timescale) {
        if(!promptClusters.size()) promptClusters.emplace_back();
        promptClusters.back().myHits.push_back(h);
        return true;
    }
    promptClusters.back().tallyHits();
    if(h.t - promptClusters.back().t_median < delayed_timescale) {
        promptClusters.emplace_back();
        promptClusters.back().myHits.push_back(h);
        return true;
    }
    return false;
}

void CoincidenceEvent::finalize() {
    if(!promptClusters.size()) return;
    promptClusters.back().tallyHits();
}

vector< pair<HitCluster,HitCluster> > CoincidenceEvent::getIBDPairs() const {
    vector< pair<HitCluster,HitCluster> > v;
    for(auto itn = promptClusters.begin(); itn != promptClusters.end(); itn++)
        if(itn->isNcapt())
            for(auto ite = promptClusters.begin(); ite != promptClusters.end(); ite++)
                if(ite->isIoni()) v.emplace_back(*ite,*itn);
    return v;
}

map<HitTypeID, vector<IoniCluster> > CoincidenceEvent::getHitTypes() const {
    map<HitTypeID, vector<IoniCluster> > m;
    for(auto itc = promptClusters.begin(); itc != promptClusters.end(); itc++) {
        for(auto itt = itc->hitTypes.begin(); itt != itc->hitTypes.end(); itt++) {
            vector<IoniCluster>& v = m[itt->first];
            v.insert( v.end(), itt->second.begin(), itt->second.end() );
        }
    }
    return m;
}

HitTypeID TimeStructureAnalyzer::classifyHit(const IoniCluster& h) const {
    if(h.vol < 0) return DEAD_HIT;
    double Eq = h.Equench(); // quenched energy estimate
    if(h.vol >= 1000) return Eq > veto_thresh? VETO_HIT : DEAD_HIT;
    
    // fake PSD from dE/dx. TODO: implement full response function
    double psd = h.E/h.EdEdx;
    double psd_cut = 0.1;
    
    if(ncapt_Emin < Eq && Eq < ncapt_Emax && psd < psd_cut) return NCAPT_HIT;
    else if(trig_thresh < Eq && psd < psd_cut) return RECOIL_HIT;
    return trig_thresh < Eq ? IONI_HIT : DEAD_HIT;
}

void TimeStructureAnalyzer::classifyHits() {
    // merge ionization tracks (all particles) into scintillator hits
    vector<IoniCluster> scintHits;
    map<Int_t, double> volIoni = mergeIoniHits(ionc->clusts, scintHits, prompt_timescale);
    
    // classify hits
    for(auto its = scintHits.begin(); its != scintHits.end(); its++) its->PID = classifyHit(*its);   
    
    // group hits into prompt clusters
    coincs.clear();
    for(auto its = scintHits.begin(); its != scintHits.end(); its++) {
        if(its->PID == DEAD_HIT) continue; // ignore dead volume hits
        if(!coincs.size()) coincs.emplace_back();
        if(coincs.back().addHit(*its, prompt_timescale, delayed_timescale)) continue;
        coincs.emplace_back();
        coincs.back().addHit(*its, prompt_timescale, delayed_timescale);
    }
    if(coincs.size()) coincs.back().finalize();
}
