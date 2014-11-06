#include <TH1F.h>
#include "ThermalNModule.hh"
#include "DetectorConstruction.hh"

#include <G4Neutron.hh>
#include <G4Gamma.hh>
#include <G4ParticleGun.hh>
#include <G4ios.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4VSolid.hh>

#include <cassert>
#include <cmath>


ThermalNModule::ThermalNModule(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "ThermalN"),
netRate(1/s) {
  //setSourceTarget(G4PhysicalVolumeStore::GetInstance()->GetVolume("DetVol_inner_phys"), G4PhysicalVolumeStore::GetInstance()->GetVolume("ScintTank_scint_phys"));  // inside shield
  //setSourceTarget(G4PhysicalVolumeStore::GetInstance()->GetVolume("Det_phys"), G4PhysicalVolumeStore::GetInstance()->GetVolume("ScintTank_scint_phys"));   //outside shield
  //  outer = true;
}

void ThermalNModule::GeneratePrimaries(G4Event* anEvent) {    
    primaryPtcl p;
    p.PDGid = 2112;
    p.KE = 0.025*eV;
    p.t = 0;
    
    vector<primaryPtcl> v;
    v.push_back(p);
    setVertices(v);
    throwPrimaries(v,anEvent);
}

G4double ThermalNModule::GetGeneratorTime() const {
    return 1.0; //double(nSurfaceThrows)/netRate;
}

void ThermalNModule::fillNode(TXMLEngine& E) {
    addAttr(E, "rate", netRate);
    //addAttr(E, "t_frac", double(nHits)/double(nSurfaceThrows));
}
