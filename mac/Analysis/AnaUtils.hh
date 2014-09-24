#include "FileKeeper.hh"
#include "PathUtils.hh"
#include "Event.hh"
#include "XMLInfo.hh"

#include <iostream>
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
#include <Rtypes.h>

using std::cout;
using std::map;
using std::string;

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
