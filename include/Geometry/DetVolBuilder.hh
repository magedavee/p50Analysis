/// \file DetVolBuilder.hh \brief PROSPECT main detector volume geometry.
#ifndef DETVOLBUILDER_HH
/// Assure this header is loaded only once
#define DETVOLBUILDER_HH

#include "Builder.hh"
#include "ScintTankBuilder.hh"
#include "PinwheelTankBuilder.hh"
#include "PMTBuilder.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4RotationMatrix.hh>

/// Builder for detector volume (everything inside metal shell)
class DetVolBuilder: public Builder {
public:
    /// Constructor
    DetVolBuilder();
    
    /// Construct geometry
    void construct();
        
    double shell_thick;         ///< thickness of shell wall
    double buffer_thick;        ///< gap size between shell and internal assembly
    
    PinwheelTankBuilder myTank; ///< scintillator tank with dividers
    PMTBuilder myPMT;           ///< PMTs to place on each segment
    
private:
    G4RotationMatrix myRot;     ///< rotation to apply to internal components
    G4VisAttributes shell_vis;  ///< visualization settings for shell
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
