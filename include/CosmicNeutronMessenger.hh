// Unrestricted Use - Property of AECL
// 
// CosmicNeutronMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Cosmic Neutron Module Messenger
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef CosmicNeutronMessenger_H		// Only carries out if object is undefined
#define CosmicNeutronMessenger_H 1		// Defines object

#include "G4UImessenger.hh"			// Specifies base class or parent class

#include "globals.hh"				// Specifies class defining all global parameters and variable types

class CosmicNeutronGenerator;			// Structures necessary for class definition
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

class CosmicNeutronMessenger: public G4UImessenger		// This class inherits functions from G4UImessenger
{
  public:	// Constructors and Destructors

    CosmicNeutronMessenger(CosmicNeutronGenerator*);		// Constructor
    virtual ~CosmicNeutronMessenger();				// Destructor

  public:	// Accessible Methods

    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

  private:	// Internal Methods

    void ResetModuleCommands();

  private:	// Member Data

    CosmicNeutronGenerator* generator;

  private:	// Constants

    G4UIdirectory* cosmicNDir;				// Directories holding commands

    G4UIcmdWithADoubleAndUnit* cosmicSolarCmd;		// Commands - activated during run-time
    G4UIcmdWithADoubleAndUnit* cosmicRigidCmd;
    G4UIcmdWithADoubleAndUnit* cosmicDepthCmd;
    G4UIcmdWithADouble* cosmicWaterCmd;
    G4UIcmdWithADoubleAndUnit* cosmicMinECmd;
    G4UIcmdWithADoubleAndUnit* cosmicMaxECmd;
    G4UIcmdWithABool* cosmicSetMonoCmd;
    G4UIcmdWithADoubleAndUnit* cosmicMonoECmd;
    G4UIcmdWithAString* cosmicCosVolCmd;
    G4UIcmdWithADoubleAndUnit* cosmicCosRadCmd;
    G4UIcmdWithAnInteger* cosmicVerbCmd;
    G4UIcmdWithoutParameter* cosmicPrintCmd;
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
