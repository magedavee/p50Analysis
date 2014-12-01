#ifndef GOLDHAGENSPECTRUM_HH
#define GOLDHAGENSPECTRUM_HH

#include "PathUtils.hh"
#include <string>
#include <TH1F.h>

using std::string;

/// Utility for loading Goldhagen ground-level neutron spectra histogram
TH1* loadGoldhagen(const string& fname = getEnvSafe("PG4_AUX")+"/BGSpectra/Goldhagen_Watson_Roof.txt");

#endif
