/// \file SurfaceThrower.hh \brief Event generator positioner flux from surface/volume.
#ifndef SURFACETHROWER_HH
#define SURFACETHROWER_HH

#include "GeomHelper.hh"
#include "VertexPositioner.hh"

#include <G4VisExtent.hh>
#include <G4LogicalVolume.hh>
#include <G4VSolid.hh>

/// Class for throwing events from a surface to a target volume, with specified angular distribution.
/// Use "setSourceTarget" to set source and target volumes.
///     T=NULL will accept all throws from source.
///     S=NULL will default to throwing from world volume interior.
class SurfaceThrower: public VertexPositioner {
public:
    /// Constructor, specifying world volume and optional source/target volumes
    SurfaceThrower(G4VPhysicalVolume* w, G4VPhysicalVolume* SS = NULL, G4VPhysicalVolume* TT = NULL, const string& nm = "SurfaceThrower");
    
    /// Set source (surface) and target (volume)
    void setSourceTarget(G4VPhysicalVolume* SS, G4VPhysicalVolume* TT = NULL);
    
    /// Set position, momentum for list of particles; return whether successful (else, need to try again with new particle list)
    virtual bool tryVertex(vector<primaryPtcl>& v);
    
    /// propose a vertex position
    virtual void proposePosition();
    
    /// Get surface area of origin solid
    double getOriginArea() const { return S? S->GetLogicalVolume()->GetSolid()->GetSurfaceArea() : 0; }
    /// Get volume of origin solid
    double getOriginVolume() const { return S? S->GetLogicalVolume()->GetSolid()->GetCubicVolume() : 0; }
    /// Get "normalized" (to volume, surface, etc.) number of attempts
    virtual double getAttemptsNormalized() const { return getAttempts()/(fromVolume?1.:getOriginArea()); }
    
    G4ThreeVector snorm;        ///< surface normal at position
    bool outer = true;          ///< whether to throw from outside (or inside) of source surface
    bool fromVolume = false;    ///< throw isotropically from inside volume
    bool reScatter = false;     ///< choose vertex in target with 1/r^2 weighting from point in source
    
protected:
    
    /// Propose throw momentum direction; defaults to isotrpoic. Subclass me with actual distribution!
    virtual G4ThreeVector proposeDirection() { return randomDirection(); }
    /// Propose direction after re-scatter; defaults to isotrpoic. Subclass me with actual distribution!
    virtual G4ThreeVector proposeRescatter() { return randomDirection(); }
    
    /// Test a proposed momentum direction for surface normal and target hit
    bool tryMomentum(G4ThreeVector& mom);
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
        
    G4VPhysicalVolume* W;       ///< World volume
    G4VPhysicalVolume* S;       ///< Source surface/volume
    G4VSolid* Ssolid;           ///< Source surface/volume shape
    G4VPhysicalVolume* T;       ///< Target volume
    G4VSolid* Tsolid;           ///< Target volume shape
    GeomHelper W2S;             ///< World to source-volume geometry
    GeomHelper W2T;             ///< World to target volume geometry
    
    G4VisExtent sourceExtent;   ///< bounding box for source volume (local coordinates)
    G4ThreeVector sourceCenter; ///< center of source bounding box (global coordinates)
    G4double sourceRadius;      ///< radius of source bounding box
    G4VisExtent targetExtent;   ///< bounding box for target (local coordinates)
    G4ThreeVector targetCenter; ///< center of target bounding box (global coordinates)
    G4double targetRadius;      ///< radius of target bounding box
    G4double rMin;              ///< lower bound on distance between source and target points
    G4double rMax;              ///< upper bound on distance between source and target points
    
    int nSurfaceThrows = 0;     ///< number of proposed throws passing surface cos-theta criteria
    int nHits = 0;              ///< number of events hitting target
};


#endif
