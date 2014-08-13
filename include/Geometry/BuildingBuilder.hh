#ifndef BUILDINGBUILDER_HH
/// Assure this header is only loaded once
#define BUILDINGBUILDER_HH

#include "ShieldBuilder.hh"
#include "XMLProvider.hh"

#include <G4LogicalVolume.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>

/// Builder for building containing detector
class BuildingBuilder: public G4UImessenger, public XMLProvider {
public:
    /// Constructor
    BuildingBuilder();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    /// Construct geometry
    void construct();
    
    /// Get outer dimensions
    G4ThreeVector getDimensions() const { return dim; }
    
    G4LogicalVolume* main_log;  ///< main mother volume: concrete building
    G4LogicalVolume* air_log;   ///< air space inside building
    
    double wall_thick;          ///< thickness of building side walls
    double wall_clearance;      ///< space from side walls to detector
    double ceil_thick;          ///< thickness of ceiling above detector
    double ceil_clearance;      ///< space between detector and ceiling
    double floor_thick;         ///< thickness of floor
    
    ShieldBuilder myDetUnit;    ///< detector assembly in shields
    
private:
    
    G4ThreeVector dim;          ///< outer dimensions
    G4VisAttributes wall_vis;   ///< visualization settings for shell
    
    G4UIdirectory building_ui_dir;      ///< UI directory for building-related commands
    G4UIcmdWithoutParameter bareCmd;    ///< UI command to remove walls
};

#endif
