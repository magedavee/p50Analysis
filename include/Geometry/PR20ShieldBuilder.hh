/// \file PR20ShieldBuilder.hh \brief PROSPECT-20 inner and outer shield geometry.
#ifndef PR20SHIELDBUILDER_HH
#define PR20SHIELDBUILDER_HH

#include "Builder.hh"

#include <G4UImessenger.hh>
#include <G4UIcmdWithABool.hh>

/// Builder for PROSPECT-20 inner shielding cave
class PR20InnerShieldBuilder: public ShellLayerBuilder, public G4UImessenger {
public:
    /// constructor
    PR20InnerShieldBuilder();
    
    /// Construct geometry
    void _construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    bool withWaterBricks = false;       ///< whether to build borated water bricks layer
    
protected:
    G4UIcmdWithABool waterBrickCmd;     ///< UI command for enabling/disabling water bricks layer
};

/// Builder for PROSPECT-20 inner+outer cave
class PR20ShieldBuilder: public ShellLayerBuilder, public G4UImessenger {
public:
    /// constructor
    PR20ShieldBuilder();
    
    /// Construct geometry
    void _construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    PR20InnerShieldBuilder myInnerShield;       ///< inner shield layers
    bool withWaterBricks = false;       ///< whether to build borated water bricks layer
    
protected:    
    G4UIcmdWithABool waterBrickCmd;     ///< UI command for enabling/disabling water bricks layer
};

#endif
