#ifndef SEPARATOR_HH
/// Assure this header is only loaded once
#define SEPARATOR_HH

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

/// Builder for scintillator separator panels
class SeparatorBuilder {
public:
    /// Constructor
    SeparatorBuilder();
    
    /// Construct geometry
    void construct(double w, double l);
    /// get width
    double getWidth() const { return width; }
    /// get length
    double getLength() const { return length; }
    /// get total thickness
    double getThick() const { return totalThick; }
    
    G4LogicalVolume* main_log;  ///< main mother volume

    double totalThick;          ///< total assembly thickness (z)
    double cfThick;             ///< carbon fiber core thickness
    
protected:
    double width;               ///< panel width (x)
    double length;              ///< panel length (y)
    G4VisAttributes sep_vis;    ///< visualization settings
};

#endif
