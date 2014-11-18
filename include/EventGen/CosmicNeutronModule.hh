#ifndef COSMICNEUTRONMODULE_HH
/// Assure headers are only loaded once
#define COSMICNEUTRONMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "SatoNiitaNeutrons.hh"

/// Cosmic neutron event generator module
class CosmicNeutronModule: public PrimaryGeneratorModule, protected SatoNiitaNeutrons {
public:
    /// Constructor
    CosmicNeutronModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);
    
    /// get total "real time" for primaries thrown
    virtual G4double GetGeneratorTime() const;

protected:
    
    /// generate distribution histogram
    void makeDistribution();
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    TH1F* myDist;       ///< underlying energy distribution
    double netFlux;     ///< total flux 1/cm^2/s
};

#endif
