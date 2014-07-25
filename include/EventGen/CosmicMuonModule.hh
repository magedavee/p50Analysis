#ifndef COSMICMUONMODULE_HH
/// Assure headers are only loaded once
#define COSMICMUONMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "CosmicMuonGenerator.hh"

/// Cosmic muon event generator module
class CosmicMuonModule: public PrimaryGeneratorModule {
public:
    /// Constructor
    CosmicMuonModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);

protected:
    CosmicMuonGenerator muon_generator; ///< kinematics code for events
};

#endif
