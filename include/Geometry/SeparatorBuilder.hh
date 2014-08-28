#ifndef SEPARATOR_HH
/// Assure this header is only loaded once
#define SEPARATOR_HH

#include "Builder.hh"
#include "OpticalSurfaceSetup.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>


/// Builder for scintillator separator panels
class SeparatorBuilder: public Builder {
public:
    /// Constructor
    SeparatorBuilder();
    
    /// Construct geometry
    void construct();
    /// get width
    double getWidth() const { return width; }
    /// get length
    double getLength() const { return length; }
    /// get total thickness
    double getThick() const { return totalThick; }
    
    double width;               ///< panel width (x)
    double length;              ///< panel length (y)
    double totalThick;          ///< total assembly thickness (z)
    double cfThick;             ///< carbon fiber core thickness
    OpticalSurfaceSetup myOptSurf;      ///< optical surface specifications
    
protected:
    G4VisAttributes sep_vis;    ///< visualization settings
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
