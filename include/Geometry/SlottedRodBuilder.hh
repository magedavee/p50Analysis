#ifndef SLOTTEDRODBUILDER_HH
/// Assure this header is loaded only once
#define SLOTTEDRODBUILDER_HH

#include "Builder.hh"
#include "OpticalSurfaceSetup.hh"

#include <cassert>

#include <G4VisAttributes.hh>

/// Base class for separator-holding rods
class RodBuilder: public Builder {
public:
    /// Constructor
    RodBuilder(const string& bnm);
    
    OpticalSurfaceSetup myOptSurf;      ///< optical surface properties
    
    double length;                      ///< rod length
    double r_hole;                      ///< inner hole radius
    
protected:
    G4VisAttributes rod_vis;            ///< visualization settings
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

/// Builder for slotted rods to hold separator panels
class SlottedRodBuilder: public RodBuilder {
public:
    /// Constructor
    SlottedRodBuilder();
    
    /// unused construct method
    virtual void construct() { assert(false); }
    /// Construct geometry, given length, slot radius, slot width
    void construct(double l, double rslot, double wslot);
    
    double r_outer;     ///< outer radius

protected:
    
    double r_slot;      ///< radius to innermost edge of each slot
    double w_slot;      ///< width of slot

    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
