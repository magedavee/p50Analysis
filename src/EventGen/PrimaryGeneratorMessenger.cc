#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "CosmicMuonGenerator.hh"
#include "CosmicCosineGenerator.hh"
#include "InverseBetaKinematics.hh"
#include "FissionAntiNuModule.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "G4ios.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* gen_action): generator(gen_action) {
        
    genDir = new G4UIdirectory("/generator/");
    genDir->SetGuidance("Custom simulation settings.");
    
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

    moduleCf252cmd = new G4UIcmdWithoutParameter("/generator/module_Cf252",this);
    moduleCf252cmd->SetGuidance("Use Cf252 neutron event generator");
    moduleCf252cmd->AvailableForStates(G4State_Idle);
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger() {
    delete genDir;
    
    delete verbCmd;
    
    delete moduleCRYcmd;
    delete moduleIBDcmd;
    delete moduleFisANucmd;
    delete moduleCosMucmd;
    delete moduleCosNcmd;
    delete moduleCf252cmd;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == verbCmd) generator->SetVerbosity(verbCmd->GetNewIntValue(newValue));
    else if(command == moduleCRYcmd) generator->loadCRYModule();
    else if(command == moduleIBDcmd) generator->loadIBDModule();
    else if(command == moduleFisANucmd) generator->loadFisAntNuModule();
    else if(command == moduleCosMucmd) generator->loadCosmicMuonModule();
    else if(command == moduleCosNcmd) generator->loadCosmicNeutronModule();
    else if(command == moduleCf252cmd) generator->loadCf252Module();
    else G4cout << "Command not found." << G4endl;
}
