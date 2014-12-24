#ifndef DIRECTIONTHROWER_HH
#define DIRECTIONTHROWER_HH

#include "SurfaceThrower.hh"

/// flux thrower for uniform in specified direction
class DirectionThrower: public SurfaceThrower {
public:
    /// Constructor
    DirectionThrower(G4VPhysicalVolume* w): SurfaceThrower(w,NULL,NULL,"DirectionThrower"), d(1,0,0) { }
    
    /// set throw direction
    void setDirection(const G4ThreeVector& v) { d = v.unit(); }
    
protected:
    /// direction distribution
    virtual G4ThreeVector proposeDirection() { return d; }
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    G4ThreeVector d;    ///< direction events are thrown
};

#endif
