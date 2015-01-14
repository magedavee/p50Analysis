/// \file PinwheelTankBuilder.hh \brief Segmented scintillator tank with ``pinwheeled'' divider scheme.
#ifndef PINWHEELTANKBUILDER_HH
/// Assure this header is loaded only once
#define PINWHEELTANKBUILDER_HH

#include "ScintTankBuilder.hh"
#include "PinwheelRodBuilder.hh"

#include <G4UImessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4VisAttributes.hh>

/// Builder for tank holding liquid scintillator and "pinwheeled" reflector lattice
class PinwheelTankBuilder: public ScintTankBuilder {
public:
    /// Constructor
    PinwheelTankBuilder();
    
    PinwheelRodBuilder myPinwheelRod;   ///< pinwheel rod for holding separators
    
protected:
    /// calculate dimensions and construct dividers
    virtual void setupDividers();
    /// construct gamma catcher geometry
    virtual void setupGammaCatcher();
};

#endif
