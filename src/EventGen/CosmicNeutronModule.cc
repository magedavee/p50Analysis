#include "CosmicNeutronModule.hh"

#include <G4Neutron.hh>
#include <G4ParticleGun.hh>

#include <cassert>

CosmicNeutronModule::CosmicNeutronModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "CosmicNeutron"),
neutron_generator(P->GetVerbosity(), "Outer Tank") { }

void CosmicNeutronModule::GeneratePrimaries(G4Event* anEvent) {    
    
    G4ParticleGun* g = myPGA->GetParticleGun();
    assert(g);
    g->SetParticleDefinition(G4Neutron::NeutronDefinition());
    
    std::vector<G4double>* location = neutron_generator.GenerateSourceLocation();
    G4ThreeVector nCosmicPosition((*location)[0],(*location)[1],(*location)[2]);
    G4ThreeVector nCosmicMomentum((*location)[3],(*location)[4],(*location)[5]);
    
    g->SetParticleEnergy(neutron_generator.GenerateNeutronEnergy());
    g->SetParticlePosition(nCosmicPosition);
    g->SetParticleMomentumDirection(nCosmicMomentum);
    g->GeneratePrimaryVertex(anEvent);
}
