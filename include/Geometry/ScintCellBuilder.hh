#ifndef SCINTCELLBUILDER_HH
#define SCINTCELLBUILDER_HH

#include "ScintSegVol.hh"

/// Builder for liquid scintillator test cell
class ScintCellBuilder: public ScintSegVol {
public:
    /// Constructor
    ScintCellBuilder();
    
    /// Construct geometry
    virtual void construct();
    
    double scintLiLoading;      ///< scintillator Li loading mass fraction
    double length;      ///< cell outer length
    double radius;      ///< cell outer radius
    double wall_thick;  ///< cell wall thickness
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};
    
#endif
