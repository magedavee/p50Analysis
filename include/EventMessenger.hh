// Unrestricted Use - Property of AECL
// 
// EventMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Event Action Messenger
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef EventMessenger_H		// Only carries out if object is undefined
#define EventMessenger_H 1		// Defines object

#include "G4UImessenger.hh"		// Specifies base class or parent class

#include "globals.hh"			// Specifies class defining all global parameters and variable types

class EventAction;			// Structures necessary for class definition
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

class EventMessenger: public G4UImessenger		// This class inherits functions from G4UImessenger
{
  public:	// Constructors and Destructors

    EventMessenger(EventAction*);		// Constructor
    virtual ~EventMessenger();			// Destructor

  public:	// Accessible Methods

    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

  private:	// Member Data

    EventAction* event;

  private:	// Constants

    G4UIdirectory* outputDir;			// Directories holding commands

    G4UIcmdWithAnInteger* printNCmd;		// Commands - for use in the terminal during run-time
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
