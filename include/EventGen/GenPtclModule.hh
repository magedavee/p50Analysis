#ifndef GENPTCLMODULE_HH
/// Assure this header is loaded only once
#define GENPTCLMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include <G4GeneralParticleSource.hh>

/// Event generator module providing G4GeneralParticleSource
class GenPtclModule: public PrimaryGeneratorModule {
public:
    /// Constructor
    GenPtclModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
  void GeneratePrimaries(G4Event*);
    
protected:
    G4GeneralParticleSource particle_source; ///< particle source
};

#endif
