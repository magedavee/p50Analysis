#include "GenPtclModule.hh"
#include <G4Geantino.hh>

GenPtclModule::GenPtclModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "GenPtclSrc") {
    // Geantino (Default) Source
    mySrc.SetNumberOfParticles(1);
    mySrc.SetParticleDefinition(G4Geantino::GeantinoDefinition());
}
