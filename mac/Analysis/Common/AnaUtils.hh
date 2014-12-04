#include <string>
#include <map>
#include <iostream>
using std::cout;
using std::map;
using std::string;
using std::pair;

#include "FileKeeper.hh"
#include "PathUtils.hh"
#include "Event.hh"
#include "XMLInfo.hh"
#include "strutils.hh"

#include <fstream>
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
#include <TLegend.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TLatex.h>

/// ionization localized to one segment, 0.2-20MeV
bool isIsolatedSegment(const map<Int_t,double>& volIoni);

/// Scale histogram by geometric bin center (for Lethargy plot)
void scale_times_bin(TH1* f);

/// Divide out histogram bin width, for differential spectrum
void normalize_to_bin_width(TH1* f);

/// Logarithmically-binned histogram
TH1F* logHist(const string& nm, const string& descrip, unsigned int nbins, double bmin, double bmax);

/// merge ionization events into single history; return total in each volume
map<Int_t, double> mergeIoniHits(TClonesArray* clusts, vector<IoniCluster>& hitHist, double dt_max);

/// merge ionization events into single history, regardless of volume
vector<IoniCluster> mergeIoniHits(const vector<IoniCluster>& hts, double dt_max);

/// Fill histogram preserving *average* value interpolated between bins
void fill_interp(TH1* h, double x, double w = 1.0);

/// (x,y,z) position projection histograms
class ProfileHistos {
public:
    /// Constructor
    ProfileHistos(int nbins, double r, const string& nm, const string& ttl, const string& u);
    
    /// scale to bin size
    void ScaleBinsize() { for(int i=0; i<3; i++) h[i]->Scale(1./h[i]->GetXaxis()->GetBinWidth(1)/h[i]->GetYaxis()->GetBinWidth(1)); }
    /// scale histograms
    void Scale(double s) { for(int i=0; i<3; i++) h[i]->Scale(s); }
    /// set z range maximum
    void SetMaximum(double m) { for(int i=0; i<3; i++) h[i]->SetMaximum(m); }
    /// set z range minimum
    void SetMinimum(double m) { for(int i=0; i<3; i++) h[i]->SetMinimum(m); }
    
    /// fill data point
    void Fill(double x, double y, double z, double w=1.);
    
    /// print histograms to bpath+"_xy.pdf", etc.
    void Print(const char* opt, const string& bpath);
    
    /// construct 1-D profiles
    void makeProf();
    
    TH2F* h_xy;
    TH2F* h_xz;
    TH2F* h_yz;
    TH2F* h[3];
    TH1* hProf[3];
};