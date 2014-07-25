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
PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* gen_action): generator(gen_action) {
        
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
    
    /////////////////////////////////
    // event generator module loaders
    
    moduleCRYcmd = new G4UIcmdWithoutParameter("/generator/module_CRY",this);
    moduleCRYcmd->SetGuidance("Use CRY event generator");
    moduleCRYcmd->AvailableForStates(G4State_Idle);
    
    moduleIBDcmd = new G4UIcmdWithoutParameter("/generator/module_IBD",this);
    moduleIBDcmd->SetGuidance("Use Inverse Beta Decay event generator");
    moduleIBDcmd->AvailableForStates(G4State_Idle);
    
    moduleFisANucmd = new G4UIcmdWithoutParameter("/generator/module_FisANu",this);
    moduleFisANucmd->SetGuidance("Use fission anti-neutrino event generator");
    moduleFisANucmd->AvailableForStates(G4State_Idle);
    
    moduleCosMucmd = new G4UIcmdWithoutParameter("/generator/module_CosMu",this);
    moduleCosMucmd->SetGuidance("Use cosmic muon event generator");
    moduleCosMucmd->AvailableForStates(G4State_Idle);
    
    moduleCosNcmd = new G4UIcmdWithoutParameter("/generator/module_CosN",this);
    moduleCosNcmd->SetGuidance("Use cosmic neutron event generator");
    moduleCosNcmd->AvailableForStates(G4State_Idle);
    
    
    
    InitializeBasicCommands();
    
    calibDir = 0;
    calSourceCmd = 0;
    calPosCmd = 0;
    
    functMinECmd = 0;
    functMaxECmd = 0;
    functTempCmd = 0;
}

// ****** Destructor ****** //
PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger() {
    delete genDir;
    if(calibDir) { delete calibDir; }
    if(gunDir) { delete gunDir; }
    if(spectDir) { delete spectDir; }
    
    delete calibOnCmd;
    if(calSourceCmd) { delete calSourceCmd; }
    if(calPosCmd) { delete calPosCmd; }
    
    delete moduleCRYcmd;
    delete moduleIBDcmd;
    delete moduleFisANucmd;
    
    delete verbCmd;
    if(gunCmd) { delete gunCmd; }
    if(functCmd) { delete functCmd; }
    if(fileCmd) { delete fileCmd; }
    if(functMinECmd) { delete functMinECmd; }
    if(functMaxECmd) { delete functMaxECmd; }
    if(functTempCmd) { delete functTempCmd; }
    if(customCmd) { delete customCmd; }
    if(energyInactivateCmd) { delete energyInactivateCmd; }
}

// ****** Initialize Standard Commands ****** //
void PrimaryGeneratorMessenger::InitializeBasicCommands() {
    
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
}

// ****** Initialize Calibration Commands ****** //
void PrimaryGeneratorMessenger::InitializeCalibrationModeCommands() {
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
void PrimaryGeneratorMessenger::InitializeSpectrumCommands() {
    G4int type = generator->GetFunction(); if(type == 0) { return; }
    if(!functMinECmd) {
        functMinECmd = new G4UIcmdWithADoubleAndUnit("/generator/spectrum/setMinEnergy",this);
        functMinECmd->SetGuidance("Modify the minimum energy range of chosen spectrum.");
        functMinECmd->SetGuidance("\tMust be a positive value.");
        functMinECmd->SetParameterName("min",false);
        functMinECmd->SetRange("min>=0.");
        functMinECmd->SetDefaultUnit("MeV");
        functMinECmd->AvailableForStates(G4State_Idle);
    }
    if(!functMaxECmd) {
        functMaxECmd = new G4UIcmdWithADoubleAndUnit("/generator/spectrum/setMaxEnergy",this);
        functMaxECmd->SetGuidance("Modify the maximum energy range of chosen spectrum.");
        functMaxECmd->SetGuidance("\tMust be a positive value.");
        functMaxECmd->SetParameterName("max",false);
        functMaxECmd->SetRange("max>=0.");
        functMaxECmd->SetDefaultUnit("MeV");
        functMaxECmd->AvailableForStates(G4State_Idle);
    }
    if(type == 2 && !functTempCmd) {
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
   if(command == calibOnCmd) {
        ToggleCalibrationMode(calibOnCmd->GetNewBoolValue(newValue));
        generator->ToggleCalibrationMode(calibOnCmd->GetNewBoolValue(newValue));
    } else if(command == verbCmd) generator->SetVerbosity(verbCmd->GetNewIntValue(newValue));
    else if(command == moduleCRYcmd) generator->loadCRYModule();
    else if(command == moduleIBDcmd) generator->loadIBDModule();
    else if(command == moduleFisANucmd) generator->loadFisAntNuModule();
    else if(command == moduleCosMucmd) generator->loadCosmicMuonModule();
    else if(command == moduleCosNcmd) generator->loadCosmicNeutronModule();
    else if(command == calSourceCmd) {
        G4int source = 0;
        if      (newValue == "none")  { source = 0; }
        else if (newValue == "Co60")  { source = 1; }
        else if (newValue == "Cs137") { source = 2; }
        else if (newValue == "Cf252") { source = 3; }
        else if (newValue == "LED")   { source = 4; }
        generator->SetCalibrationSource(source);
    } else if(command == calPosCmd) generator->SetCalibrationPosition(calPosCmd->GetNew3VectorValue(newValue));
    else if(command == gunCmd) {
        if      (newValue == "gun")
        { generator->SetGenerationWithGun(true); }
        else if (newValue == "source")
        { generator->SetGenerationWithGun(false); }
    } else if(command == functCmd) {
        if      (newValue == "cf252")   { generator->SetFunctionEnergySpectrum(1); }
        else if (newValue == "maxwell") { generator->SetFunctionEnergySpectrum(2); }
        InitializeSpectrumCommands();
    } else if(command == functMinECmd) generator->SetMinimumEnergyRange(functMinECmd->GetNewDoubleValue(newValue));
    else if(command == functMaxECmd) generator->SetMaximumEnergyRange(functMaxECmd->GetNewDoubleValue(newValue));
    else if(command == functTempCmd) generator->SetMaxwellianTemperature(functTempCmd->GetNewDoubleValue(newValue));
    else if(command == customCmd) generator->SetCustomEnergySpectrum(newValue);
    else if(command == energyInactivateCmd) {
        ResetSpectrumCommands();
        generator->InactivateUserEnergySpectrum(energyInactivateCmd->GetNewBoolValue(newValue));
    } else G4cout << "Command not found." << G4endl;
}

// ****** Calibration Command Switch ****** //
void PrimaryGeneratorMessenger::ToggleCalibrationMode(G4bool flipOn) {
    G4UImanager* UI_manager = G4UImanager::GetUIpointer();
    
    if(flipOn) {
        UI_manager->RemoveCommand(gunCmd);
        UI_manager->RemoveCommand(gunDir);
        UI_manager->RemoveCommand(functCmd);
        UI_manager->RemoveCommand(fileCmd);
        UI_manager->RemoveCommand(customCmd);
        UI_manager->RemoveCommand(energyInactivateCmd);
        UI_manager->RemoveCommand(spectDir);
        
        delete gunCmd; gunCmd = 0;
        delete functCmd; functCmd = 0;
        delete fileCmd; fileCmd = 0;
        delete customCmd; customCmd = 0;
        delete energyInactivateCmd; energyInactivateCmd = 0;
        
        delete gunDir; gunDir = 0;
        delete spectDir; spectDir = 0;
        
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
void PrimaryGeneratorMessenger::ResetSpectrumCommands() {
    G4UImanager* UI_manager = G4UImanager::GetUIpointer();
    UI_manager->RemoveCommand(functMinECmd);
    UI_manager->RemoveCommand(functMaxECmd);
    if(generator->GetDistribution() == "Temperature-Dependent Maxwellian Spectrum") { UI_manager->RemoveCommand(functTempCmd); }
    
    delete functMinECmd; functMinECmd = 0;
    delete functMaxECmd; functMaxECmd = 0;
    delete functTempCmd; functTempCmd = 0;
}

// ****** Reset Calibration Commands ****** //
void PrimaryGeneratorMessenger::ResetCalibrationModeCommands() {
    G4UImanager* UI_manager = G4UImanager::GetUIpointer();
    UI_manager->RemoveCommand(calSourceCmd);
    UI_manager->RemoveCommand(calPosCmd);
    UI_manager->RemoveCommand(calibDir);
    
    delete calSourceCmd; calSourceCmd = 0;
    delete calPosCmd; calPosCmd = 0;
    delete calibDir; calibDir = 0;
} 

// EOF
