#include "GenPtclModule.hh"
#include <G4Geantino.hh>

GenPtclModule::GenPtclModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P) {
    // Geantino (Default) Source
    mySrc.SetNumberOfParticles(1);
    mySrc.SetParticleDefinition(G4Geantino::GeantinoDefinition());
}
