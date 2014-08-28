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
SurfaceThrower(myPGA->GetDetector()->theWorld),
netRate(1/s), a(1.18*MeV), b(1.03419/MeV) {
    setSourceTarget(myPGA->GetDetector()->ptclSrc, myPGA->GetDetector()->building_phys);
    outer = true;
    
    const unsigned int nBins = 1000;
    myDist = new TH1F("hCf252_Watt","Cf252 Neutron 'Watt' Spectrum", nBins, 0, 100*MeV);
    for(unsigned int i=0; i<nBins; i++) {
        double E = myDist->GetBinCenter(i+1);
        myDist->SetBinContent(i+1, exp(-E/a)*sqrt(sinh(b*E))*myDist->GetBinWidth(i+1));
    }
}

void Cf252Module::GeneratePrimaries(G4Event* anEvent) {    
    
    G4ParticleGun* gn = myPGA->GetParticleGun();
    assert(gn);
    gn->SetParticleDefinition(G4Neutron::NeutronDefinition());
    
    genThrow();
    gn->SetParticlePosition(pos);
    gn->SetParticleMomentumDirection(mom);
    
    gn->SetParticleEnergy(myDist->GetRandom());
    
    gn->GeneratePrimaryVertex(anEvent);
}

G4double Cf252Module::GetGeneratorTime() const {
    return double(nSurfaceThrows)/netRate;
}

void Cf252Module::fillNode(TXMLEngine& E) {
    addAttr(E, "rate", netRate);
    addAttr(E, "a", G4BestUnit(a,"Energy"));
    addAttr(E, "b", to_str(b*MeV)+"/MeV");
    addAttr(E, "t_frac", double(nHits)/double(nSurfaceThrows));
}
