#ifndef SHIELDBUILDER_HH
/// Assure this header is only loaded once
#define SHIELDBUILDER_HH

#include "Builder.hh"
#include "DetVolBuilder.hh"
#include "Utilities.hh"
#include <vector>

#include <G4LogicalVolume.hh>

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

/// Builder for shielding layers around detector
class ShieldBuilder: public Builder, public G4UImessenger {
public:
    /// constructor
    ShieldBuilder();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    /// Construct geometry
    void construct();
    /// remove all shield layers
    void clearShield() { layers.clear(); }
    /// add shield layer
    void addLayer(const ShellLayerSpec& sh) { layers.push_back(sh); }
    
    DetVolBuilder myDet;                        ///< detector assembly
    
protected:
    std::vector<ShellLayerSpec> layers;        ///< descriptions of each layer
    
    G4UIdirectory shield_dir;                   ///< UI directory for shield commands
    G4UIcmdWithoutParameter clearCmd;           ///< command to remove shielding layers
    G4UIcmdWithADoubleAndUnit vetoCmd;          ///< command to add muon veto shell layer
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
