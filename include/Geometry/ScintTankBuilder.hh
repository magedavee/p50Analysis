#ifndef SCINTTANKBUILDER_HH
/// Assure this header is loaded only once
#define SCINTTANKBUILDER_HH

#include "SeparatorBuilder.hh"
#include "SlottedRodBuilder.hh"

#include <G4UImessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4VisAttributes.hh>

/// Builder for tank holding liquid scintillator and reflector lattice
class ScintTankBuilder: public G4UImessenger {
public:
    /// Constructor
    ScintTankBuilder();
    
    /// Construct geometry
    void construct();
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    G4LogicalVolume* main_log;  ///< main mother volume: outer tank sides
    G4LogicalVolume* scint_log; ///< liquid scintillator volume
    
    /// get total number of segments
    uint getNSeg() const { return nSegX*nSegY; }
    /// get total width in x
    double getWidthX() const { return nSegX*seg_size+2*(tank_wall_thick+ls_buffer_thick); }
    /// get total width in y
    double getWidthY() const { return nSegY*seg_size+2*(tank_wall_thick+ls_buffer_thick); }
    /// get position at center of numbered segment
    G4ThreeVector getSegmentPosition(uint n) const;
    
    double tank_depth;          ///< scintillator depth in long (z) direction
    double tank_wall_thick;     ///< thickness of tank wall
    double ls_buffer_thick;     ///< thickness of scintillator buffer region around separators
    double seg_size;            ///< size of each segment in x-y plane
    uint nSegX;                 ///< number of x segments
    uint nSegY;                 ///< number of y segments
    double scint6LiLoading;     ///< loading fraction of 6Li in scintillator
    
    SlottedRodBuilder mySlottedRod;     ///< slotted rod for holding separators
    SeparatorBuilder mySeparator;       ///< separator panels
    
private:
    
    G4UIdirectory tank_ui_dir;          ///< UI directory for tank-related commands
    G4UIcmdWithAnInteger nsegXcmd;      ///< UI command for number of x segments
    G4UIcmdWithAnInteger nsegYcmd;      ///< UI command for number of y segments
    G4UIcmdWithADoubleAndUnit segWcmd;  ///< UI command for segment width
    G4UIcmdWithADouble scint6Licmd;     ///< UI command for 6Li loading in scintillator
    G4VisAttributes tank_vis;           ///< visualization settings for tank
    G4VisAttributes scint_vis;          ///< visualization settings for scintillator volume
};

#endif
