// Unrestricted Use - Property of AECL
// 
// DetectorMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Geometry and Optical Module Messenger
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef DetectorMessenger_H		// Only carries out if object is undefined
#define DetectorMessenger_H 1		// Defines object

#include "G4UImessenger.hh"		// Specifies base class or parent class

#include "globals.hh"			// Specifies class defining all global parameters and variable types

class DetectorConstruction;		// Structures necessary for class definition
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

class DetectorMessenger: public G4UImessenger		// This class inherits functions from G4UImessenger
{
  public:	// Constructors and Destructors

    DetectorMessenger(DetectorConstruction*);		// Constructor
    virtual ~DetectorMessenger();			// Destructor

  public:	// Accessible Methods

    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

  private:	// Member Data

    DetectorConstruction* detector;

  private:	// Constants

    G4UIdirectory* scintDir;			// Directories holding commands
    G4UIdirectory* detectDir;

    G4UIcmdWithABool* opticalCmd;		// Commands - for use in the terminal during run-time
    G4UIcmdWithADouble* birksCmd;
  G4UIcmdWithADouble* lobeCmd;
  G4UIcmdWithADouble* sigalCmd;
  G4UIcmdWithADouble* spikeCmd;
  G4UIcmdWithADouble* reflCmd;
  G4UIcmdWithADouble* effCmd;
  G4UIcmdWithADouble* backCmd;
  G4UIcmdWithADouble* QECmd;
  G4UIcmdWithAnInteger* segXCmd;
  G4UIcmdWithAnInteger* segYCmd;
   G4UIcmdWithADouble* segLCmd;
    G4UIcmdWithADouble* segWCmd;
     G4UIcmdWithADouble* segHCmd;
  G4UIcmdWithADouble* shLeadCmd;
 G4UIcmdWithADouble* shBPolyCmd;
  G4UIcmdWithADouble* shLiPolyCmd;
 G4UIcmdWithADouble* scinLCmd;
    G4UIcmdWithADouble* scinWCmd;
     G4UIcmdWithADouble* scinHCmd;
      G4UIcmdWithADouble* segBuffCmd;
  G4UIcmdWithADouble* segAirCmd;
 G4UIcmdWithADouble* segWrapGapCmd;
  G4UIcmdWithADouble* segWrapCmd;
  G4UIcmdWithADouble* segAcrylCmd;
  G4UIcmdWithADouble* segPMTCmd;
    G4UIcmdWithAString* scintCompCmd;
    G4UIcmdWithABool* innerCmd;
    G4UIcmdWithABool* outerCmd;
    G4UIcmdWithABool* polyCmd;
    G4UIcmdWithABool* shieldCmd;
  G4UIcmdWithABool* cylinderCmd;
    G4UIcmdWithABool* vetoCmd;
    G4UIcmdWithABool* innerVCmd;
    G4UIcmdWithABool* outerVCmd;
    G4UIcmdWithABool* polyVCmd;
    G4UIcmdWithABool* shieldVCmd;
    G4UIcmdWithABool* vetoVCmd;
  G4UIcmdWithABool* vertCmd;
  G4UIcommand* modCmd;
  G4UIcommand* conCmd;
    G4UIcmdWithABool* explodeCmd;
    G4UIcmdWithoutParameter* printCmd;
  G4UIcmdWithAString *optSurfCmd;
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
