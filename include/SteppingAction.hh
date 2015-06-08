/// \file SteppingAction.hh \brief Geant4 stepping action user class.
#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include <G4UserSteppingAction.hh>
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"
#include <G4UImessenger.hh>
#include <G4UIcmdWithADouble.hh>

/// user stepping action to check for and abort "trapped" events
class SteppingAction : public G4UserSteppingAction, public G4UImessenger {
public:
    /// constructor
    SteppingAction();
    
    /// custom per-step action: checks computation time not exceeded
    void UserSteppingAction(const G4Step*);
    /// reset trapping flags
    void Reset() { isTrapped = false; timeSpentSoFar = 0.; }
    /// respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    bool isTrapped;             ///< whether current event is "trapped"
    double timeSpentSoFar;      ///< CPU time spent on current event
    bool debugOptical = false;  ///< print optical photon debugging infoi
    double maxCalcTime = 10;    ///< maximum wall time for event calculation [s]
    
protected:
    G4UIcmdWithADouble calcTimeCmd;     ///< command for setting maxCalcTime
    
private:
    /// print debugging information for optical photons at boundary
    void CheckBoundaryStatus(G4OpBoundaryProcessStatus boundaryStatus);
};

#endif
