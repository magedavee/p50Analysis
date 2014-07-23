// Unrestricted Use - Property of AECL
// 
// RunMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Run Action Messenger
//      Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef RunMessenger_H
/// Assure this header is loaded only once
#define RunMessenger_H

#include "G4UImessenger.hh"

#include "globals.hh"

class RunAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class RunMessenger: public G4UImessenger {
public:

    /// Constructor
    RunMessenger(RunAction*);
    /// Destructor
    virtual ~RunMessenger();

    void SetNewValue(G4UIcommand*,G4String);

private:

    RunAction* run;                     ///< run object being controlled

    G4UIdirectory* outputDir;           ///< UI directory for run commands

    G4UIcmdWithAnInteger* outNumCmd;    ///< set run number
    G4UIcmdWithAnInteger* recLevCmd;    ///< set event recording completeness
    G4UIcmdWithAString* outFileName;    ///< set filename for ROOT output
};

#endif
