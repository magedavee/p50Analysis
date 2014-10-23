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
SurfaceThrower(myPGA->GetDetector()->theWorld),
netRate(1/s) {
  setSourceTarget(G4PhysicalVolumeStore::GetInstance()->GetVolume("DetVol_inner_phys"), G4PhysicalVolumeStore::GetInstance()->GetVolume("ScintTank_scint_phys"));  // inside shield
  //setSourceTarget(G4PhysicalVolumeStore::GetInstance()->GetVolume("Det_phys"), G4PhysicalVolumeStore::GetInstance()->GetVolume("ScintTank_scint_phys"));   //outside shield
    outer = true;
}

void ThermalNModule::GeneratePrimaries(G4Event* anEvent) {    
  
  G4ParticleGun* gn = myPGA->GetParticleGun();
  assert(gn);
  gn->SetParticleDefinition(G4Neutron::NeutronDefinition());
  
  // propose throw direction
  mom = proposeDirection();
  
  // propose source surface point.
  // Note: this won't be right if solid doesn't return uniformly distributed points!
  pos = S->GetLogicalVolume()->GetSolid()->GetPointOnSurface();
  gn->SetParticlePosition(pos);
  gn->SetParticleMomentumDirection(mom);
  
  gn->SetParticleEnergy(0.025*eV);
  
  gn->GeneratePrimaryVertex(anEvent);
}

G4double ThermalNModule::GetGeneratorTime() const {
    return double(nSurfaceThrows)/netRate;
}

void ThermalNModule::fillNode(TXMLEngine& E) {
    addAttr(E, "rate", netRate);
    addAttr(E, "t_frac", double(nHits)/double(nSurfaceThrows));
}
