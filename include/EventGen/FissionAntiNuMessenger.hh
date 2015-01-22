// Unrestricted Use - Property of AECL
// 
// FissionAntiNuMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Fission Antinu Module Messenger
//      Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited 201407 M. P. Mendenhall
// --------------------------------------------------------

#ifndef FissionAntiNuMessenger_H
/// Assure headers are only loaded once
#define FissionAntiNuMessenger_H

#include "G4UImessenger.hh"

class FissionAntiNuModule;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithoutParameter;
class G4UIcmdWithABool;

class FissionAntiNuMessenger: public G4UImessenger {
public:
    /// Constructor
    FissionAntiNuMessenger(FissionAntiNuModule* M);
    /// Destructor
    virtual ~FissionAntiNuMessenger();

    void SetNewValue(G4UIcommand*,G4String);

private:

    FissionAntiNuModule* generator;

    G4UIdirectory* fissNuDir;
    G4UIcommand* fisProdCmd;
    G4UIcmdWithABool* fisDwyerCmd;
    G4UIcmdWithoutParameter* fisPrintCmd;
};

#endif
