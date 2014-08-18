#ifndef GOLDHAGENSPECTRUM_HH
#define GOLDHAGENSPECTRUM_HH

#include <string>
#include <TH1F.h>

using std::string;

/// Scale histogram by value at bin geometric mean center
void scale_times_bin(TH1* f);

/// Scale histogram times bin width
void normalize_to_bin_width(TH1* f);

/// Utility for loading Goldhagen ground-level neutron spectra histogram
TH1* loadGoldhagen(const string& fname = "/home/mpmendenhall/Documents/PROSPECT/Goldhagen_Watson_Roof.txt");

#endif
