#ifndef SCINTLAYER_HH
#define SCINTLAYER_HH

#include "ScintSegVol.hh"

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

/// Builder for liquid scintillator test cell
class ScintLayer: public ScintSegVol, public G4UImessenger {
public:
    /// Constructor
    ScintLayer(int);
    
    /// Construct geometry
    virtual void construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    double scintLiLoading;      ///< scintillator Li loading mass fraction
    double length;              ///< cell outer length
    double radius;              ///< cell outer radius
    double wall_thick;          ///< cell wall thickness
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    int getSegmentNum(const G4ThreeVector& pos) const; 
    int volID;
    G4UIdirectory cellDir;                      ///< UI directory for test-cell-related commands
    G4UIcmdWithADoubleAndUnit lengthCmd;        ///< cell length command
    G4UIcmdWithADoubleAndUnit radiusCmd;        ///< cell radius command
    G4UIcmdWithADoubleAndUnit wallThickCmd;     ///< wall thickness command
    G4UIcmdWithADouble loadingCmd;              ///< Li loading command
};
    
#endif
