/// \file FluxCounterBuilder.hh \brief Vacuum ``flux counter'' volume for toy models.
#ifndef FLUXCOUNTERBUILDER_HH
#define FLUXCOUNTERBUILDER_HH

#include "Builder.hh"
#include "FluxCounterSD.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAnInteger.hh>

/// Class for toy models, evacuated flux-counting volume
class FluxCounterBuilder: public Builder, public G4UImessenger {
public:
    /// Constructor
    FluxCounterBuilder();
    
    /// Construct geometry
    void construct();
    
    /// Set dimensions
    void setDimensions(G4ThreeVector d) { dim = d; }
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
private:
    FluxCounterSD* mySD = NULL;         ///< sensitive detector
    G4VisAttributes main_vis;           ///< visualization settings for volume
    G4UIdirectory fluxDir;              ///< UI directory for flux counter commands
    G4UIcmdWithAnInteger ptclCmd;       ///< set particle types to count
};

#endif
