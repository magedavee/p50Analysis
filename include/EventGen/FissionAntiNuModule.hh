#ifndef FISSIONANTINUMODULE_HH
/// Assure headers are only loaded once
#define FISSIONANTINUMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "FissionAntiNuGenerator.hh"

/// Inverse Beta Decay event generator module
class FissionAntiNuModule: public PrimaryGeneratorModule {
public:
    /// Constructor
    FissionAntiNuModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);

protected:
    FissionAntiNuGenerator fission_spec; ///< kinematics code for events
};

#endif
