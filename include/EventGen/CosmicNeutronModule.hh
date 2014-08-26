#ifndef COSMICNEUTRONMODULE_HH
/// Assure headers are only loaded once
#define COSMICNEUTRONMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "SurfaceThrower.hh"
#include "SatoNiitaNeutrons.hh"

class TH1F;

/// Cosmic neutron event generator module
class CosmicNeutronModule: public PrimaryGeneratorModule, public SurfaceThrower, protected SatoNiitaNeutrons {
public:
    /// Constructor
    CosmicNeutronModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);

protected:
    /// generate distribution histogram
    void makeDistribution();
    /// neutron flux direction distribution
    virtual G4ThreeVector proposeDirection();
    
    TH1F* myDist;       ///< underlying energy distribution
    double waterFrac;   ///< water fraction by weight in reflecting material
};

#endif
