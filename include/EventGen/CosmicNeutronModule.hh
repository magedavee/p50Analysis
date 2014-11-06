#ifndef COSMICNEUTRONMODULE_HH
/// Assure headers are only loaded once
#define COSMICNEUTRONMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "SurfaceThrower.hh"
#include "SatoNiitaNeutrons.hh"

#include <G4LogicalVolume.hh>
#include <G4VSolid.hh>

class TH1F;

/// Cosmic ray flux cosine-weighted vertex thrower
class CosmicFluxThrower: public SurfaceThrower {
public:
    /// constructor
    CosmicFluxThrower(G4VPhysicalVolume* w): SurfaceThrower(w) { }
    
    double getSourceArea() { return S->GetLogicalVolume()->GetSolid()->GetSurfaceArea(); }
protected:
    /// neutron flux direction distribution
    virtual G4ThreeVector proposeDirection();
};

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
    
    CosmicFluxThrower myThrower;        ///< vertex selection
    
    /// generate distribution histogram
    void makeDistribution();
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    TH1F* myDist;       ///< underlying energy distribution
    double netFlux;     ///< total flux 1/cm^2/s
    double area;        ///< originating surface area
};

#endif
