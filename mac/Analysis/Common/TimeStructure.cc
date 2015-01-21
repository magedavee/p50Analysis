#include "TimeStructure.hh"

void HitCluster::tallyHits() {
    if(!myHits.size()) return;
    for(auto it = myHits.begin(); it != myHits.end(); it++) {
        if(it->PID == IONI_HIT) Eioni += it->Equench();
        hitTypes[HitTypeID(it->PID)]++;
    }
    t_median = myHits[myHits.size()/2].t;       // assumes myHits is already time-ordered
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

void TimeStructureAnalyzer::classifyHits() {
    // merge ionization tracks (all particles) into scintillator hits
    vector<IoniCluster> scintHits;
    map<Int_t, double> volIoni = mergeIoniHits(ionc->clusts, scintHits, prompt_timescale);
    
    // classify hits
    for(auto its = scintHits.begin(); its != scintHits.end(); its++) {
        its->PID = DEAD_HIT;
        if(its->vol < 0) continue; // dead volume hit
        
        double Eq = its->Equench(); // quenched energy estimate
        
        if(its->vol >= 1000) { // muon veto hit
            if(Eq > veto_thresh) its->PID = VETO_HIT;
            continue;
        } 
       
        // fake PSD from dE/dx. TODO implement more sophisticated PSD model
        double psd = 1./(its->EdEdx/its->E);
        double psd_cut = 0.1;
        
        if(ncapt_Emin < Eq && Eq < ncapt_Emax && psd < psd_cut) its->PID = NCAPT_HIT;
        else if(0.1 < Eq && psd < psd_cut) its->PID = RECOIL_HIT;
        else if(trig_thresh < its->Equench() && psd_cut < psd) its->PID = IONI_HIT;
    }
    
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
