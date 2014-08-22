#include "CosmicNeutronModule.hh"
#include "DetectorConstruction.hh"

#include <G4Neutron.hh>
#include <G4ParticleGun.hh>

#include <cassert>

CosmicNeutronModule::CosmicNeutronModule(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "CosmicNeutron"),
SurfaceThrower(myPGA->GetDetector()->theWorld) {
    
}

void CosmicNeutronModule::GeneratePrimaries(G4Event* anEvent) {    
    
    G4ParticleGun* gn = myPGA->GetParticleGun();
    assert(gn);
    gn->SetParticleDefinition(G4Neutron::NeutronDefinition());
    
    
    do { } while(!genThrow());
    gn->SetParticlePosition(pos);
    gn->SetParticleMomentumDirection(mom);
    
    //gn->SetParticleEnergy();
    
    gn->GeneratePrimaryVertex(anEvent);
}
