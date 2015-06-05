#include "GenPtclModule.hh"

GenPtclModule::GenPtclModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "GenPtclSrc") {
    particle_source.SetNumberOfParticles(1);
}

void GenPtclModule::GeneratePrimaries(G4Event* anEvent) { 
   particle_source.GeneratePrimaryVertex(anEvent);
}   
