#ifndef VISSETUP_HH
#define VISSETUP_HH

#include <G4TrajectoryDrawByParticleID.hh>
#include <G4VisManager.hh>

/// Provides default visualization settings
class VisSetup {
public:
    /// Constructor
    VisSetup(G4VisManager* vis_manager);
    
    G4TrajectoryDrawByParticleID pidModel; ///< draw-by-particle-ID model
};

#endif
