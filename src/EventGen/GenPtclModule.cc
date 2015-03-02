#include "GenPtclModule.hh"
#include <G4Geantino.hh>

GenPtclModule::GenPtclModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "GenPtclSrc") {
}

void GenPtclModule::GeneratePrimaries(G4Event* anEvent) { 
  G4GeneralParticleSource* mySrc = myPGA->GetParticleSource();
  mySrc->GeneratePrimaryVertex(anEvent);
}   
