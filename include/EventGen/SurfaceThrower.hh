#ifndef SURFACETHROWER_HH
#define SURFACETHROWER_HH

#include "GeomHelper.hh"

/// Class for throwing events from a surface to a volume, with specified angular distribution
class SurfaceThrower {
public:
    /// Constructor
    SurfaceThrower(G4VPhysicalVolume* w): outer(true), W(w) { }
    
    /// Set source (surface) and target (volume)
    void setSourceTarget(G4VPhysicalVolume* SS, G4VPhysicalVolume* TT);
    
    /// Generate position and momentum for throwing particle; return whether successful direction generated
    bool genThrow();
    
    G4ThreeVector pos;          ///< vertex position
    G4ThreeVector mom;          ///< momentum direction
    bool outer;                 ///< whether to throw from outside (or inside) of source surface
    
protected:
    
    /// Propose throw momentum direction; defaults to isotrpoic. Subclass me with actual distribution!
    virtual G4ThreeVector proposeDirection();
    
    G4VPhysicalVolume* W;       ///< World volume
    G4VPhysicalVolume* S;       ///< Source surface
    G4VPhysicalVolume* T;       ///< Target volume
    GeomHelper W2S;             ///< World to source-volume geometry
    GeomHelper W2T;             ///< World to target volume geometry
};


#endif
