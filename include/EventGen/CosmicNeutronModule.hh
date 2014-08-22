#ifndef COSMICNEUTRONMODULE_HH
/// Assure headers are only loaded once
#define COSMICNEUTRONMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "SatoNiitaNeutrons.hh"
#include "SurfaceThrower.hh"

//#include "CosmicNeutronGenerator.hh"

/// Cosmic neutron event generator module
class CosmicNeutronModule: public PrimaryGeneratorModule, public SurfaceThrower, protected SatoNiitaNeutrons {
public:
    /// Constructor
    CosmicNeutronModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);

//protected:
    //CosmicNeutronGenerator neutron_generator; ///< kinematics code for events
};

#endif
