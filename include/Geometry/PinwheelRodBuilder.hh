#ifndef PINWHEELRODBUILDER_HH
/// Assure this header is loaded only once
#define PINWHEELRODBUILDER_HH

#include "Builder.hh"
#include "OpticalSurfaceSetup.hh"

#include <cassert>

#include <G4VisAttributes.hh>

/// Builder for "pinwheel" rods to hold separator panels
/// See doc/Geometry/Pinwheel_Rod.svg
class PinwheelRodBuilder: public Builder {
public:
    /// Constructor
    PinwheelRodBuilder();
    
    /// Construct geometry
    virtual void construct();
    
    OpticalSurfaceSetup myOptSurf;      ///< optical surface properties
    
    double length;      ///< rod length
    double w_inner;     ///< inner square width
    double r_hole;      ///< central hole radius
    double t_end;       ///< thickness at end of separator panel
    double t_panel;     ///< thickness of slot for separator panel
    double t_hook;      ///< thickness of hook over separator panel
    double l_hook;      ///< length of hook over separator panel

protected:
    
    G4VisAttributes rod_vis;    ///< visualization settings

    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
