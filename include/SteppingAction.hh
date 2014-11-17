#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include <G4UserSteppingAction.hh>

/// user stepping action to check for and abort "trapped" events
class SteppingAction : public G4UserSteppingAction {
public:
    /// constructor
    SteppingAction() { Reset(); }
    
    /// custom per-step action: checks computation time not exceeded
    void UserSteppingAction(const G4Step*);
    
    /// reset trapping flags
    void Reset() { isTrapped = false; timeSpentSoFar = 0.; }
    
    bool isTrapped;             ///< whether current event is "trapped"
    double timeSpentSoFar;      ///< CPU time spent on current event
};

#endif
