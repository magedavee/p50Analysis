#include "DwyerLangford.hh"
#include "PathUtils.hh"
#include "strutils.hh"

#include <cassert>
#include <sstream>
#include <fstream>

#include <G4SystemOfUnits.hh>
#include <G4ios.hh>

DwyerLangford::DwyerLangford() {
    string fBase = getEnvSafe("PG4_AUX")+"/ReactorSpectra/Dwyer_Langford_";
    load_data(fBase+"U235.txt", DwyerE, Dwyer235);
    load_data(fBase+"U238.txt", DwyerE, Dwyer238);
    assert(DwyerE.size() == Dwyer235.size());
    load_data(fBase+"Pu239.txt", DwyerE, Dwyer239);
    assert(DwyerE.size() == Dwyer235.size());
    load_data(fBase+"Pu241.txt", DwyerE, Dwyer241);
    assert(DwyerE.size() == Dwyer235.size());
    
    G4cout << "Dwyer-Langford spectrum initialized with " << DwyerE.size() << " datapoints." << G4endl;
}

void DwyerLangford::load_data(const string& fname, vector<G4double>& E, vector<G4double>& rate) const {
    E.clear();
    rate.clear();
    
    std::ifstream infl(fname.c_str());
    while (infl) {
        string l;
        if (!getline(infl, l)) break;
        if(l[0]=='#') continue;
        vector<double> v = sToDoubles(l,"\t");
        if(v.size() != 2) continue;
        E.push_back(v[0]*MeV);
        rate.push_back(v[1]);
    }
}
