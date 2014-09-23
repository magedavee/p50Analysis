// Unrestricted Use - Property of AECL
// 
// InverseBetaMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Inverse Beta Module Messenger
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef InverseBetaMessenger_H			// Only carries out if object is undefined
#define InverseBetaMessenger_H 1		// Defines object

#include "G4UImessenger.hh"			// Specifies base class or parent class

#include "globals.hh"				// Specifies class defining all global parameters and variable types

class InverseBetaKinematics;			// Structures necessary for class definition
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

class InverseBetaMessenger: public G4UImessenger		// This class inherits functions from G4UImessenger
{
  public:	// Constructors and Destructors

    InverseBetaMessenger(InverseBetaKinematics*);		// Constructor
    virtual ~InverseBetaMessenger();				// Destructor

  public:	// Accessible Methods

    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

  private:	// Internal Methods

    void ResetModuleCommands();

  private:	// Member Data

    InverseBetaKinematics* generator;

  private:	// Constants

    G4UIdirectory* invBetaDir;				// Directories holding commands

    G4UIcmdWithAString* invTargetCmd;			// Commands - activated during run-time
    G4UIcmdWithABool* invMonoTogCmd;
    G4UIcmdWithADoubleAndUnit* invMonoECmd;
    G4UIcmdWithABool* invNeutCmd;
    G4UIcmdWithABool* invPosiCmd;
    G4UIcmdWithABool* invSeqCmd;
    G4UIcommand* invProdCmd;
    G4UIcmdWith3Vector* invDirectCmd;
    G4UIcmdWithAnInteger* invVerbCmd;
    G4UIcmdWithoutParameter* invPrintCmd;
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
