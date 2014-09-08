#include "FileKeeper.hh"
#include "PathUtils.hh"
#include "Event.hh"
#include "XMLInfo.hh"

#include <iostream>
#include <sys/stat.h>
#include <math.h>

#include <TSystem.h>
#include <TClonesArray.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>

using std::cout;
using std::map;
using std::string;

/// ionization localized to one segment, 0.2-20MeV
bool isIsolatedSegment(const map<Int_t,double>& volIoni) {
    int nThresh = 0;
    int nHi = 0;
    for(auto it = volIoni.begin(); it != volIoni.end(); it++) {
        nThresh += it->second > 0.2;
        nHi += it->second > 20;
    }
    return (nThresh == 1 && nHi == 0);
}

/// Scale histogram by geometric bin center (for Lethargy plot)
void scale_times_bin(TH1* f) {
    for(int i=1; i<f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = sqrt(A->GetBinLowEdge(i)*A->GetBinUpEdge(i));
        f->SetBinContent(i, f->GetBinContent(i)*scale);
    }
}

/// Divide out histogram bin width, for differential spectrum
void normalize_to_bin_width(TH1* f) {
    for(int i=1; i<f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = 1./A->GetBinWidth(i);
        f->SetBinContent(i, f->GetBinContent(i)*scale);
    }
}

/// Logarithmically-binned histogram
TH1F* logHist(const string& nm, const string& descrip, unsigned int nbins, double bmin, double bmax) {
    vector<Double_t> binEdges(nbins+1);
    for(unsigned int i=0; i<=nbins; i++)
        binEdges[i] = exp((nbins-i)*log(bmin)/nbins + i*log(bmax)/nbins);
    return new TH1F(nm.c_str(), descrip.c_str(), nbins, &binEdges[0]);
}
