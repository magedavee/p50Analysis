#ifndef PINWHEELRODBUILDER_HH
/// Assure this header is loaded only once
#define PINWHEELRODBUILDER_HH

#include "SlottedRodBuilder.hh"

#include <cassert>

#include <G4VisAttributes.hh>
#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4TwoVector.hh>

/// Builder for "pinwheel" rods to hold separator panels
/// See doc/Geometry/Pinwheel_Rod.svg
class PinwheelRodBuilder: public RodBuilder, public G4UImessenger {
public:
    /// Constructor
    PinwheelRodBuilder();
    
    /// Construct geometry
    virtual void construct();
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    double w_inner;     ///< inner square width
    double t_end;       ///< thickness at end of separator panel
    double t_panel;     ///< thickness of slot for separator panel
    double t_hook;      ///< thickness of hook over separator panel
    double l_hook;      ///< length of hook over separator panel

protected:
    
    G4UIdirectory ui_dir;       ///< UI directory for pinwheel-rod-related commands
    G4UIcmdWithADoubleAndUnit w_in_cmd; ///< UI command for inner width
    G4UIcmdWithADoubleAndUnit hole_cmd; ///< UI command for hole radius
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

/// create multiple rotated copies of vertices
void rotvert(vector<G4TwoVector>& vertices, unsigned int nrot);

#endif
