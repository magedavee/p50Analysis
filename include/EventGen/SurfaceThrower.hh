#ifndef SURFACETHROWER_HH
#define SURFACETHROWER_HH

#include "GeomHelper.hh"
#include "VertexPositioner.hh"

#include <G4VisExtent.hh>
#include <G4LogicalVolume.hh>
#include <G4VSolid.hh>

/// Class for throwing events from a surface to a target volume, with specified angular distribution
/// Use "setSourceTarget" to set source and target volumes.
///     T=NULL will accept all throws from source.
///     S=NULL will default to throwing from world volume interior.
class SurfaceThrower: public VertexPositioner {
public:
    /// Constructor, specifying world volume and optional source/target volumes
    SurfaceThrower(G4VPhysicalVolume* w, G4VPhysicalVolume* SS = NULL, G4VPhysicalVolume* TT = NULL);
    
    /// Set source (surface) and target (volume)
    void setSourceTarget(G4VPhysicalVolume* SS, G4VPhysicalVolume* TT = NULL);
    
    /// Generate position and momentum for throwing particle; return whether successful direction generated
    void genThrow();
    
    /// Set position, momentum for list of particles; note: multiple primaries all thrown in same direction!
    virtual bool tryVertex(vector<primaryPtcl>& v);
    
    /// Get surface area of origin solid
    double getOriginArea() const { return W? W->GetLogicalVolume()->GetSolid()->GetSurfaceArea() : 0; }
    /// Get volume of origin solid
    double getOriginVolume() const { return W? W->GetLogicalVolume()->GetSolid()->GetCubicVolume() : 0; }
    
    G4ThreeVector pos;          ///< vertex position
    G4ThreeVector snorm;        ///< surface normal at position
    G4ThreeVector mom;          ///< momentum direction
    bool outer;                 ///< whether to throw from outside (or inside) of source surface
    bool fromVolume;            ///< throw isotropically from inside volume
    
protected:
    
    /// Propose throw momentum direction; defaults to isotrpoic. Subclass me with actual distribution!
    virtual G4ThreeVector proposeDirection() { return randomDirection(); }
    /// propose vertex position
    void proposePosition();
    /// Test a proposed momentum direction for surface normal and target hit
    bool tryMomentum();
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
        
    G4VPhysicalVolume* W;       ///< World volume
    G4VPhysicalVolume* S;       ///< Source surface
    G4VPhysicalVolume* T;       ///< Target volume
    GeomHelper W2S;             ///< World to source-volume geometry
    GeomHelper W2T;             ///< World to target volume geometry
    
    G4VisExtent sourceExtent;   ///< bounding box for source volume (local coordinates)
    
    int nSurfaceThrows = 0;     ///< number of proposed throws passing surface cos-theta criteria
    int nHits = 0;              ///< number of events hitting target
};


#endif
