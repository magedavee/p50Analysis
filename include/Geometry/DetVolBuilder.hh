#ifndef DETVOLBUILDER_HH
/// Assure this header is loaded only once
#define DETVOLBUILDER_HH

#include "ScintTankBuilder.hh"
#include "TankLidBuilder.hh"
#include "PMTBuilder.hh"
#include "XMLProvider.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4RotationMatrix.hh>

/// Builder for detector volume (everything inside metal shell)
class DetVolBuilder: public XMLProvider {
public:
    /// Constructor
    DetVolBuilder();
    
    /// Construct geometry
    void construct();
    /// Get outer length/width/height
    G4ThreeVector getDimensions() const { return dim; }
    
    G4LogicalVolume* main_log;  ///< main mother volume, metal shell enclosure
    
    double shell_thick;         ///< thickness of shell wall
    double buffer_thick;    ///< gap size between shell and internal assembly
    
    ScintTankBuilder myTank;    ///< scintillator tank with dividers
    TankLidBuilder myLid;       ///< lid to place on either side of tank
    PMTBuilder myPMT;           ///< PMTs to place on each segment
    
private:
    G4ThreeVector dim;          ///< outer dimensions
    G4RotationMatrix myRot;     ///< rotation to apply to internal components
    G4VisAttributes shell_vis;  ///< visualization settings for shell
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
