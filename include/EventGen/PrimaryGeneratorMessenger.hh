#ifndef PrimaryGeneratorMessenger_H
/// Assure header file is only loaded once
#define PrimaryGeneratorMessenger_H

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAnInteger.hh>

class PrimaryGeneratorAction;

/// UI messenger for primary event generator
class PrimaryGeneratorMessenger: public G4UImessenger {
public:
    /// Constructor
    PrimaryGeneratorMessenger(PrimaryGeneratorAction*);

    /// Respond to UI commands
    void SetNewValue(G4UIcommand*,G4String);
    
private:
    
    PrimaryGeneratorAction* generator;          ///< generator being controlled
    G4UIdirectory genDir;                       ///< UI directory for generator commands

    G4UIcmdWithAnInteger verbCmd;               ///< UI command for setting verbosity
    G4UIcmdWithoutParameter moduleGuncmd;       ///< UI command for using "particle gun" generator
    G4UIcmdWithoutParameter moduleCRYcmd;       ///< UI command for using CRY cosmic ray generator
    G4UIcmdWithoutParameter moduleIBDcmd;       ///< UI command for using IBD generator
    G4UIcmdWithoutParameter moduleFisANucmd;    ///< UI command for using fission antineutrino generator
    G4UIcmdWithoutParameter moduleCosMucmd;     ///< UI command for using cosmic muon generator
    G4UIcmdWithoutParameter moduleCosNcmd;      ///< UI command for using cosmic neutron generator
    G4UIcmdWithoutParameter moduleCf252cmd;     ///< UI command for using Cf252 neutron generator
};

#endif
