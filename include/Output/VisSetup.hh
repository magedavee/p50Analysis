#ifndef VISSETUP_HH
#define VISSETUP_HH

#include <G4VisManager.hh>
#include <G4SmartFilter.hh>
#include <G4VTrajectory.hh>

/// Provides default visualization settings
class VisSetup {
public:
    /// Constructor
    VisSetup(G4VisManager* vis_manager);
};

/// Filter to thin out optical photon tracks
class OpticalPhotonReducer: public G4SmartFilter<G4VTrajectory> {
public:
    /// constructor
    OpticalPhotonReducer(): G4SmartFilter<G4VTrajectory>("PhotonReducer"), pAccept(0.002), accum(1.) { SetActive(true); }
    
    /// decide whether to pass trajectory
    virtual bool Evaluate (const G4VTrajectory& T) const;
    /// print filter info
    virtual void Print (std::ostream&) const { }
    /// clear filter
    virtual void Clear() { }
    
protected:
    G4double pAccept;           ///< proportion of trajectories to draw
    mutable G4double accum;     ///< accumulator for undrawn trajectories
};

#endif
