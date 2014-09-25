#include "AnaUtils.hh"
#include <TArray.h>
#include <algorithm>

bool isIsolatedSegment(const map<Int_t,double>& volIoni) {
    int nThresh = 0;
    int nHi = 0;
    for(auto it = volIoni.begin(); it != volIoni.end(); it++) {
        nThresh += it->second > 0.2;
        nHi += it->second > 20;
    }
    return (nThresh == 1 && nHi == 0);
}

void scale_times_bin(TH1* f) {
    for(int i=1; i<f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = sqrt(A->GetBinLowEdge(i)*A->GetBinUpEdge(i));
        f->SetBinContent(i, f->GetBinContent(i)*scale);
    }
}

void normalize_to_bin_width(TH1* f) {
    for(int i=1; i<f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = 1./A->GetBinWidth(i);
        f->SetBinContent(i, f->GetBinContent(i)*scale);
    }
}

TH1F* logHist(const string& nm, const string& descrip, unsigned int nbins, double bmin, double bmax) {
    vector<Double_t> binEdges(nbins+1);
    for(unsigned int i=0; i<=nbins; i++)
        binEdges[i] = exp((nbins-i)*log(bmin)/nbins + i*log(bmax)/nbins);
    return new TH1F(nm.c_str(), descrip.c_str(), nbins, &binEdges[0]);
}

bool compare_hit_times(const IoniCluster& a, const IoniCluster& b) { return a.t < b.t; }

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
