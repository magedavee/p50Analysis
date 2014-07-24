// Unrestricted Use - Property of AECL
// 
// FissionAntiNuMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Fission Antinu Module Messenger
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef FissionAntiNuMessenger_H		// Only carries out if object is undefined
#define FissionAntiNuMessenger_H 1		// Defines object

#include "G4UImessenger.hh"			// Specifies base class or parent class

#include "globals.hh"				// Specifies class defining all global parameters and variable types

class FissionAntiNuGenerator;			// Structures necessary for class definition
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

class FissionAntiNuMessenger: public G4UImessenger		// This class inherits functions from G4UImessenger
{
  public:	// Constructors and Destructors

    FissionAntiNuMessenger(FissionAntiNuGenerator*);		// Constructor
    virtual ~FissionAntiNuMessenger();				// Destructor

  public:	// Accessible Methods

    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

  private:	// Internal Methods

    void ResetModuleCommands();

  private:	// Member Data

    FissionAntiNuGenerator* generator;

  private:	// Constants

    G4UIdirectory* fissNuDir;				// Directories holding commands

    G4UIcommand* fisProdCmd;				// Commands - activated during run-time
    G4UIcmdWithAnInteger* fisVerbCmd;
    G4UIcmdWithoutParameter* fisPrintCmd;
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
