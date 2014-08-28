#ifndef SCINTCELLBUILDER_HH
#define SCINTCELLBUILDER_HH

#include "Builder.hh"

/// Builder for liquid scintillator test cell
class ScintCellBuilder: public Builder {
public:
    /// Constructor
    ScintCellBuilder();
    
    /// Construct geometry
    virtual void construct();
    
    G4LogicalVolume* scint_log; ///< liquid scintillator volume
    
    double scint6LiLoading;     ///< scintillator 6Li loading mass fraction
    double length;      ///< cell outer length
    double radius;      ///< cell outer radius
    double wall_thick;  ///< cell wall thickness
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};
    
#endif
