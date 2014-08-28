#include "GoldhagenSpectrum.hh"
#include "strutils.hh"

#include <sstream>
#include <fstream>
#include <cmath>
#include <iostream>

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

TH1* loadGoldhagen(const string& fname) {
    
    std::ifstream infl(fname.c_str());
    std::vector<double> bins;
    std::vector<double> data;
    while (infl) {
        string s;
        if (!getline(infl, s)) break;
        std::vector<double> v = sToDoubles(s,"\t");
        if(v.size() != 10) {
            std::cerr << "Unreadable line in input file: '" << s << "' (" << v.size() << ")\n";
            for(size_t i=0; i<v.size(); i++) std::cerr << "\t" << v[i];
            std::cerr << "\n";
            continue;
        }
        bins.push_back(v[0]);
        data.push_back(v[3]);
    }
    
    TH1F* foo = new TH1F("hGoldhagen", "Goldhagen surface neutron spectrum", bins.size()-1, &bins[0]);
    foo->GetXaxis()->SetTitle("kinetic energy [MeV]");
    foo->GetXaxis()->SetTitleOffset(1.45);
    foo->GetYaxis()->SetTitle("flux per energy, [1/cm^{2}/s/MeV]");
    foo->GetYaxis()->SetTitleOffset(1.45);
    double netFlux = 0;
    for(unsigned int i=0; i<bins.size()-1; i++) {
        netFlux += data[i]*(bins[i+1]-bins[i]);
        foo->SetBinContent(i+1,data[i]);
    }
    std::cout << "Loaded Goldhagen spectrum with net flux " << netFlux*1000 << " mHz/cm^2\n";
    return foo;
}
