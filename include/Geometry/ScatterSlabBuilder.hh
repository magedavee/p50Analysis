#ifndef SCATTERSLABBUILDER_HH
#define SCATTERSLABBUILDER_HH

#include "Builder.hh"

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

/// Builder for "slab" scattering toy geometry
class ScatterSlabBuilder: public Builder, public G4UImessenger {
public:
    /// Constructor
    ScatterSlabBuilder();
    
    /// Construct geometry
    virtual void construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    G4Material* mat;    ///< slab material
    double thick;       ///< slab thickness
    double width;       ///< slab width/height (close to "infinite")
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    G4LogicalVolume* slab_log;          ///< material slab
    G4LogicalVolume* slab_flux_log;     ///< particle flux counter
    
    G4UIdirectory slabDir;              ///< UI directory for slab-related commands
    G4UIcmdWithADoubleAndUnit thickCmd; ///< slab thickness command
    G4UIcmdWithAString matCmd;          ///< slab material command
};

#endif
