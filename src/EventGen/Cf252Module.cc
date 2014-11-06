#include <TH1F.h>
#include "Cf252Module.hh"
#include "DetectorConstruction.hh"

#include <G4Neutron.hh>
#include <G4ParticleGun.hh>
#include <G4ios.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4VSolid.hh>

#include <cassert>
#include <cmath>

Cf252Module::Cf252Module(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "Cf252"),
netRate(1/s), a(1.18*MeV), b(1.03419/MeV) {
    const unsigned int nBins = 200;
    myDist = new TH1F("hCf252_Watt","Cf252 Neutron 'Watt' Spectrum", nBins, 0, 40*MeV);
    for(unsigned int i=0; i<nBins; i++) {
        double E = myDist->GetBinCenter(i+1);
        myDist->SetBinContent(i+1, exp(-E/a)*sqrt(sinh(b*E))*myDist->GetBinWidth(i+1));
    }
}

void Cf252Module::GeneratePrimaries(G4Event* anEvent) {    
    primaryPtcl p;
    p.PDGid = 2112;
    p.KE = myDist->GetRandom();
    p.t = 0;
    
    vector<primaryPtcl> v;
    v.push_back(p);
    setVertices(v);
    throwPrimaries(v,anEvent);
}

G4double Cf252Module::GetGeneratorTime() const {
    return 1; //double(nSurfaceThrows)/netRate;
}

void Cf252Module::fillNode(TXMLEngine& E) {
    addAttr(E, "rate", netRate);
    addAttr(E, "a", G4BestUnit(a,"Energy"));
    addAttr(E, "b", to_str(b*MeV)+"/MeV");
    //addAttr(E, "t_frac", double(nHits)/double(nSurfaceThrows));
}
