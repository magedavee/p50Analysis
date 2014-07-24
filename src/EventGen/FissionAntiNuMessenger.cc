// Unrestricted Use - Property of AECL
//
// FissionAntiNuMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Fission Antinu Module Messenger
//	Conatins definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "FissionAntiNuMessenger.hh"		// Specifies the file which contains the class structure

#include "FissionAntiNuGenerator.hh"		// Specifies user-defined classes which are called upon in this class

#include "G4UIdirectory.hh"			// Specifies the classes which contain structures called upon in this class
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UImanager.hh"
#include "G4Tokenizer.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
FissionAntiNuMessenger::FissionAntiNuMessenger(FissionAntiNuGenerator* fission_gen)
 :  generator(fission_gen)			// Allows retrieval of FissionAntiNuGenerator to set user-defined values
{
	// Specifies all commands available for FissionAntiNuGenerator.cc

  fissNuDir = new G4UIdirectory("/generator/module/fissionAntinu/");
  fissNuDir->SetGuidance("Reactor Antineutrino Module settings.");
  fissNuDir->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);

  G4UIparameter* param;		// Custom command definition

  fisProdCmd = new G4UIcommand("/generator/module/fissionAntinu/setFuelComposition",this);
  fisProdCmd->SetGuidance("Set the fission fuel composition that will dictate the antineutrino energy spectrum.");
  fisProdCmd->SetGuidance("     Insert four decimal values (does not necessarily have to total to unity):");
  fisProdCmd->SetGuidance("       First: U235, Second: U238, Third: Pu239, Fourth: Pu241");
  param = new G4UIparameter("U235",'d',false);
  param->SetDefaultValue("1.06");
  fisProdCmd->SetParameter(param);
  param = new G4UIparameter("U238",'d',false);
  param->SetDefaultValue("0.057");
  fisProdCmd->SetParameter(param);
  param = new G4UIparameter("Pu239",'d',false);
  param->SetDefaultValue("0.0");
  fisProdCmd->SetParameter(param);
  param = new G4UIparameter("Pu241",'d',false);
  param->SetDefaultValue("0.0");
  fisProdCmd->SetParameter(param);
  fisProdCmd->AvailableForStates(G4State_Idle);

  fisVerbCmd = new G4UIcmdWithAnInteger("/generator/module/fissionAntinu/verbose",this);
  fisVerbCmd->SetGuidance("Set the verbosity of this module");
  fisVerbCmd->SetGuidance("    0 = silent, 1 = minimal, 2 = loud");
  fisVerbCmd->SetGuidance("    Entries less than 0 will count as 0");
  fisVerbCmd->SetGuidance("    Entries greater than 0 will also output generated values");
  fisVerbCmd->SetParameterName("v",false);
  fisVerbCmd->AvailableForStates(G4State_Idle);

  fisPrintCmd = new G4UIcmdWithoutParameter("/generator/module/fissionAntinu/printParameters",this);
  fisPrintCmd->SetGuidance("Prints all user-defined parameters for the Reactor Antineutrino Module.");
  fisPrintCmd->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);
}

	// ****** Destructor ****** //
FissionAntiNuMessenger::~FissionAntiNuMessenger()
{
  ResetModuleCommands();

  delete fissNuDir;
  delete fisProdCmd;
  delete fisVerbCmd;
  delete fisPrintCmd;
}

	// ****** Input Command Instructions ****** //
void FissionAntiNuMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == fisProdCmd)
  {
    G4Tokenizer next(newValue);
    G4double setU = StoD(next()); G4double setUr = StoD(next()); G4double setPu = StoD(next()); G4double setPU = StoD(next());
    generator->SetAntiNeutrinoSpectrum(setU,setUr,setPu,setPU); 
  }
  else if(command == fisVerbCmd)
  {
    generator->SetVerbosity(fisVerbCmd->GetNewIntValue(newValue));
  }
  else if(command == fisPrintCmd)
  {
    generator->PrintAllParameters();
  }
  else
  {
    G4cout << "Command not found." << G4endl;
  }
}

	// ****** Return Command String ****** //
G4String FissionAntiNuMessenger::GetCurrentValue(G4UIcommand* /*command*/)
{
  G4String cv;
  cv = "Command not implemented.";
  return cv;
}

	// ****** Reset Command List ****** //
void FissionAntiNuMessenger::ResetModuleCommands()		// Removes commands related to Fission Antinu Module when module is destroyed
{
  G4UImanager* UI_manager = G4UImanager::GetUIpointer();

  UI_manager->RemoveCommand(fisProdCmd);
  UI_manager->RemoveCommand(fisVerbCmd);
  UI_manager->RemoveCommand(fisPrintCmd);
  UI_manager->RemoveCommand(fissNuDir);

  fissNuDir = 0;
  fisProdCmd = 0;
  fisVerbCmd = 0;
  fisPrintCmd = 0;
}

// EOF
