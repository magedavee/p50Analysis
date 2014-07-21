// Unrestricted Use - Property of AECL
//
// PrimaryGeneratorMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Initial Kinematics Messenger
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "PrimaryGeneratorMessenger.hh"		// Specifies the file which contains the class structure

#include "PrimaryGeneratorAction.hh"		// Specifies user-defined classes which are called upon in this class
#include "CosmicNeutronGenerator.hh"
#include "CosmicMuonGenerator.hh"
#include "CosmicCosineGenerator.hh"
#include "InverseBetaKinematics.hh"
#include "FissionAntiNuGenerator.hh"

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
#include "G4ApplicationState.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* gen_action)
  :  generator(gen_action)		// Allows retrieval of PrimaryGeneratorAction to set user-defined values
{
	// Specifies all commands available for PrimaryGeneratorAction.cc
  CRYDir = new G4UIdirectory("/CRY/");
  CRYDir->SetGuidance("CRY initialization");

  cryOnCmd = new G4UIcmdWithABool("/CRY/toggleCryMode",this);
  cryOnCmd->SetGuidance("Toggles the detector CRY mode, disables all other functions.");
  cryOnCmd->SetGuidance("   true: Enable CRY mode");
  cryOnCmd->SetGuidance("  false: Disable CRY mode");
  cryOnCmd->SetParameterName("cryMode",false);
  cryOnCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

  cryPointCmd = new G4UIcmdWithABool("/CRY/toggleCryPointing",this);
  cryPointCmd->SetGuidance("Toggles ehether CRY preselwects particles pointing towards the detector");
  cryPointCmd->SetGuidance("  true: Enable CRY pointing");
  cryPointCmd->SetGuidance("  false: Disable CRY pointing");
  cryPointCmd->SetParameterName("cryPoint",false);
  cryPointCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

  cryZCmd = new G4UIcmdWithADoubleAndUnit("/CRY/setCRYZOffset",this);
  cryZCmd->SetGuidance("sets the Z offset for the CRY generator (default is world maximum Z)");
  cryZCmd->SetParameterName("cryZ",false);
  cryZCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
   
  cryFileCmd = new G4UIcmdWithAString("/CRY/file",this);
  cryFileCmd->SetGuidance("This reads the CRY definition from a file");
  cryFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
   
  cryInputCmd = new G4UIcmdWithAString("/CRY/input",this);
  cryInputCmd->SetGuidance("CRY input lines");
  cryInputCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  cryUpdateCmd = new G4UIcmdWithoutParameter("/CRY/update",this);
  cryUpdateCmd->SetGuidance("Update CRY definition.");
  cryUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  cryUpdateCmd->SetGuidance("if you changed the CRY definition.");
  cryUpdateCmd->AvailableForStates(G4State_Idle);

  MessInput = new std::string;

  genDir = new G4UIdirectory("/generator/");
  genDir->SetGuidance("Custom simulation settings.");

  calibOnCmd = new G4UIcmdWithABool("/generator/toggleCalibrationMode",this);
  calibOnCmd->SetGuidance("Toggles the detector calibration mode, disables all other functions.");
  calibOnCmd->SetGuidance("   true: Enable calibration mode");
  calibOnCmd->SetGuidance("  false: Disable calibration mode");
  calibOnCmd->SetParameterName("calMode",false);
  calibOnCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

  verbCmd = new G4UIcmdWithAnInteger("/generator/verbose",this);
  verbCmd->SetGuidance("Set the verbosity of this module");
  verbCmd->SetGuidance("    0 = silent, 1 = minimal, 2 = loud");
  verbCmd->SetGuidance("    Entries less than 0 will count as 0");
  verbCmd->SetGuidance("    Entries greater than 0 will also output generated values");
  verbCmd->SetParameterName("v",false);
  verbCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

  InitializeBasicCommands();

  calibDir = 0;
  calSourceCmd = 0;
  calPosCmd = 0;

  functMinECmd = 0;
  functMaxECmd = 0;
  functTempCmd = 0;
}

	// ****** Destructor ****** //
PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete genDir;
  delete CRYDir;
  delete cryInputCmd;
  delete cryUpdateCmd;
  delete cryFileCmd;
  if(calibDir) { delete calibDir; }
  if(gunDir) { delete gunDir; }
  if(spectDir) { delete spectDir; }
  if(moduleDir) { delete moduleDir; }

  delete calibOnCmd;
  if(calSourceCmd) { delete calSourceCmd; }
  if(calPosCmd) { delete calPosCmd; }

  delete verbCmd;
  if(testCmd) { delete testCmd; }
  if(gunCmd) { delete gunCmd; }
  if(functCmd) { delete functCmd; }
  if(fileCmd) { delete fileCmd; }
  if(functMinECmd) { delete functMinECmd; }
  if(functMaxECmd) { delete functMaxECmd; }
  if(functTempCmd) { delete functTempCmd; }
  if(customCmd) { delete customCmd; }
  if(energyInactivateCmd) { delete energyInactivateCmd; }
  if(moduleCmd) { delete moduleCmd; }
  if(moduleResetCmd) { delete moduleResetCmd; }
}

	// ****** Initialize Standard Commands ****** //
void PrimaryGeneratorMessenger::InitializeBasicCommands()
{
  testCmd = new G4UIcmdWithAnInteger("/generator/test",this);
  testCmd->SetGuidance("Create a histogram of energy spectrum without generating particles.");
  testCmd->SetParameterName("nSamples",true);
  testCmd->SetDefaultValue(1000);
  testCmd->SetRange("nSamples>0");
  testCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

  gunDir = new G4UIdirectory("/generator/gunType/");
  gunDir->SetGuidance("Toggle ParticleGun or ParticleSource.");
  gunDir->AvailableForStates(G4State_Idle);

  gunCmd = new G4UIcmdWithAString("/generator/gunType/set",this);
  gunCmd->SetGuidance("Use a specified particle generator");
  gunCmd->SetGuidance("   - Overriden if one of the pre-defined models is specifed");
  gunCmd->SetParameterName("Type",false);
  gunCmd->SetCandidates("gun source");
  gunCmd->AvailableForStates(G4State_Idle);

  spectDir = new G4UIdirectory("/generator/spectrum/");
  spectDir->SetGuidance("User energy spectrum commands.");
  spectDir->AvailableForStates(G4State_Idle);

  functCmd = new G4UIcmdWithAString("/generator/spectrum/setFunction",this);
  functCmd->SetGuidance("Name of function describing energy spectrum data");
  functCmd->SetParameterName("funcName",false);
  functCmd->SetCandidates("cf252 maxwell");
  functCmd->AvailableForStates(G4State_Idle);
  fileCmd = new G4UIcmdWithAString("/generator/spectrum/setFile",this);
  fileCmd->SetGuidance("Name of file containing particle data");
  fileCmd->SetParameterName("fileName",false);
  fileCmd->AvailableForStates(G4State_Idle);

  customCmd = new G4UIcmdWithAString("/generator/spectrum/setCustom",this);
  customCmd->SetGuidance("Path name of text file containing energy spectrum data");
  customCmd->SetParameterName("pathName",false);
  customCmd->AvailableForStates(G4State_Idle);

  energyInactivateCmd = new G4UIcmdWithABool("/generator/spectrum/inactivate",this);
  energyInactivateCmd->SetGuidance("Inactivate custom energy spectrum");
  energyInactivateCmd->SetGuidance("(reactivate using set command)");
  energyInactivateCmd->SetGuidance("     false :  Keep function distribution if specified.");
  energyInactivateCmd->SetGuidance("     true  :  Discard function distribution as well.");
  energyInactivateCmd->SetParameterName("reset",true);
  energyInactivateCmd->SetDefaultValue(false);
  energyInactivateCmd->AvailableForStates(G4State_Idle);

  moduleDir = new G4UIdirectory("/generator/module/");
  moduleDir->SetGuidance("Activate and control pre-defined simulation modules");
  moduleDir->AvailableForStates(G4State_Idle);

  moduleCmd = new G4UIcmdWithAString("/generator/module/set",this);
  moduleCmd->SetGuidance("Set the kinematics module of the simulation");
  moduleCmd->SetParameterName("moduleName",true);
  moduleCmd->SetDefaultValue("Options:");
  moduleCmd->SetCandidates("Options: cosmicNeutron inverseBeta fissionAntinu cosmicMuon");
  moduleCmd->AvailableForStates(G4State_Idle);

  moduleResetCmd = new G4UIcmdWithoutParameter("/generator/module/reset",this);
  moduleResetCmd->SetGuidance("Remove all simulation modules");
  moduleResetCmd->AvailableForStates(G4State_Idle);
}

	// ****** Initialize Calibration Commands ****** //
void PrimaryGeneratorMessenger::InitializeCalibrationModeCommands()
{
  calibDir = new G4UIdirectory("/generator/calibrate/");
  calibDir->SetGuidance("Detector calibration functions.");
  calibDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

  calSourceCmd = new G4UIcmdWithAString("/generator/calibrate/setSource",this);
  calSourceCmd->SetGuidance("Set the calibration source to be used");
  calSourceCmd->SetParameterName("source",true);
  calSourceCmd->SetDefaultValue("none");
  calSourceCmd->SetCandidates("Co60 Cs137 Cf252 LED none");
  calSourceCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

  calPosCmd = new G4UIcmdWith3VectorAndUnit("/generator/calibrate/setPosition",this);
  calPosCmd->SetGuidance("Set the calibration source position");
  calPosCmd->SetParameterName("xPos","yPos","zPos",false);
  calPosCmd->SetDefaultUnit("m");
  calPosCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
}

	// ****** Initialize Function Commands ****** //
void PrimaryGeneratorMessenger::InitializeSpectrumCommands()
{
  G4int type = generator->GetFunction(); if(type == 0) { return; }
  if(!functMinECmd)
  {
    functMinECmd = new G4UIcmdWithADoubleAndUnit("/generator/spectrum/setMinEnergy",this);
    functMinECmd->SetGuidance("Modify the minimum energy range of chosen spectrum.");
    functMinECmd->SetGuidance("\tMust be a positive value.");
    functMinECmd->SetParameterName("min",false);
    functMinECmd->SetRange("min>=0.");
    functMinECmd->SetDefaultUnit("MeV");
    functMinECmd->AvailableForStates(G4State_Idle);
  }
  if(!functMaxECmd)
  {
    functMaxECmd = new G4UIcmdWithADoubleAndUnit("/generator/spectrum/setMaxEnergy",this);
    functMaxECmd->SetGuidance("Modify the maximum energy range of chosen spectrum.");
    functMaxECmd->SetGuidance("\tMust be a positive value.");
    functMaxECmd->SetParameterName("max",false);
    functMaxECmd->SetRange("max>=0.");
    functMaxECmd->SetDefaultUnit("MeV");
    functMaxECmd->AvailableForStates(G4State_Idle);
  }
  if(type == 2 && !functTempCmd)
  {
    functTempCmd = new G4UIcmdWithADoubleAndUnit("/generator/spectrum/setTemperature",this);
    functTempCmd->SetGuidance("Modify the ambient temperature of Maxwellian spectrum.");
    functTempCmd->SetGuidance("\tMust be a positive value.");
    functTempCmd->SetParameterName("temp",false);
    functTempCmd->SetRange("temp>0.");
    functTempCmd->SetDefaultUnit("kelvin");
    functTempCmd->AvailableForStates(G4State_Idle);
  }
}

	// ****** Input Command Instructions ****** //
void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
//  G4UImanager* UI = G4UImanager::GetUIpointer();
 if( command == cryInputCmd ) { 
     G4cerr << "CRY INPUT COMMAND " <<newValue<< G4endl;
     generator->InputCRY();
     (*MessInput).append(newValue);
     (*MessInput).append(" ");
   } else if( command == cryUpdateCmd ) { 
     generator->UpdateCRY(MessInput); 
     *MessInput = "";
   } else if( command == cryFileCmd ) {
     generator->CRYFromFile(newValue);
   } else if(command == testCmd) {
     generator->GenerateEnergySpectrumWithoutSimulation(testCmd->GetNewIntValue(newValue));
  } else if(command == calibOnCmd) {
    ToggleCalibrationMode(calibOnCmd->GetNewBoolValue(newValue));
    generator->ToggleCalibrationMode(calibOnCmd->GetNewBoolValue(newValue));
  }
  else if(command == cryOnCmd)
  {
    generator->SetCRY(cryOnCmd->GetNewBoolValue(newValue));
  }
 else if(command == cryPointCmd)
  {
    generator->SetCRYPoint(cryPointCmd->GetNewBoolValue(newValue));
  }
 else if(command == cryZCmd)
  {
    generator->SetCRYZOffset(cryZCmd->GetNewDoubleValue(newValue));
  }
  else if(command == verbCmd)
  {
    generator->SetVerbosity(verbCmd->GetNewIntValue(newValue));
  }
  else if(command == calSourceCmd)
  {
    G4int source = 0;
    if      (newValue == "none")  { source = 0; }
    else if (newValue == "Co60")  { source = 1; }
    else if (newValue == "Cs137") { source = 2; }
    else if (newValue == "Cf252") { source = 3; }
    else if (newValue == "LED")   { source = 4; }
    generator->SetCalibrationSource(source);
  }
  else if(command == calPosCmd)
  {
    generator->SetCalibrationPosition(calPosCmd->GetNew3VectorValue(newValue));
  }
  else if(command == gunCmd)
  {
    if      (newValue == "gun")
    { generator->SetGenerationWithGun(true); }
    else if (newValue == "source")
    { generator->SetGenerationWithGun(false); }
  }
  else if(command == functCmd)
  {
    if      (newValue == "cf252")   { generator->SetFunctionEnergySpectrum(1); }
    else if (newValue == "maxwell") { generator->SetFunctionEnergySpectrum(2); }
    InitializeSpectrumCommands();
  }
  else if(command == fileCmd)
  {
    generator->SetFile(newValue); 
    InitializeSpectrumCommands();
  }
  else if(command == functMinECmd)
  {
    generator->SetMinimumEnergyRange(functMinECmd->GetNewDoubleValue(newValue));
  }
  else if(command == functMaxECmd)
  {
    generator->SetMaximumEnergyRange(functMaxECmd->GetNewDoubleValue(newValue));
  }
  else if(command == functTempCmd)
  {
    generator->SetMaxwellianTemperature(functTempCmd->GetNewDoubleValue(newValue));
  }
  else if(command == customCmd)
  {
    generator->SetCustomEnergySpectrum(newValue);
  }
  else if(command == energyInactivateCmd)
  {
    ResetSpectrumCommands();
    generator->InactivateUserEnergySpectrum(energyInactivateCmd->GetNewBoolValue(newValue));
  }
  else if(command == moduleCmd)
  {
    generator->ResetSimulationModule();
    if      (newValue == "Options:")      { G4cout << moduleCmd->GetParameter(0)->GetParameterCandidates() << G4endl; }
    else if (newValue == "cosmicNeutron") { generator->SetCosmicNeutronFlag(true); }
    else if (newValue == "inverseBeta")   { generator->SetInverseBetaFlag(true); }
    else if (newValue == "fissionAntinu") { generator->SetFissionAntiNuFlag(true); }
    else if (newValue == "cosmicMuon")    { generator->SetCosmicMuonFlag(true); }
  }
  else if(command == moduleResetCmd)
  {
    generator->ResetSimulationModule();
  }
  else
  {
    G4cout << "Command not found." << G4endl;
  }
}

	// ****** Return Command String ****** //
G4String PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
  G4String cv;
  if(command == verbCmd)
  {
    cv = ItoS(generator->GetVerbosity());
  }
  else if(command == moduleCmd)
  {
    if      (generator->GetNeutronGenerator()) { cv = "cosmicNeutron"; }
    else if (generator->GetInverseGenerator()) { cv = "inverseBeta"; }
    else if (generator->GetAntiNuGenerator())  { cv = "fissionAntinu"; }
    else if (generator->GetMuonGenerator())    { cv = "cosmicMuon"; }
    else                                       { cv = "none"; }
  }
  return cv;
}

	// ****** Calibration Command Switch ****** //
void PrimaryGeneratorMessenger::ToggleCalibrationMode(G4bool flipOn)
{
  G4UImanager* UI_manager = G4UImanager::GetUIpointer();

  if(flipOn)
  {
    UI_manager->RemoveCommand(testCmd);
    UI_manager->RemoveCommand(gunCmd);
    UI_manager->RemoveCommand(gunDir);
    UI_manager->RemoveCommand(functCmd);
    UI_manager->RemoveCommand(fileCmd);
    UI_manager->RemoveCommand(customCmd);
    UI_manager->RemoveCommand(energyInactivateCmd);
    UI_manager->RemoveCommand(spectDir);
    UI_manager->RemoveCommand(moduleCmd);
    UI_manager->RemoveCommand(moduleResetCmd);
    UI_manager->RemoveCommand(moduleDir);

    delete testCmd; testCmd = 0;
    delete gunCmd; gunCmd = 0;
    delete functCmd; functCmd = 0;
    delete fileCmd; fileCmd = 0;
    delete customCmd; customCmd = 0;
    delete energyInactivateCmd; energyInactivateCmd = 0;
    delete moduleCmd; moduleCmd = 0;
    delete moduleResetCmd; moduleResetCmd = 0;

    delete gunDir; gunDir = 0;
    delete spectDir; spectDir = 0;
    delete moduleDir; moduleDir = 0;

    if(generator->GetFunction()) { ResetSpectrumCommands(); }

    InitializeCalibrationModeCommands();

	// Outputs large formatted text warning
    G4cout << "******************************* CAUTION *********************************\n"
           << "                  Entering Detector Calibration Mode.                    \n"
           << "                                                                         \n"
           << "     Configuration of the previous defined source specification will be  \n"
           << " kept, but it will not be used or modified until you leave calibration   \n"
           << " mode. All source specification commands in this mode should be input    \n"
           << " using the /gps commands. Only one calibration source spectrum may be    \n"
           << " used in any one run.                                                    \n"
           << " GPS set to source #2, an isotropic and mono-energetic point neutron     \n"
           << " source. The reference position (origin) of the geometry is the centre   \n"
           << " of the outer tank volume. Please do not modify or use sources #0 and #1 \n"
           << " in GPS as they have been designated for the predefined modules in the   \n"
           << " simulation. Note that all other custom commands have been disabled      \n"
           << " until you leave this mode.                                              \n"
           << "*************************************************************************\n"
           << G4endl;
  }
  else
  {
    ResetCalibrationModeCommands();

    InitializeBasicCommands();
    if(generator->GetFunction()) { InitializeSpectrumCommands(); }

	// Outputs large formatted text warning
    G4cout << "******************************* CAUTION *********************************\n"
           << "                   Leaving Detector Calibration Mode.                    \n"
           << "                                                                         \n"
           << "     Simulation settings should be reverted back to the state just prior \n"
           << " to entering calibration mode. However, it is strongly recommended that  \n"
           << " you double-check this before starting your next simulation run.         \n"
           << "*************************************************************************\n"
           << G4endl;
  }
}

	// ****** Reset Function Commands ****** //
void PrimaryGeneratorMessenger::ResetSpectrumCommands()
{
  G4UImanager* UI_manager = G4UImanager::GetUIpointer();
  UI_manager->RemoveCommand(functMinECmd);
  UI_manager->RemoveCommand(functMaxECmd);
  if(generator->GetDistribution() == "Temperature-Dependent Maxwellian Spectrum") { UI_manager->RemoveCommand(functTempCmd); }

  delete functMinECmd; functMinECmd = 0;
  delete functMaxECmd; functMaxECmd = 0;
  delete functTempCmd; functTempCmd = 0;
}

	// ****** Reset Calibration Commands ****** //
void PrimaryGeneratorMessenger::ResetCalibrationModeCommands()
{
  G4UImanager* UI_manager = G4UImanager::GetUIpointer();
  UI_manager->RemoveCommand(calSourceCmd);
  UI_manager->RemoveCommand(calPosCmd);
  UI_manager->RemoveCommand(calibDir);

  delete calSourceCmd; calSourceCmd = 0;
  delete calPosCmd; calPosCmd = 0;
  delete calibDir; calibDir = 0;
} 

// EOF
