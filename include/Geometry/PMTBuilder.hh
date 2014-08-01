#ifndef PMTBUILDER_HH
/// Assure this header is loaded only once
#define PMTBUILDER_HH

#include "XMLProvider.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

/// Builder for a PMT, cylindrical and looking upwards to +z
class PMTBuilder: public XMLProvider {
public:
    /// Constructor
    PMTBuilder();
    
    /// Construct geometry
    void construct();
    /// Get length
    double getLength() const { return length; }

    G4LogicalVolume* main_log;  ///< main volume enclosing PMT
    
    double diameter;            ///< PMT diameter
    
protected:
    
    double length;                      ///< PMT length
    G4VisAttributes outer_vis;          ///< visualization setup for outside shell
    G4VisAttributes capsule_vis;        ///< visualization for quartz vacuum capsule

    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
