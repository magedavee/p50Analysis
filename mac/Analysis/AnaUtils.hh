#include "FileKeeper.hh"
#include "PathUtils.hh"
#include "Event.hh"
#include "XMLInfo.hh"

#include <iostream>
#include <sys/stat.h>

#include <TSystem.h>
#include <TClonesArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>

using std::cout;
using std::map;

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
