// Unrestricted Use - Property of AECL
//
// InverseBetaMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Inverse Beta Module Messenger
//	Conatins definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "InverseBetaMessenger.hh"		// Specifies the file which contains the class structure

#include "InverseBetaKinematics.hh"		// Specifies user-defined classes which are called upon in this class

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
InverseBetaMessenger::InverseBetaMessenger(InverseBetaKinematics* inv_beta)
 :  generator(inv_beta)			// Allows retrieval of InverseBetaKinematics to set user-defined values
{
	// Specifies all commands available for InverseBetaKinematics.cc

  invBetaDir = new G4UIdirectory("/generator/module/inverseBeta/");
  invBetaDir->SetGuidance("Antineutrino Inverse Beta Decay Product Module commands.");
  invBetaDir->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);

  invTargetCmd = new G4UIcmdWithAString("/generator/module/inverseBeta/setReactionVolume",this);
  invTargetCmd->SetGuidance("Modify target physical volume for Inverse Beta reaction.");
  invTargetCmd->SetGuidance("    Enter the name of the physical volume that contains active material.");
  invTargetCmd->SetParameterName("targName",false);
  invTargetCmd->AvailableForStates(G4State_Idle);

  invMonoTogCmd = new G4UIcmdWithABool("/generator/module/inverseBeta/setFissionSpectrum",this);
  invMonoTogCmd->SetGuidance("Set module configuration to use fission antineutrino spectrum or not.");
  invMonoTogCmd->SetParameterName("toggle",true);
  invMonoTogCmd->SetDefaultValue(true);
  invMonoTogCmd->AvailableForStates(G4State_Idle);

  invMonoECmd = new G4UIcmdWithADoubleAndUnit("/generator/module/inverseBeta/setAntinuEnergy",this);
  invMonoECmd->SetGuidance("Set energy of mono-energetic antineutrinos.");
  invMonoECmd->SetParameterName("nuEnergy",true);
  invMonoECmd->SetDefaultValue(2.0);
  invMonoECmd->SetDefaultUnit("MeV");
  invMonoECmd->AvailableForStates(G4State_Idle);

  invNeutCmd = new G4UIcmdWithABool("/generator/module/inverseBeta/setNeutrons",this);
  invNeutCmd->SetGuidance("Set module configuration to generate inverse reaction neutrons.");
  invNeutCmd->SetParameterName("neutrons",true);
  invNeutCmd->SetDefaultValue(true);
  invNeutCmd->AvailableForStates(G4State_Idle);

  invPosiCmd = new G4UIcmdWithABool("/generator/module/inverseBeta/setPositrons",this);
  invPosiCmd->SetGuidance("Set module configuration to generate inverse reaction positrons.");
  invPosiCmd->SetParameterName("positrons",true);
  invPosiCmd->SetDefaultValue(true);
  invPosiCmd->AvailableForStates(G4State_Idle);

  invSeqCmd = new G4UIcmdWithABool("/generator/module/inverseBeta/setSequential",this);
  invSeqCmd->SetGuidance("Set module configuration to generate inverse reaction positrons and neutrons.");
  invSeqCmd->SetParameterName("sequential",true);
  invSeqCmd->SetDefaultValue(true);
  invSeqCmd->AvailableForStates(G4State_Idle);

  G4UIparameter* param;		// Custom command definition

  invProdCmd = new G4UIcommand("/generator/module/inverseBeta/setFuelComposition",this);
  invProdCmd->SetGuidance("Set the fission fuel composition that will dictate the antineutrino energy spectrum.");
  invProdCmd->SetGuidance("     Insert four decimal values (does not necessarily have to total to unity):");
  invProdCmd->SetGuidance("       First: U235, Second: U238, Third: Pu239, Fourth: Pu241");
  param = new G4UIparameter("U235",'d',false);
  param->SetDefaultValue("1.0");
  invProdCmd->SetParameter(param);
  param = new G4UIparameter("U238",'d',false);
  param->SetDefaultValue("0.057");
  invProdCmd->SetParameter(param);
  param = new G4UIparameter("Pu239",'d',false);
  param->SetDefaultValue("0.0");
  invProdCmd->SetParameter(param);
  param = new G4UIparameter("Pu241",'d',false);
  param->SetDefaultValue("0.0");
  invProdCmd->SetParameter(param);
  invProdCmd->AvailableForStates(G4State_Idle);

  invDirectCmd = new G4UIcmdWith3Vector("/generator/module/inverseBeta/setIncidentDirection",this);
  invDirectCmd->SetGuidance("Set the direction of incoming antineutrinos for use in the kinematics module.");
  invDirectCmd->SetGuidance("     Input the direction by using a vector, entries separated by spaces:");
  invDirectCmd->SetGuidance("       (For reference: x is the long edge, y is up, z is the short edge)");
  invDirectCmd->SetParameterName("nuDirX","nuDirY","nuDirZ",false);
  invDirectCmd->AvailableForStates(G4State_Idle);

  invVerbCmd = new G4UIcmdWithAnInteger("/generator/module/inverseBeta/verbose",this);
  invVerbCmd->SetGuidance("Set the verbosity of this module");
  invVerbCmd->SetGuidance("    0 = silent, 1 = minimal, 2 = loud");
  invVerbCmd->SetGuidance("    Entries less than 0 will count as 0");
  invVerbCmd->SetGuidance("    Entries greater than 0 will also output generated values");
  invVerbCmd->SetParameterName("v",false);
  invVerbCmd->AvailableForStates(G4State_Idle);

  invPrintCmd = new G4UIcmdWithoutParameter("/generator/module/inverseBeta/printParameters",this);
  invPrintCmd->SetGuidance("Prints all user-defined parameters for the Inverse Beta Decay Product Module.");
  invPrintCmd->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);
}

	// ****** Destructor ****** //
InverseBetaMessenger::~InverseBetaMessenger()
{
  ResetModuleCommands();

  delete invBetaDir;
  delete invTargetCmd;
  delete invMonoTogCmd;
  delete invMonoECmd;
  delete invNeutCmd;
  delete invPosiCmd;
  delete invSeqCmd;
  delete invProdCmd;
  delete invDirectCmd;
  delete invVerbCmd;
  delete invPrintCmd;
}

	// ****** Input Command Instructions ****** //
void InverseBetaMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == invTargetCmd)
  {
    generator->SetReactionVolume(newValue);
  }
  else if(command == invMonoTogCmd)
  {
    generator->ToggleFission(invMonoTogCmd->GetNewBoolValue(newValue));
  }
  else if(command == invMonoECmd)
  {
    generator->SetAntiNeutrinoMonoEnergy(invMonoECmd->GetNewDoubleValue(newValue));
  }
  else if(command == invNeutCmd)
  {
    generator->ToggleNeutronGeneration(invNeutCmd->GetNewBoolValue(newValue));
  }
  else if(command == invPosiCmd)
  {
    generator->TogglePositronGeneration(invPosiCmd->GetNewBoolValue(newValue));
  }
  else if(command == invSeqCmd)
  {
    generator->ToggleSequentialGeneration(invSeqCmd->GetNewBoolValue(newValue));
  }
  else if(command == invProdCmd)
  {
    G4Tokenizer next(newValue);		// Tokenizer splits input string into delimited sections
    G4double setU = StoD(next()); G4double setUr = StoD(next()); G4double setPu = StoD(next()); G4double setPU = StoD(next());
    G4cout << "Simulation to be set to use antineutrino spectrum from fissions in the following split \n" 
           << "--- U235: " << setU << " U238: " << setUr << " Pu239: " << setPu << " Pu241: " << setPU << " ---" << G4endl;
    generator->SetAntiNeutrinoSpectrum(setU,setUr,setPu,setPU); 
    G4cout << "Simulation set to use antineutrino spectrum from fissions in the following split \n" 
           << "--- U235: " << setU << " U238: " << setUr << " Pu239: " << setPu << " Pu241: " << setPU << " ---" << G4endl;
  }
  else if(command == invDirectCmd)
  {
    generator->SetAntiNeutrinoDirection(invDirectCmd->GetNew3VectorValue(newValue));
  }
  else if(command == invVerbCmd)
  {
    generator->SetVerbosity(invVerbCmd->GetNewIntValue(newValue));
  }
  else if(command == invPrintCmd)
  {
    generator->PrintAllParameters();
  }
  else
  {
    G4cout << "Command not found." << G4endl;
  }
}

	// ****** Return Command String ****** //
G4String InverseBetaMessenger::GetCurrentValue(G4UIcommand* /*command*/)
{
  G4String cv;
  cv = "Command not implemented.";
  return cv;
}

	// ****** Reset Command List ****** //
void InverseBetaMessenger::ResetModuleCommands()		// Removes commands related to Inverse Beta Module when module is destroyed
{
  G4UImanager* UI_manager = G4UImanager::GetUIpointer();

  UI_manager->RemoveCommand(invMonoTogCmd);
  UI_manager->RemoveCommand(invMonoECmd);
  UI_manager->RemoveCommand(invNeutCmd);
  UI_manager->RemoveCommand(invPosiCmd);
  UI_manager->RemoveCommand(invSeqCmd);
  UI_manager->RemoveCommand(invProdCmd);
  UI_manager->RemoveCommand(invDirectCmd);
  UI_manager->RemoveCommand(invVerbCmd);
  UI_manager->RemoveCommand(invPrintCmd);
  UI_manager->RemoveCommand(invBetaDir);

  invBetaDir = 0;
  invTargetCmd = 0;
  invMonoTogCmd = 0;
  invMonoECmd = 0;
  invNeutCmd = 0;
  invPosiCmd = 0;
  invSeqCmd = 0;
  invProdCmd = 0;
  invDirectCmd = 0;
  invVerbCmd = 0;
  invPrintCmd = 0;
}

// EOF
