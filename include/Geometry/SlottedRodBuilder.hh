#ifndef SLOTTEDRODBUILDER_HH
/// Assure this header is loaded only once
#define SLOTTEDRODBUILDER_HH

#include "Builder.hh"
#include "OpticalSurfaceSetup.hh"

#include <cassert>

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

/// Builder for slotted rods to hold separator panels
class SlottedRodBuilder: public Builder {
public:
    /// Constructor
    SlottedRodBuilder();
    
    /// unused construct method
    virtual void construct() { assert(false); }
    /// Construct geometry, given length, slot radius, slot width
    void construct(double l, double rslot, double wslot);
    
    OpticalSurfaceSetup myOptSurf;      ///< optical surface properties
    
    double r_outer;             ///< outer radius
    double r_inner;             ///< inner radius

protected:
    double length;              ///< rod length
    double r_slot;              ///< radius to innermost edge of each slot
    double w_slot;              ///< width of slot   
    G4VisAttributes rod_vis;    ///< visualization settings

    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
