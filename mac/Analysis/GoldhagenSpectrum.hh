#ifndef GOLDHAGENSPECTRUM_HH
#define GOLDHAGENSPECTRUM_HH

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include <TH1F.h>

using std::string;
using std::vector;

/// utility function to split a string into a list of doubles
vector<double> sToDoubles(const string& s, const string splitchars=" \t,") {
    vector<double> v;
    size_t p = 0;
    while(p<s.size()) {
        size_t wstart = s.find_first_not_of(splitchars,p);
        if(wstart == string::npos)
            break;
        p = s.find_first_of(splitchars,wstart);
        if(p == string::npos)
            p = s.size();
        v.push_back(atof(s.substr(wstart,p-wstart).c_str()));
    }
    return v;
}

/// Scale histogram by value at bin geometric mean center
void scale_times_bin(TH1* f) {
    for(int i=1; i<f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = sqrt(A->GetBinLowEdge(i)*A->GetBinUpEdge(i));
        f->SetBinContent(i, f->GetBinContent(i)*scale);
    }
}

/// Scale histogram times bin width
void normalize_to_bin_width(TH1* f) {
    for(int i=1; i<f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = 1./A->GetBinWidth(i);
        f->SetBinContent(i, f->GetBinContent(i)*scale);
    }
}

/// Utility for loading Goldhagen ground-level neutron spectra histogram
TH1* loadGoldhagen(const string& fname = "/home/mpmendenhall/Documents/PROSPECT/Goldhagen_Watson_Roof.txt") {
    
    std::ifstream infl(fname.c_str());
    std::vector<double> bins;
    std::vector<double> data;
    while (infl) {
        string s;
        if (!getline(infl, s)) break;
        std::vector<double> v = sToDoubles(s,"\t");
        if(v.size() != 10) {
            std::cerr << "Unreadable line in input file: '" << s << "' (" << v.size() << ")\n";
            for(int i=0; i<v.size(); i++) std::cerr << "\t" << v[i];
            std::cerr << "\n";
            continue;
        }
        bins.push_back(v[0]);
        data.push_back(v[7]);
    }
    
    TH1F* foo = new TH1F("hGoldhagen", "Goldhagen surface neutron spectrum", bins.size()-1, &bins[0]);
    foo->GetXaxis()->SetTitle("kinetic energy [MeV]");
    foo->GetXaxis()->SetTitleOffset(1.45);
    foo->GetYaxis()->SetTitle("flux per energy, [1/cm^{2}/s/MeV]");
    foo->GetYaxis()->SetTitleOffset(1.45);
    for(unsigned int i=0; i<bins.size()-1; i++) foo->SetBinContent(i+1,data[i]);
    return foo;
}

#endif
