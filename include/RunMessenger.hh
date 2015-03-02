/// \file RunMessenger.hh \brief Messenger for run control.
#ifndef RunMessenger_H
/// Assure this header is loaded only once
#define RunMessenger_H

#include <G4UImessenger.hh>

class RunAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

/// Messenger class for RunAction
class RunMessenger: public G4UImessenger {
public:
    /// Constructor
    RunMessenger(RunAction*);
    /// Destructor
    virtual ~RunMessenger();
    /// respond to UI commands
    void SetNewValue(G4UIcommand*,G4String);

private:
    RunAction* run;                             ///< run object being controlled

    G4UIdirectory* outputDir;                   ///< UI directory for run commands

    G4UIcmdWithAnInteger* outNumCmd;            ///< set run number
    G4UIcmdWithAnInteger* recLevCmd;            ///< set event recording completeness
    G4UIcmdWithoutParameter* primOnlyCmd;       ///< set to only produce primaries
    G4UIcmdWithAString* outFileName;            ///< set filename for ROOT output
};

#endif
