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
    /// get total "real time" for primaries thrown
    virtual G4double GetGeneratorTime() const;
    
protected:
    InverseBetaKinematics inverse_beta; ///< kinematics code for events
    vector<G4double>* kinematics;
    G4bool primary;
    G4ThreeVector position;
    int nIBD = 0;                       ///< number of IBD events generated
};

#endif
