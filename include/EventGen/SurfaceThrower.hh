#ifndef SURFACETHROWER_HH
#define SURFACETHROWER_HH

#include "GeomHelper.hh"

/// Class for throwing events from a surface to a volume, with specified angular distribution
class SurfaceThrower {
public:
    /// Constructor
    SurfaceThrower(G4VPhysicalVolume* w): outer(true), W(w), nAttempts(0), nSurfaceThrows(0), nHits(0) { }
    
    /// Set source (surface) and target (volume)
    void setSourceTarget(G4VPhysicalVolume* SS, G4VPhysicalVolume* TT);
    
    /// Generate position and momentum for throwing particle; return whether successful direction generated
    void genThrow();
    
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
    
    int nAttempts;              ///< number of proposed throws
    int nSurfaceThrows;         ///< number of proposed throws passing surface cos-theta criteria
    int nHits;                  ///< number of events hitting target
};


#endif
