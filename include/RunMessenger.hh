// Unrestricted Use - Property of AECL
// 
// RunMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Run Action Messenger
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef RunMessenger_H			// Only carries out if object is undefined
#define RunMessenger_H 1		// Defines object

#include "G4UImessenger.hh"		// Specifies base class or parent class

#include "globals.hh"			// Specifies class defining all global parameters and variable types

class RunAction;			// Structures necessary for class definition
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

class RunMessenger: public G4UImessenger		// This class inherits functions from G4UImessenger
{
  public:	// Constructors and Destructors

    RunMessenger(RunAction*);			// Constructor
    virtual ~RunMessenger();			// Destructor

  public:	// Accessible Methods

    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

  private:	// Internal Methods

    void InitializeNEnergyCommands(G4bool);
    void InitializeNTimeCommands(G4bool);
    void InitializeNThermalizationCommands(G4bool);
    void InitializeNDistanceCommands(G4bool);
    void InitializePEnergyCommands(G4bool);
    void InitializeEntryEnergyCommands(G4bool);
    void InitializeLSPhotonHitCommands(G4bool);
    void InitializePhotonTravelTimeCommands(G4bool);
    void InitializeVetoPhotonHitCommands(G4bool);
    void InitializeIonEnergyDepositCommands(G4bool);
    void InitializeIonEnergyEscapeCommands(G4bool);
    void InitializeVetoEnergyDepositCommands(G4bool);

    void ResetOutputCommands();

  private:	// Member Data

    RunAction* run;

  private:	// Constants

    G4UIdirectory* outputDir;			// Directories holding commands
    G4UIdirectory* nEnergyDir;
    G4UIdirectory* nTimeDir;
    G4UIdirectory* nThermDir;
    G4UIdirectory* nDistDir;
    G4UIdirectory* pEnergyDir;
    G4UIdirectory* nEntryDir;
    G4UIdirectory* photHitDir;
    G4UIdirectory* photTimeDir;
    G4UIdirectory* vetoHitDir;
    G4UIdirectory* ionDepDir;
    G4UIdirectory* ionEscDir;
    G4UIdirectory* ionVDepDir;

  G4UIcommand* outSetCmd;			// Commands - for use in the terminal during run-time
  G4UIcommand* outRawCmd;
  G4UIcmdWithAString* outputCmd;
  G4UIcmdWithAString* rootOutputCmd;
    G4UIcmdWithoutParameter* listCmd;
  G4UIcommand* outNumCmd;
  G4UIcommand* evNumCmd;
  G4UIcommand* recLevCmd;

    G4UIcmdWithADoubleAndUnit* nEnBinCmd;
    G4UIcmdWithADoubleAndUnit* nEnRefCmd;
    G4UIcmdWithAnInteger* nEnNumCmd;

    G4UIcmdWithADoubleAndUnit* nTmBinCmd;
    G4UIcmdWithADoubleAndUnit* nTmRefCmd;
    G4UIcmdWithAnInteger* nTmNumCmd;

    G4UIcmdWithADoubleAndUnit* nThBinCmd;
    G4UIcmdWithADoubleAndUnit* nThRefCmd;
    G4UIcmdWithAnInteger* nThNumCmd;

    G4UIcmdWithADoubleAndUnit* nDsBinCmd;
    G4UIcmdWithADoubleAndUnit* nDsRefCmd;
    G4UIcmdWithAnInteger* nDsNumCmd;

    G4UIcmdWithADoubleAndUnit* pEnBinCmd;
    G4UIcmdWithADoubleAndUnit* pEnRefCmd;
    G4UIcmdWithAnInteger* pEnNumCmd;

    G4UIcmdWithADoubleAndUnit* nEEBinCmd;
    G4UIcmdWithADoubleAndUnit* nEERefCmd;
    G4UIcmdWithAnInteger* nEENumCmd;

    G4UIcmdWithAnInteger* phBinCmd;
    G4UIcmdWithAnInteger* phRefCmd;
    G4UIcmdWithAnInteger* phNumCmd;
    G4UIcmdWithADoubleAndUnit* phCutCmd;
    G4UIcmdWithABool* phPMTCmd;

    G4UIcmdWithADoubleAndUnit* phTBinCmd;
    G4UIcmdWithADoubleAndUnit* phTRefCmd;
    G4UIcmdWithAnInteger* phTNumCmd;

    G4UIcmdWithAnInteger* phVBinCmd;
    G4UIcmdWithAnInteger* phVRefCmd;
    G4UIcmdWithAnInteger* phVNumCmd;

    G4UIcmdWithADoubleAndUnit* iEDBinCmd;
    G4UIcmdWithADoubleAndUnit* iEDRefCmd;
    G4UIcmdWithAnInteger* iEDNumCmd;

    G4UIcmdWithADoubleAndUnit* iEEBinCmd;
    G4UIcmdWithADoubleAndUnit* iEERefCmd;
    G4UIcmdWithAnInteger* iEENumCmd;

    G4UIcmdWithADoubleAndUnit* iVDBinCmd;
    G4UIcmdWithADoubleAndUnit* iVDRefCmd;
    G4UIcmdWithAnInteger* iVDNumCmd;
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
