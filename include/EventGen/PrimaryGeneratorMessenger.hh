// Unrestricted Use - Property of AECL
// 
// PrimaryGeneratorMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Initial Kinematics Messenger
//      Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited 201407 M. P. Mendenhall
// --------------------------------------------------------

#ifndef PrimaryGeneratorMessenger_H		// Only carries out if object is undefined
#define PrimaryGeneratorMessenger_H 1		// Defines object

#include "G4UImessenger.hh"			// Specifies base class or parent class

#include "globals.hh"				// Specifies class defining all global parameters and variable types

class PrimaryGeneratorAction;			// Structures necessary for class definition
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;

class PrimaryGeneratorMessenger: public G4UImessenger {
public:
    /// Constructor
    PrimaryGeneratorMessenger(PrimaryGeneratorAction*);
    /// Destructor
    virtual ~PrimaryGeneratorMessenger();
    
    void SetNewValue(G4UIcommand*,G4String);
    
private:

    void InitializeBasicCommands();
    void InitializeSpectrumCommands();
    void ResetSpectrumCommands();
    
    void ToggleCalibrationMode(G4bool);
    void InitializeCalibrationModeCommands();
    void ResetCalibrationModeCommands();
    
    PrimaryGeneratorAction* generator;

    G4UIdirectory* genDir;
    G4UIdirectory* calibDir;
    G4UIdirectory* gunDir;
    G4UIdirectory* spectDir;
    
    G4UIcmdWithoutParameter* moduleCRYcmd;
    G4UIcmdWithoutParameter* moduleIBDcmd;
    G4UIcmdWithoutParameter* moduleFisANucmd;
    G4UIcmdWithoutParameter* moduleCosMucmd;
    G4UIcmdWithoutParameter* moduleCosNcmd;
    
    G4UIcmdWithABool* calibOnCmd;
    G4UIcmdWithAnInteger* verbCmd;
    G4UIcmdWithAString* calSourceCmd;
    G4UIcmdWith3VectorAndUnit* calPosCmd;
    G4UIcmdWithAString* gunCmd;
    G4UIcmdWithAString* functCmd;
    G4UIcmdWithAString* fileCmd;
    G4UIcmdWithADoubleAndUnit* functMinECmd;
    G4UIcmdWithADoubleAndUnit* functMaxECmd;
    G4UIcmdWithADoubleAndUnit* functTempCmd;
    G4UIcmdWithAString* customCmd;
    G4UIcmdWithABool* energyInactivateCmd;
};

#endif