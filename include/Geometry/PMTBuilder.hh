#ifndef PMTBUILDER_HH
/// Assure this header is loaded only once
#define PMTBUILDER_HH

#include "Builder.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

/// Builder for a PMT, cylindrical and looking upwards to +z
class PMTBuilder: public Builder {
public:
    /// Constructor
    PMTBuilder();
    
    /// Construct geometry
    void construct();
    
    double block_width;			///< width of outer square housing block
    double block_length;		///< length of outer housing block
    double diameter;            ///< PMT diameter
    
protected:
    
    double length;                      ///< PMT length
    G4VisAttributes outer_vis;          ///< visualization setup for outside shell
    G4VisAttributes capsule_vis;        ///< visualization for quartz vacuum capsule

    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
