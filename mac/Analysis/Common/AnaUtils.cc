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
        f->SetBinError(i, f->GetBinError(i)*scale);
    }
}

void normalize_to_bin_width(TH1* f) {
    for(int i=1; i<f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = 1./A->GetBinWidth(i);
        f->SetBinContent(i, f->GetBinContent(i)*scale);
        f->SetBinError(i, f->GetBinError(i)*scale);
    }
}

TH1F* logHist(const string& nm, const string& descrip, unsigned int nbins, double bmin, double bmax) {
    vector<Double_t> binEdges(nbins+1);
    for(unsigned int i=0; i<=nbins; i++)
        binEdges[i] = exp((nbins-i)*log(bmin)/nbins + i*log(bmax)/nbins);
    return new TH1F(nm.c_str(), descrip.c_str(), nbins, &binEdges[0]);
}

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

void fill_interp(TH1* h, double x, double w) {
    TAxis* Ax = h->GetXaxis();
    int b0 = Ax->FindBin(x);
    double c0 = Ax->GetBinCenter(b0);
    int b1 = x > c0? b0+1 : b0-1;
    double c1 = Ax->GetBinCenter(b1);
    double a = (c1-x)/(c1-c0);
    h->Fill(c0, a*w);
    h->Fill(c1, (1-a)*w);
}

/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////


ProfileHistos::ProfileHistos(int nbins, double r, const string& nm, const string& ttl, const string& u) {
    h_xy = new TH2F((nm+"_xy").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
    h_xy->GetXaxis()->SetTitle(("x position "+u).c_str());
    h_xy->GetYaxis()->SetTitle(("y position "+u).c_str());
    h[0] = h_xy;
        
    h_xz = new TH2F((nm+"_xz").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
    h_xz->GetXaxis()->SetTitle(("x position "+u).c_str());
    h_xz->GetYaxis()->SetTitle(("z position "+u).c_str());
    h[1] = h_xz;
    
    h_yz = new TH2F((nm+"_yz").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
    h_yz->GetXaxis()->SetTitle(("y position "+u).c_str());
    h_yz->GetYaxis()->SetTitle(("z position "+u).c_str());
    h[2] = h_yz;
    
    for(int i=0; i<3; i++) {
        h[i]->GetYaxis()->SetTitleOffset(1.4);
        hProf[i] = NULL;
    }
}

void ProfileHistos::Fill(double x, double y, double z, double w) {
    h_xy->Fill(x,y,w);
    h_xz->Fill(x,z,w);
    h_yz->Fill(y,z,w);
}

void ProfileHistos::Print(const char* opt, const string& bpath) {
    h_xy->Draw(opt);
    gPad->Print((bpath+"_xy.pdf").c_str());
    h_xz->Draw(opt);
    gPad->Print((bpath+"_xz.pdf").c_str());
    h_yz->Draw(opt);
    gPad->Print((bpath+"_yz.pdf").c_str());
}

void ProfileHistos::makeProf() {
    hProf[0] = h_xy->ProjectionX();
    hProf[1] = h_yz->ProjectionX();
    hProf[2] = h_xz->ProjectionY();
    for(int i=0; i<3; i++) {
        hProf[i]->Scale(1./hProf[i]->GetXaxis()->GetBinWidth(1));
        hProf[i]->SetLineColor(2+i);
    }
}

  