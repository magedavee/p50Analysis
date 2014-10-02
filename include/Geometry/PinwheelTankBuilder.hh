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
    
    /// Construct geometry
    virtual void construct();
    
    /// get position at center of numbered segment
    virtual G4ThreeVector getSegmentPosition(unsigned int n) const;
    /// get segment number at position in scintillator local coordinates
    virtual int getSegmentNum(const G4ThreeVector& pos) const;
    
    PinwheelRodBuilder myPinwheelRod; ///< pinwheel rod for holding separators
    
protected:
    
    double theta_pw, sin_pw, cos_pw;    ///< pinwheeling rotation angle
    double lat_size;                    ///< rod lattice spacing
    G4RotationMatrix rotRod;            ///< rotation matrix for rods
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
