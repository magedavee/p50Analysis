#include "GoldhagenSpectrum.hh"
#include "strutils.hh"

#include <sstream>
#include <fstream>
#include <cmath>
#include <iostream>



TH1* loadGoldhagen(const string& fname) {
    
    std::ifstream infl(fname.c_str());
    vector<double> bins;
    vector<double> data;
    while (infl) {
        string s;
        if (!getline(infl, s)) break;
        vector<double> v = sToDoubles(s,"\t");
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
