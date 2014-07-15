// Unrestricted Use - Property of AECL
//
// CosmicMuonMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Cosmic Muon Module Messenger
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "CosmicMuonMessenger.hh"		// Specifies the file which contains the class structure

#include "CosmicMuonGenerator.hh"		// Specifies user-defined classes which are called upon in this class
#include "CosmicCosineGenerator.hh"

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
CosmicMuonMessenger::CosmicMuonMessenger(CosmicMuonGenerator* cosmic_mu)
 :  generator(cosmic_mu)		// Allows retrieval of CosmicMuonGenerator to set user-defined values
{
	// Specifies all commands available for CosmicMuonGenerator.cc

  cosmicMuDir = new G4UIdirectory("/generator/module/cosmicMuon/");
  cosmicMuDir->SetGuidance("Cosmic Muon Module settings.");
  cosmicMuDir->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);

  cosmuTestCmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicMuon/setTestAngle",this);
  cosmuTestCmd->SetGuidance("Modify the spectrum test angle for the Lipari Cosmic Muon spectrum.");
  cosmuTestCmd->SetParameterName("angle",false);
  cosmuTestCmd->SetRange("angle>=0.0&&angle<=90.0");
  cosmuTestCmd->SetDefaultUnit("deg");
  cosmuTestCmd->AvailableForStates(G4State_Idle);

  cosmuRatioCmd = new G4UIcmdWithADouble("/generator/module/cosmicMuon/setPlusMinusRatio",this);
  cosmuRatioCmd->SetGuidance("Modify mu+ to mu- ratio for Cosmic Muon spectrum.");
  cosmuRatioCmd->SetGuidance("    Specify the ratio as number of mu+ generated per one mu- generated.");
  cosmuRatioCmd->SetParameterName("ratio",false);
  cosmuRatioCmd->SetRange("ratio>=0.0");
  cosmuRatioCmd->AvailableForStates(G4State_Idle);

  cosmuPlusCmd = new G4UIcmdWithABool("/generator/module/cosmicMuon/setOnlyMuonPlus",this);
  cosmuPlusCmd->SetGuidance("Set module configuration to generate mu+ particles only.");
  cosmuPlusCmd->SetParameterName("muPlus",true);
  cosmuPlusCmd->SetDefaultValue(true);
  cosmuPlusCmd->AvailableForStates(G4State_Idle);

  cosmuMinusCmd = new G4UIcmdWithABool("/generator/module/cosmicMuon/setOnlyMuonMinus",this);
  cosmuMinusCmd->SetGuidance("Set module configuration to generate mu- particles only.");
  cosmuMinusCmd->SetParameterName("muMinus",true);
  cosmuMinusCmd->SetDefaultValue(true);
  cosmuMinusCmd->AvailableForStates(G4State_Idle);

  cosmuBESSCmd = new G4UIcmdWithABool("/generator/module/cosmicMuon/setBESSSpec",this);
  cosmuBESSCmd->SetGuidance("Set module configuration to use the BESS muon energy spectrum.");
  cosmuBESSCmd->SetParameterName("BESS",false);
  cosmuBESSCmd->AvailableForStates(G4State_Idle);

  cosmuMinPCmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicMuon/setMinimumSampleRange",this);
  cosmuMinPCmd->SetGuidance("Modify minimum momentum to sample for Cosmic Muon spectrum.");
  cosmuMinPCmd->SetParameterName("minE",true);
  cosmuMinPCmd->SetDefaultValue(0.576);
  cosmuMinPCmd->SetDefaultUnit("GeV");
  cosmuMinPCmd->AvailableForStates(G4State_Idle);

  cosmuMaxPCmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicMuon/setMaximumSampleRange",this);
  cosmuMaxPCmd->SetGuidance("Modify maximum momentum/energy to sample for Cosmic Muon spectrum.");
  cosmuMaxPCmd->SetParameterName("maxE",true);
  cosmuMaxPCmd->SetDefaultValue(20.552);
  cosmuMaxPCmd->SetDefaultUnit("GeV");
  cosmuMaxPCmd->AvailableForStates(G4State_Idle);

  cosmuSetMonoCmd = new G4UIcmdWithABool("/generator/module/cosmicMuon/setMonoEnergyFlag",this);
  cosmuSetMonoCmd->SetGuidance("Toggle mono-energetic neutrons for Cosmic Muon module.");
  cosmuSetMonoCmd->SetGuidance("  true  = use specified energy");
  cosmuSetMonoCmd->SetGuidance("  false = use Cosmic Muon spectrum");
  cosmuSetMonoCmd->SetParameterName("monoTog",true);
  cosmuSetMonoCmd->SetDefaultValue(true);
  cosmuSetMonoCmd->AvailableForStates(G4State_Idle);

  cosmuMonoECmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicMuon/setMuMonoEnergy",this);
  cosmuMonoECmd->SetGuidance("Modify the particle energy to be simulated using the mono-energetic option.");
  cosmuMonoECmd->SetParameterName("muMono",false);
  cosmuMonoECmd->SetRange("muMono>=0.");
  cosmuMonoECmd->SetDefaultUnit("GeV");
  cosmuMonoECmd->AvailableForStates(G4State_Idle);

  cosmuCosVolCmd = new G4UIcmdWithAString("/generator/module/cosmicMuon/setTargetPhysicalVolume",this);
  cosmuCosVolCmd->SetGuidance("Modify target physical volume for Cosine Distribution spectrum.");
  cosmuCosVolCmd->SetGuidance("    Enter the name of the physical volume that will see the cosine distribution.");
  cosmuCosVolCmd->SetParameterName("volName",false);
  cosmuCosVolCmd->AvailableForStates(G4State_Idle);

  cosmuCosRadCmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicMuon/setSourceSphereRadius",this);
  cosmuCosRadCmd->SetGuidance("Modify the radius of the source sphere.");
  cosmuCosRadCmd->SetGuidance("    Ensure that the radius is larger than the target volume for the most accurate results.");
  cosmuCosRadCmd->SetGuidance("    Enter a negative number to reset the radius to the optimized value.");
  cosmuCosRadCmd->SetParameterName("radius",true);
  cosmuCosRadCmd->SetDefaultValue(-1.0);
  cosmuCosRadCmd->SetDefaultUnit("m");
  cosmuCosRadCmd->AvailableForStates(G4State_Idle);

  cosmuVerbCmd = new G4UIcmdWithAnInteger("/generator/module/cosmicMuon/verbose",this);
  cosmuVerbCmd->SetGuidance("Set the verbosity of this module");
  cosmuVerbCmd->SetGuidance("    0 = silent, 1 = minimal, 2 = loud");
  cosmuVerbCmd->SetGuidance("    Entries less than 0 will count as 0");
  cosmuVerbCmd->SetGuidance("    Entries greater than 0 will also output generated values");
  cosmuVerbCmd->SetParameterName("v",false);
  cosmuVerbCmd->AvailableForStates(G4State_Idle);

  cosmuPrintCmd = new G4UIcmdWithoutParameter("/generator/module/cosmicMuon/printParameters",this);
  cosmuPrintCmd->SetGuidance("Prints all user-defined parameters for the Cosmic Neutron Module.");
  cosmuPrintCmd->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);
}

	// ****** Destructor ****** //
CosmicMuonMessenger::~CosmicMuonMessenger()
{
  ResetModuleCommands();

  delete cosmicMuDir;
  delete cosmuTestCmd;
  delete cosmuRatioCmd;
  delete cosmuPlusCmd;
  delete cosmuMinusCmd;
  delete cosmuBESSCmd;
  delete cosmuMinPCmd;
  delete cosmuMaxPCmd;
  delete cosmuSetMonoCmd;
  delete cosmuMonoECmd;
  delete cosmuCosVolCmd;
  delete cosmuCosRadCmd;
  delete cosmuVerbCmd;
  delete cosmuPrintCmd;
}

	// ****** Input Command Instructions ****** //
void CosmicMuonMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == cosmuTestCmd)
  {
    generator->SetTestAngle(cosmuTestCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmuRatioCmd)
  {
    generator->SetPlusMinusRatio(cosmuRatioCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmuPlusCmd)
  {
    generator->SetMuonPlusOnlyFlag(cosmuPlusCmd->GetNewBoolValue(newValue));
  }
  else if(command == cosmuMinusCmd)
  {
    generator->SetMuonMinusOnlyFlag(cosmuMinusCmd->GetNewBoolValue(newValue));
  }
  else if(command == cosmuBESSCmd)
  {
    generator->SetBESSSpectrumFlag(cosmuBESSCmd->GetNewBoolValue(newValue));
  }
  else if(command == cosmuMinPCmd)
  {
    generator->SetMinRange(cosmuMinPCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmuMaxPCmd)
  {
    generator->SetMaxRange(cosmuMaxPCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmuSetMonoCmd)
  {
    generator->SetMonoEnergyFlag(cosmuSetMonoCmd->GetNewBoolValue(newValue));
  }
  else if(command == cosmuMonoECmd)
  {
    generator->SetMuonMonoEnergy(cosmuMonoECmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmuCosVolCmd)
  {
    generator->SetTargetVolume(newValue);
  }
  else if(command == cosmuCosRadCmd)
  {
    generator->SetSourceRadius(cosmuCosRadCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmuVerbCmd)
  {
    generator->SetVerbosity(cosmuVerbCmd->GetNewIntValue(newValue));
  }
  else if(command == cosmuPrintCmd)
  {
    generator->PrintAllParameters();
  }
  else
  {
    G4cout << "Command not found." << G4endl;
  }
}

	// ****** Return Command String ****** //
G4String CosmicMuonMessenger::GetCurrentValue(G4UIcommand* /*command*/)
{
  G4String cv;
  cv = "Command not implemented.";
  return cv;
}

	// ****** Reset Command List ****** //
void CosmicMuonMessenger::ResetModuleCommands()		// Removes commands related to Cosmic Muon Module when module is destroyed
{
  G4UImanager* UI_manager = G4UImanager::GetUIpointer();

  UI_manager->RemoveCommand(cosmuTestCmd);
  UI_manager->RemoveCommand(cosmuRatioCmd);
  UI_manager->RemoveCommand(cosmuPlusCmd);
  UI_manager->RemoveCommand(cosmuMinusCmd);
  UI_manager->RemoveCommand(cosmuBESSCmd);
  UI_manager->RemoveCommand(cosmuMinPCmd);
  UI_manager->RemoveCommand(cosmuMaxPCmd);
  UI_manager->RemoveCommand(cosmuSetMonoCmd);
  UI_manager->RemoveCommand(cosmuMonoECmd);
  UI_manager->RemoveCommand(cosmuCosVolCmd);
  UI_manager->RemoveCommand(cosmuCosRadCmd);
  UI_manager->RemoveCommand(cosmuVerbCmd);
  UI_manager->RemoveCommand(cosmuPrintCmd);
  UI_manager->RemoveCommand(cosmicMuDir);

  cosmicMuDir = 0;
  cosmuTestCmd = 0;
  cosmuRatioCmd = 0;
  cosmuPlusCmd = 0;
  cosmuMinusCmd = 0;
  cosmuBESSCmd = 0;
  cosmuMinPCmd = 0;
  cosmuMaxPCmd = 0;
  cosmuCosVolCmd = 0;
  cosmuCosRadCmd = 0;
  cosmuVerbCmd = 0;
  cosmuPrintCmd = 0;
}

// EOF
