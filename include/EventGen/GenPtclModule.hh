#ifndef GENPTCLMODULE_HH
/// Assure this header is loaded only once
#define GENPTCLMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include <G4UImessenger.hh>
#include <G4GeneralParticleSource.hh>

/// Event generator module providing G4GeneralParticleSource
class GenPtclModule: public PrimaryGeneratorModule {
public:
    /// Constructor
    GenPtclModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent) { }
    
protected:
    
    G4GeneralParticleSource mySrc;      ///< particle source
    
};

#endif
