#ifndef SHIELDBUILDER_HH
/// Assure this header is only loaded once
#define SHIELDBUILDER_HH

#include "DetVolBuilder.hh"
#include "Utilities.hh"
#include "XMLProvider.hh"
#include <vector>

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>

/// Specification for a shielding layer
class ShieldLayerSpec: public XMLProvider {
public:
    /// Constructor
    ShieldLayerSpec(double t, G4Material* M = NULL, const G4Colour& c = G4Colour(0.5,0.5,0.2)):
    XMLProvider("ShieldLayer"), top_thick(t), side_thick(t), bottom_thick(0), mat(M), vis(c) { }
    
    double top_thick;           ///< thickness on top
    double side_thick;          ///< thickness on sides
    double bottom_thick;        ///< thickness on bottom
    G4Material* mat;            ///< material
    G4VisAttributes vis;        ///< layer visualization attributes

protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

/// Builder for shielding layers around detector
class ShieldBuilder: public G4UImessenger, public XMLProvider {
public:
    /// constructor
    ShieldBuilder();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    /// Construct geometry
    void construct();
    /// Get outer dimensions
    G4ThreeVector getDimensions() const { return dim; }
    /// remove all shield layers
    void clearShield() { layers.clear(); }
    /// add shield layer
    void addLayer(const ShieldLayerSpec& s) { layers.push_back(s); }
    
    G4LogicalVolume* main_log;                  ///< outermost volume layer
    DetVolBuilder myDet;                        ///< detector assembly
    
protected:
    std::vector<ShieldLayerSpec> layers;        ///< descriptions of each layer
    G4ThreeVector dim;                          ///< outermost dimensions
    
    G4UIdirectory shield_dir;                   ///< UI directory for shield commands
    G4UIcmdWithoutParameter clearCmd;           ///< command to remove shielding layers
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
