#ifndef GEOMHELPER_HH
#define GEOMHELPER_HH

#include <G4VPhysicalVolume.hh>
#include <G4RotationMatrix.hh>
#include <G4ThreeVector.hh>

/// Various utilities for locating solids in a geometry
class GeomHelper {
public:
    /// Constructor
    GeomHelper(G4VPhysicalVolume* p = NULL, G4VPhysicalVolume* c = NULL): P(p), C(c) { calcChildOffset(); }
    
    /// Set parent and child volumes (must not be NULL)
    void setParentChild(G4VPhysicalVolume* p, G4VPhysicalVolume* c);
    
    /// Transform a coordinate from child to parent coordinates
    G4ThreeVector coordCtoP(G4ThreeVector x) const;
    /// Transform a coordinate from parent to child coordinates
    G4ThreeVector coordPtoC(G4ThreeVector x) const;
    /// Rotate a direction from parent to child coordinates
    G4ThreeVector dirPtoC(G4ThreeVector d) { return rotPtoC*d; }
    /// Rotate a direction from child to parent coordinates
    G4ThreeVector dirCtoP(G4ThreeVector d) { return rotCtoP*d; }
    
    /// Determine whether direction d from point x (P coordinates) intersects C
    bool intersects(G4ThreeVector x, G4ThreeVector d);
    
protected:
    /// Calculate rotation and translation between parent and child volume
    void calcChildOffset();
    

    
    G4VPhysicalVolume* P;       ///< parent volume
    G4VPhysicalVolume* C;       ///< child volume
    G4RotationMatrix rotCtoP;   ///< rotate child coordinates to parent frame
    G4RotationMatrix rotPtoC;   ///< rotate parent coordinates to child frame
    G4ThreeVector transCtoP;    ///< translate child coordinates to parent frame
};

#endif
