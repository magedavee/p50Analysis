// Unrestricted Use - Property of AECL
//
// CosmicNeutronMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Cosmic Neutron Module Messenger
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "CosmicNeutronMessenger.hh"		// Specifies the file which contains the class structure

#include "CosmicNeutronGenerator.hh"		// Specifies user-defined classes which are called upon in this class
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
CosmicNeutronMessenger::CosmicNeutronMessenger(CosmicNeutronGenerator* cosmic_n)
 :  generator(cosmic_n)			// Allows retrieval of CosmicNeutronGenerator to set user-defined values
{
	// Specifies all commands available for CosmicNeutronGenerator.cc

  cosmicNDir = new G4UIdirectory("/generator/module/cosmicNeutron/");
  cosmicNDir->SetGuidance("Cosmic Neutron Module settings.");
  cosmicNDir->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);

  cosmicSolarCmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicNeutron/setSolarModulation",this);
  cosmicSolarCmd->SetGuidance("Modify solar modulation parameter for Cosmic Neutron spectrum.");
  cosmicSolarCmd->SetGuidance("\tTypically a value between 465 MV and 1700 MV.");
  cosmicSolarCmd->SetParameterName("solar",false);
  cosmicSolarCmd->SetRange("solar>=0.");
  cosmicSolarCmd->SetDefaultUnit("megavolt");
  cosmicSolarCmd->AvailableForStates(G4State_Idle);

  cosmicRigidCmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicNeutron/setCutoffRigidity",this);
  cosmicRigidCmd->SetGuidance("Modify cut-off rigidity parameter for Cosmic Neutron spectrum.");
  cosmicRigidCmd->SetParameterName("rigid",false);
  cosmicRigidCmd->SetRange("rigid>=0.");
  cosmicRigidCmd->SetDefaultUnit("megavolt");
  cosmicRigidCmd->AvailableForStates(G4State_Idle);

  cosmicDepthCmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicNeutron/setAtmosphericDepth",this);
  cosmicDepthCmd->SetGuidance("Modify atmospheric depth parameter for Cosmic Neutron spectrum.");
  cosmicDepthCmd->SetGuidance("\t(1030. g/cm2 == ground level)");
  cosmicDepthCmd->SetGuidance("Can accept altitude parameter as well, use length units for this option.");
  cosmicDepthCmd->SetParameterName("depth",false);
  cosmicDepthCmd->SetRange("depth>=0.");
  cosmicDepthCmd->SetUnitCandidates("g/cm2 km");
  cosmicDepthCmd->AvailableForStates(G4State_Idle);

  cosmicWaterCmd = new G4UIcmdWithADouble("/generator/module/cosmicNeutron/setWaterEquivalence",this);
  cosmicWaterCmd->SetGuidance("Modify water-equivalent content parameter for Cosmic Neutron spectrum.");
  cosmicWaterCmd->SetGuidance("\tMust be a value between 0 and 1.");
  cosmicWaterCmd->SetParameterName("water",false);
  cosmicWaterCmd->SetRange("water>=0. && water<=1.");
  cosmicWaterCmd->AvailableForStates(G4State_Idle);

  cosmicMinECmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicNeutron/setMinimumSampleEnergy",this);
  cosmicMinECmd->SetGuidance("Modify minimum energy to sample for Cosmic Neutron spectrum.");
  cosmicMinECmd->SetParameterName("minE",true);
  cosmicMinECmd->SetDefaultValue(1.00e-8);
  cosmicMinECmd->SetDefaultUnit("MeV");
  cosmicMinECmd->AvailableForStates(G4State_Idle);

  cosmicMaxECmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicNeutron/setMaximumSampleEnergy",this);
  cosmicMaxECmd->SetGuidance("Modify maximum energy to sample for Cosmic Neutron spectrum.");
  cosmicMaxECmd->SetParameterName("maxE",true);
  cosmicMaxECmd->SetDefaultValue(1.00e+4);
  cosmicMaxECmd->SetDefaultUnit("MeV");
  cosmicMaxECmd->AvailableForStates(G4State_Idle);

  cosmicSetMonoCmd = new G4UIcmdWithABool("/generator/module/cosmicNeutron/setMonoEnergyFlag",this);
  cosmicSetMonoCmd->SetGuidance("Toggle mono-energetic neutrons for Cosmic Neutron module.");
  cosmicSetMonoCmd->SetGuidance("  true  = use specified energy");
  cosmicSetMonoCmd->SetGuidance("  false = use Cosmic Neutron spectrum");
  cosmicSetMonoCmd->SetParameterName("monoTog",true);
  cosmicSetMonoCmd->SetDefaultValue(true);
  cosmicSetMonoCmd->AvailableForStates(G4State_Idle);

  cosmicMonoECmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicNeutron/setNMonoEnergy",this);
  cosmicMonoECmd->SetGuidance("Modify the particle energy to be simulated using the mono-energetic option.");
  cosmicMonoECmd->SetParameterName("nMono",false);
  cosmicMonoECmd->SetRange("nMono>=0.");
  cosmicMonoECmd->SetDefaultUnit("MeV");
  cosmicMonoECmd->AvailableForStates(G4State_Idle);

  cosmicCosVolCmd = new G4UIcmdWithAString("/generator/module/cosmicNeutron/setTargetPhysicalVolume",this);
  cosmicCosVolCmd->SetGuidance("Modify target physical volume for Cosine Distribution spectrum.");
  cosmicCosVolCmd->SetGuidance("    Enter the name of the physical volume that will see the cosine distribution.");
  cosmicCosVolCmd->SetParameterName("volName",false);
  cosmicCosVolCmd->AvailableForStates(G4State_Idle);

  cosmicCosRadCmd = new G4UIcmdWithADoubleAndUnit("/generator/module/cosmicNeutron/setSourceSphereRadius",this);
  cosmicCosRadCmd->SetGuidance("Modify the radius of the source sphere.");
  cosmicCosRadCmd->SetGuidance("    Ensure that the radius is larger than the target volume for the most accurate results.");
  cosmicCosRadCmd->SetGuidance("    Enter a negative number to reset the radius to the optimized value.");
  cosmicCosRadCmd->SetParameterName("radius",true);
  cosmicCosRadCmd->SetDefaultValue(-1.0);
  cosmicCosRadCmd->SetDefaultUnit("m");
  cosmicCosRadCmd->AvailableForStates(G4State_Idle);

  cosmicVerbCmd = new G4UIcmdWithAnInteger("/generator/module/cosmicNeutron/verbose",this);
  cosmicVerbCmd->SetGuidance("Set the verbosity of this module");
  cosmicVerbCmd->SetParameterName("v",false);
  cosmicVerbCmd->AvailableForStates(G4State_Idle);

  cosmicPrintCmd = new G4UIcmdWithoutParameter("/generator/module/cosmicNeutron/printParameters",this);
  cosmicPrintCmd->SetGuidance("Prints all user-defined parameters for the Cosmic Neutron Module.");
  cosmicPrintCmd->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);
}

	// ****** Destructor ****** //
CosmicNeutronMessenger::~CosmicNeutronMessenger()
{
  ResetModuleCommands();

  delete cosmicNDir;
  delete cosmicSolarCmd;
  delete cosmicRigidCmd;
  delete cosmicDepthCmd;
  delete cosmicWaterCmd;
  delete cosmicMinECmd;
  delete cosmicMaxECmd;
  delete cosmicSetMonoCmd;
  delete cosmicMonoECmd;
  delete cosmicCosVolCmd;
  delete cosmicCosRadCmd;
  delete cosmicVerbCmd;
  delete cosmicPrintCmd;
}

	// ****** Input Command Instructions ****** //
void CosmicNeutronMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == cosmicSolarCmd)
  {
    generator->SetSolarModulation(cosmicSolarCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmicRigidCmd)
  {
    generator->SetCutoffRigidity(cosmicRigidCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmicDepthCmd)
  {
    if(cosmicDepthCmd->GetNewUnitValue(newValue) < 100.)
    { generator->SetAtmosphericDepth((cosmicDepthCmd->GetNewDoubleValue(newValue)),false); }
    else
    { generator->SetAtmosphericDepth((cosmicDepthCmd->GetNewDoubleValue(newValue)),true); }
  }
  else if(command == cosmicWaterCmd)
  {
    generator->SetWaterEquivalence(cosmicWaterCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmicMinECmd)
  {
    generator->SetMinEnergy(cosmicMinECmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmicMaxECmd)
  {
    generator->SetMaxEnergy(cosmicMaxECmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmicSetMonoCmd)
  {
    generator->SetMonoEnergyFlag(cosmicSetMonoCmd->GetNewBoolValue(newValue));
  }
  else if(command == cosmicMonoECmd)
  {
    generator->SetNeutronMonoEnergy(cosmicMonoECmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmicCosVolCmd)
  {
    generator->SetTargetVolume(newValue);
  }
  else if(command == cosmicCosRadCmd)
  {
    generator->SetSourceRadius(cosmicCosRadCmd->GetNewDoubleValue(newValue));
  }
  else if(command == cosmicVerbCmd)
  {
    generator->SetVerbosity(cosmicVerbCmd->GetNewIntValue(newValue));
  }
  else if(command == cosmicPrintCmd)
  {
    generator->PrintAllParameters();
  }
  else
  {
    G4cout << "Command not found." << G4endl;
  }
}

	// ****** Return Command String ****** //
G4String CosmicNeutronMessenger::GetCurrentValue(G4UIcommand* /*command*/)
{
  G4String cv;
  cv = "Command not implemented.";
  return cv;
}

	// ****** Reset Command List ****** //
void CosmicNeutronMessenger::ResetModuleCommands()		// Remove commands related to Cosmic Neutron Module when module is destroyed
{
  G4UImanager* UI_manager = G4UImanager::GetUIpointer();

  UI_manager->RemoveCommand(cosmicSolarCmd);
  UI_manager->RemoveCommand(cosmicRigidCmd);
  UI_manager->RemoveCommand(cosmicDepthCmd);
  UI_manager->RemoveCommand(cosmicWaterCmd);
  UI_manager->RemoveCommand(cosmicMinECmd);
  UI_manager->RemoveCommand(cosmicMaxECmd);
  UI_manager->RemoveCommand(cosmicSetMonoCmd);
  UI_manager->RemoveCommand(cosmicMonoECmd);
  UI_manager->RemoveCommand(cosmicCosVolCmd);
  UI_manager->RemoveCommand(cosmicCosRadCmd);
  UI_manager->RemoveCommand(cosmicVerbCmd);
  UI_manager->RemoveCommand(cosmicPrintCmd);
  UI_manager->RemoveCommand(cosmicNDir);

  cosmicNDir = 0;
  cosmicSolarCmd = 0;
  cosmicRigidCmd = 0;
  cosmicDepthCmd = 0;
  cosmicWaterCmd = 0;
  cosmicMinECmd = 0;
  cosmicMaxECmd = 0;
  cosmicSetMonoCmd = 0;
  cosmicMonoECmd = 0;
  cosmicCosVolCmd = 0;
  cosmicCosRadCmd = 0;
  cosmicVerbCmd = 0;
  cosmicPrintCmd = 0;
}

// EOF
