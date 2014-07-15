// Unrestricted Use - Property of AECL
// 
// PrimaryGeneratorMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Initial Kinematics Messenger
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
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

/* -------- Class Definition --------- */

class PrimaryGeneratorMessenger: public G4UImessenger		// This class inherits functions from G4UImessenger
{
  public:	// Constructors and Destructors

    PrimaryGeneratorMessenger(PrimaryGeneratorAction*);		// Constructor
    virtual ~PrimaryGeneratorMessenger();			// Destructor

  public:	// Accessible Methods

    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

  private:	// Internal Methods

    void InitializeBasicCommands();
    void InitializeSpectrumCommands();
    void ResetSpectrumCommands();

    void ToggleCalibrationMode(G4bool);
    void InitializeCalibrationModeCommands();
    void ResetCalibrationModeCommands();

  private:	// Member Data

    PrimaryGeneratorAction* generator;

  private:	// Constants

    G4UIdirectory* genDir;			// Directories holding commands
    G4UIdirectory* calibDir;
    G4UIdirectory* gunDir;
    G4UIdirectory* spectDir;
    G4UIdirectory* moduleDir;
  G4UIdirectory*               CRYDir;

    G4UIcmdWithABool* calibOnCmd;		// Commands - activated during run-time
    G4UIcmdWithAnInteger* verbCmd;
    G4UIcmdWithAnInteger* testCmd;
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
    G4UIcmdWithAString* moduleCmd;
    G4UIcmdWithoutParameter* moduleResetCmd;
  G4UIcmdWithABool* cryOnCmd;
  G4UIcmdWithABool* cryPointCmd;
  G4UIcmdWithADouble* cryZCmd;
 G4UIcmdWithAString*          cryFileCmd; 
    G4UIcmdWithAString*          cryInputCmd;
    G4UIcmdWithoutParameter*     cryUpdateCmd;
    std::string* MessInput;
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
