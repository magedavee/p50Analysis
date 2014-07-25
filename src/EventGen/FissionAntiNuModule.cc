#include "FissionAntiNuModule.hh"

#include "DetectorConstruction.hh"

#include <G4AntiNeutrinoE.hh>
#include <Randomize.hh>

FissionAntiNuModule::FissionAntiNuModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P),
fission_spec(true, P->GetVerbosity()) { }

void FissionAntiNuModule::GeneratePrimaries(G4Event* anEvent) {    
    G4ParticleGun* g = myPGA->GetParticleGun();
    assert(g);
    g->SetParticleDefinition(G4AntiNeutrinoE::AntiNeutrinoEDefinition());
    
    /// neutrinos coming straight up from under floor
    G4double positionZ = -( myPGA->GetDetector()->GetWorldSizeZ());
    G4double positionX = 2.0*( myPGA->GetDetector()->GetWorldSizeX())*(G4UniformRand()-0.5);
    G4double positionY = 2.0*( myPGA->GetDetector()->GetWorldSizeY())*(G4UniformRand()-0.5);
    g->SetParticlePosition(G4ThreeVector(positionX,positionY,positionZ));
    g->SetParticleEnergy(fission_spec.GenerateAntiNeutrinoEnergy());
    g->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
    g->GeneratePrimaryVertex(anEvent);
}
