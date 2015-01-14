/// \file BuildingBuilder.hh \brief Simple concrete building geometry.
#ifndef BUILDINGBUILDER_HH
/// Assure this header is only loaded once
#define BUILDINGBUILDER_HH

#include "Builder.hh"

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithoutParameter.hh>

/// Builder for building containing detector
class BuildingBuilder: public ShellLayerBuilder, public G4UImessenger {
public:
    /// Constructor
    BuildingBuilder();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    /// Construct geometry
    void _construct();
        
    double wall_thick;          ///< thickness of building side walls
    double wall_clearance;      ///< space from side walls to detector
    double ceil_thick;          ///< thickness of ceiling above detector
    double ceil_clearance;      ///< space between detector and ceiling
    double floor_thick;         ///< thickness of floor
    bool makeVacuum;            ///< whether to turn building materials to vacuum
    
private:
    
    G4UIdirectory building_ui_dir;      ///< UI directory for building-related commands
    
    G4UIcmdWithADoubleAndUnit ceilCmd;  ///< UI command for ceiling thickness
    G4UIcmdWithoutParameter vacuumCmd;  ///< UI command to turn building materials into vacuum
        
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
