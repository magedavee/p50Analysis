#ifndef IBDMODULE_HH
/// Assure headers are only loaded once
#define IBDMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "InverseBetaKinematics.hh"

/// Inverse Beta Decay event generator module
class IBDModule: public PrimaryGeneratorModule {
public:
    /// Constructor
    IBDModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);

protected:
    InverseBetaKinematics inverse_beta; ///< kinematics code for events
};

#endif
