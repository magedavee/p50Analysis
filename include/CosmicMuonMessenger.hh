// Unrestricted Use - Property of AECL
// 
// CosmicMuonMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Cosmic Muon Module Messenger
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef CosmicMuonMessenger_H		// Only carries out if object is undefined
#define CosmicMuonMessenger_H 1		// Defines object

#include "G4UImessenger.hh"			// Specifies base class or parent class

#include "globals.hh"				// Specifies class defining all global parameters and variable types

class CosmicMuonGenerator;			// Structures necessary for class definition
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

class CosmicMuonMessenger: public G4UImessenger		// This class inherits functions from G4UImessenger
{
  public:	// Constructors and Destructors

    CosmicMuonMessenger(CosmicMuonGenerator*);		// Constructor
    virtual ~CosmicMuonMessenger();			// Destructor

  public:	// Accessible Methods

    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

  private:	// Internal Methods

    void ResetModuleCommands();

  private:	// Member Data

    CosmicMuonGenerator* generator;

  private:	// Constants

    G4UIdirectory* cosmicMuDir;				// Directories holding commands

    G4UIcmdWithADoubleAndUnit* cosmuTestCmd;		// Commands - activated during run-time
    G4UIcmdWithADouble* cosmuRatioCmd;
    G4UIcmdWithABool* cosmuPlusCmd;
    G4UIcmdWithABool* cosmuMinusCmd;
    G4UIcmdWithABool* cosmuBESSCmd;
    G4UIcmdWithADoubleAndUnit* cosmuMinPCmd;
    G4UIcmdWithADoubleAndUnit* cosmuMaxPCmd;
    G4UIcmdWithABool* cosmuSetMonoCmd;
    G4UIcmdWithADoubleAndUnit* cosmuMonoECmd;
    G4UIcmdWithAString* cosmuCosVolCmd;
    G4UIcmdWithADoubleAndUnit* cosmuCosRadCmd;
    G4UIcmdWithAnInteger* cosmuVerbCmd;
    G4UIcmdWithoutParameter* cosmuPrintCmd;
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
