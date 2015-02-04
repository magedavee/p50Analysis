#include "CosmicMuonModule.hh"

#include <G4MuonMinus.hh>
#include <G4MuonPlus.hh>

#include <cassert>

CosmicMuonModule::CosmicMuonModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "CosmicMuon"),
muon_generator("Detector Shell") { }

void CosmicMuonModule::GeneratePrimaries(G4Event* anEvent) {    
    G4ParticleGun* g = myPGA->GetParticleGun();
    assert(g);
    
    muon_generator.GenerateSourceLocation();
    g->SetParticlePosition(muon_generator.pos);
    g->SetParticleMomentumDirection(muon_generator.mom);
    
    G4double theEnergy = muon_generator.GenerateMuonEnergy();
    g->SetParticleEnergy(theEnergy);
    
    G4int mu_type = muon_generator.GenerateMuonType(theEnergy);
    if(mu_type) g->SetParticleDefinition(G4MuonMinus::MuonMinusDefinition());
    else g->SetParticleDefinition(G4MuonPlus::MuonPlusDefinition());
    
    g->GeneratePrimaryVertex(anEvent);
}
