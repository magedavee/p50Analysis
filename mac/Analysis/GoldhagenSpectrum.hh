#ifndef GOLDHAGENSPECTRUM_HH
#define GOLDHAGENSPECTRUM_HH

#include <string>
#include <TH1F.h>

using std::string;

/// Utility for loading Goldhagen ground-level neutron spectra histogram
TH1* loadGoldhagen(const string& fname = "/home/mpmendenhall/Documents/PROSPECT/Cosmic Background Papers/Goldhagen_Watson_Roof.txt");

#endif
