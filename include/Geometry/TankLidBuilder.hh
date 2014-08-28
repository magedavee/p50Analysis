#ifndef TANKLIDBUILDER_HH
/// Assure this header is only loaded once
#define TANKLIDBUILDER_HH

#include "Builder.hh"
#include "ScintTankBuilder.hh"

#include <cassert>

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

/// Builder for lids (with lightguide feedthroughs, etc.) on scintillator tank
class TankLidBuilder: public Builder {
public:
    /// Constructor
    TankLidBuilder();
    
    
    /// unused construct method
    virtual void construct() { assert(false); }
    /// Construct geometry, given dimensions of tank
    void construct(const ScintTankBuilder& T);

    /// get total thickness
    double getThick() const { return totalThick; }
    
    double totalThick;          ///< total assembly thickness (z)
    
protected:
    G4VisAttributes lid_vis;    ///< visualization settings
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
