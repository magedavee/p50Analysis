#ifndef COSINETHROWER_HH
#define COSINETHROWER_HH

#include "SurfaceThrower.hh"

/// cos^x - weighted flux direction thrower
class CosineThrower: public SurfaceThrower {
public:
    /// Constructor
    CosineThrower(G4VPhysicalVolume* w): SurfaceThrower(w,NULL,NULL,"CosineThrower") { }
    
    /// set cosine exponent (0 for isotropic)
    void setExponent(double xx) { x = xx; }
    
protected:
    /// direction distribution
    virtual G4ThreeVector proposeDirection();
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    double x = 0;       ///< cosine exponent
};

#endif
