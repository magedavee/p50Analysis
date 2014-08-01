#include "CosmicMuonModule.hh"

#include <G4MuonMinus.hh>
#include <G4MuonPlus.hh>

#include <cassert>

CosmicMuonModule::CosmicMuonModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "CosmicMuon"),
muon_generator(P->GetVerbosity(), "Detector Shell") { }

void CosmicMuonModule::GeneratePrimaries(G4Event* anEvent) {    
    G4ParticleGun* g = myPGA->GetParticleGun();
    assert(g);
    
    std::vector<G4double>* location = muon_generator.GenerateSourceLocation();
    G4ThreeVector muCosmicPosition((*location)[0],(*location)[1],(*location)[2]);
    G4ThreeVector muCosmicMomentum((*location)[3],(*location)[4],(*location)[5]);
    G4double theEnergy = muon_generator.GenerateMuonEnergy();
    g->SetParticleEnergy(theEnergy);
    
    G4int mu_type = muon_generator.GenerateMuonType(theEnergy);
    if(mu_type) g->SetParticleDefinition(G4MuonMinus::MuonMinusDefinition());
    else g->SetParticleDefinition(G4MuonPlus::MuonPlusDefinition());
    
    g->SetParticlePosition(muCosmicPosition);
    g->SetParticleMomentumDirection(muCosmicMomentum);
    g->GeneratePrimaryVertex(anEvent);
}
