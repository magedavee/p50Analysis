#ifndef SEPARATOR_HH
/// Assure this header is only loaded once
#define SEPARATOR_HH

#include "Builder.hh"
#include "OpticalSurfaceSetup.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

/// Builder for scintillator separator panels
class SeparatorBuilder: public Builder, public G4UImessenger {
public:
    /// Constructor
    SeparatorBuilder();
    
    /// Construct geometry
    void construct();
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    double width;               ///< panel width (y)
    double length;              ///< panel length (z)
    double totalThick;          ///< total assembly thickness (x)
    double cfThick;             ///< carbon fiber core thickness
    OpticalSurfaceSetup myOptSurf;      ///< optical surface specifications
    
protected:
    G4VisAttributes sep_vis;    ///< visualization settings
    G4UIdirectory ui_dir;       ///< UI directory for separator-related commands
    G4UIcmdWithADoubleAndUnit thick_cmd; ///< UI command for thickness
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
