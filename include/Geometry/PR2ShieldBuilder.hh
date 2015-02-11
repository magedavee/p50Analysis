/// \file PR2ShieldBuilder.hh \brief PROSPECT-2 shielding cave geometry.
#ifndef PR2SHIELDBUILDER_HH
#define PR2SHIELDBUILDER_HH

#include "Builder.hh"
#include "ScintSegVol.hh"

#include <G4UImessenger.hh>
#include <G4UIcmdWithABool.hh>

/// Builder for PROSPECT-2 shielding cave
class PR2ShieldBuilder: public ShellLayerBuilder, public G4UImessenger {
public:
    /// constructor
    PR2ShieldBuilder();
    
    /// Construct geometry
    void _construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    bool withWaterBricks = false;       ///< whether to build borated water bricks layer
    bool P2B_or_not_2B = false;         ///< enlarged "PROSPECT-2B" shield mode
    
protected:    
    G4UIcmdWithABool waterBrickCmd;     ///< UI command for enabling/disabling water bricks layer
    G4UIcmdWithABool mode2Bcmd;         ///< UI command for enabling/disabling "2B" configuration
};

/// Builder for PROSPECT-2/20 muon veto
class PR2MuVetoBuilder: public ScintChunk {
public:
    /// Constructor
    PR2MuVetoBuilder(int n);
};

#endif
