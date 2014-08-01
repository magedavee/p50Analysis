#include "IBDModule.hh"

#include <G4Positron.hh>
#include <G4Neutron.hh>

#include <cassert>

IBDModule::IBDModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "IBD"),
inverse_beta(P->GetVerbosity(), "Scintillator Volume") { }

void IBDModule::GeneratePrimaries(G4Event* anEvent) {    
    G4ParticleGun* g = myPGA->GetParticleGun();
    assert(g);
    g->SetParticlePosition(inverse_beta.GenerateReactionPosition());
    
    std::vector<G4double>* kinematics = inverse_beta.GenerateReactionKinematics();
    G4ThreeVector positronMomentum((*kinematics)[1],(*kinematics)[2],(*kinematics)[3]);
    G4ThreeVector neutronMomentum((*kinematics)[5],(*kinematics)[6],(*kinematics)[7]);
    
    g->SetParticleDefinition(G4Positron::PositronDefinition());
    g->SetParticleEnergy((*kinematics)[0]);
    g->SetParticleMomentumDirection(positronMomentum);
    g->GeneratePrimaryVertex(anEvent);
    
    g->SetParticleDefinition(G4Neutron::NeutronDefinition());
    g->SetParticleEnergy((*kinematics)[4]);
    g->SetParticleMomentumDirection(neutronMomentum);
    g->GeneratePrimaryVertex(anEvent);
}
