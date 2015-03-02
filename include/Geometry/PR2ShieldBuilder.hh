/// \file PR2ShieldBuilder.hh \brief PROSPECT-2 shielding cave geometry.
#ifndef PR2SHIELDBUILDER_HH
#define PR2SHIELDBUILDER_HH

#include "Builder.hh"
#include "ScintSegVol.hh"

#include <G4UImessenger.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithAString.hh>

/// Base class for builder with waterbricks layer
class WaterbrickerBuilder:  public ShellLayerBuilder, public G4UImessenger {
public:
    /// Constructor
    WaterbrickerBuilder(const string& name);
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    int nWaterBrickLayers = 0;          ///< number of 9" waterbrick layers to build
    G4Material* wbMat = NULL;           ///< waterbrick layer material; default to water if unspecified
    
protected:
    
    /// add waterbricks layer
    void addWaterbricks();
    
    G4UIcmdWithAnInteger waterBrickCmd; ///< UI command for setting water bricks layers
    G4UIcmdWithAString wbMatCmd;        ///< UI command for setting waterbrick material
};

/// Builder for PROSPECT-2 shielding cave
class PR2ShieldBuilder: public WaterbrickerBuilder {
public:
    /// constructor
    PR2ShieldBuilder();
    
    /// Construct geometry
    void _construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    bool P2B_or_not_2B = false;         ///< enlarged "PROSPECT-2B" shield mode
    
protected:    
    G4UIcmdWithABool mode2Bcmd;         ///< UI command for enabling/disabling "2B" configuration
};

/// Builder for PROSPECT-2/20 muon veto
class PR2MuVetoBuilder: public ScintChunk {
public:
    /// Constructor
    PR2MuVetoBuilder(int n);
};

#endif
