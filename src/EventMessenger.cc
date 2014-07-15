// Unrestricted Use - Property of AECL
//
// EventMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Event Action Messenger
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "EventMessenger.hh"			// Specifies the file which contains the class structure

#include "EventAction.hh"			// Specifies user-defined classes which are called upon in this class

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
#include "G4UnitsTable.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
EventMessenger::EventMessenger(EventAction* event_action)
  :  event(event_action)		// Allows retrieval of EventAction to set user-defined values
{
	// Specifies all commands available for EventMessenger.cc

  outputDir = new G4UIdirectory("/output/");
  outputDir->SetGuidance("Custom simulation output settings.");
  outputDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);

  printNCmd = new G4UIcmdWithAnInteger("/output/printPerNEvents",this);
  printNCmd->SetGuidance("Set the simulation to print out all currently requested data every N events of simulation");
  printNCmd->SetGuidance("   - Entering 0 will cause the simulation to save all outputs until the end of run");
  printNCmd->SetParameterName("N",true);
  printNCmd->SetRange("N>=0");
  printNCmd->SetDefaultValue(100000);
  printNCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
}

	// ****** Destructor ****** //
EventMessenger::~EventMessenger()
{
  delete outputDir;
  delete printNCmd;
}

	// ****** Input Command Instructions ****** //
void EventMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == printNCmd)
  {
    event->SetPrintPerNEvents(printNCmd->GetNewIntValue(newValue));
  }
  else
  {
    G4cout << "Command not found." << G4endl;
  }
}

	// ****** Return Command String ****** //
G4String EventMessenger::GetCurrentValue(G4UIcommand* /*command*/)
{
  G4String cv;
  cv = "Command not implemented.";
  return cv;
}

// EOF
