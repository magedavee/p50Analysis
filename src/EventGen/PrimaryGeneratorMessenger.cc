#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "CosmicMuonGenerator.hh"
#include "CosmicCosineGenerator.hh"
#include "InverseBetaKinematics.hh"
#include "FissionAntiNuModule.hh"

#include "G4ios.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* gen_action): generator(gen_action), 
genDir("/generator/"), 
verbCmd("/generator/verbose",this),
moduleGuncmd("/generator/module_gun",this),
moduleCRYcmd("/generator/module_CRY",this),
moduleIBDcmd("/generator/module_IBD",this),
moduleFisANucmd("/generator/module_FisANu",this),
moduleCosMucmd("/generator/module_CosMu",this),
moduleCosNcmd("/generator/module_CosN",this),
moduleCf252cmd("/generator/module_Cf252",this),
moduleSimpleBGcmd("/generator/module_SimpleBG",this),
moduleGPScmd("/generator/module_gps",this) {
        
    genDir.SetGuidance("Custom simulation settings.");
    
    verbCmd.SetGuidance("Set the verbosity of this module");
    verbCmd.SetGuidance("    0 = silent, 1 = minimal, 2 = loud");
    verbCmd.SetGuidance("    Entries less than 0 will count as 0");
    verbCmd.SetGuidance("    Entries greater than 0 will also output generated values");
    verbCmd.SetParameterName("v",false);
    verbCmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    /////////////////////////////////
    // event generator module loaders
    
    moduleGuncmd.SetGuidance("Use default particle gun generator");
    moduleGuncmd.AvailableForStates(G4State_Idle);
    
    moduleCRYcmd.SetGuidance("Use CRY event generator");
    moduleCRYcmd.AvailableForStates(G4State_Idle);
    
    moduleIBDcmd.SetGuidance("Use Inverse Beta Decay event generator");
    moduleIBDcmd.AvailableForStates(G4State_Idle);
    
    moduleFisANucmd.SetGuidance("Use fission anti-neutrino event generator");
    moduleFisANucmd.AvailableForStates(G4State_Idle);
    
    moduleCosMucmd.SetGuidance("Use cosmic muon event generator");
    moduleCosMucmd.AvailableForStates(G4State_Idle);
    
    moduleCosNcmd.SetGuidance("Use cosmic neutron event generator");
    moduleCosNcmd.AvailableForStates(G4State_Idle);
    
    moduleCf252cmd.SetGuidance("Use Cf252 neutron event generator");
    moduleCf252cmd.AvailableForStates(G4State_Idle);

    moduleSimpleBGcmd.SetGuidance("Use SimpleBG gamma event generator");
    moduleSimpleBGcmd.AvailableForStates(G4State_Idle);

    moduleGPScmd.SetGuidance("Use G4GeneralParticleSource generator");
    moduleGPScmd.AvailableForStates(G4State_Idle);
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &verbCmd) generator->SetVerbosity(verbCmd.GetNewIntValue(newValue));
    else if(command == &moduleGuncmd) generator->loadGunModule();
    else if(command == &moduleCRYcmd) generator->loadCRYModule();
    else if(command == &moduleIBDcmd) generator->loadIBDModule();
    else if(command == &moduleFisANucmd) generator->loadFisAntNuModule();
    else if(command == &moduleCosMucmd) generator->loadCosmicMuonModule();
    else if(command == &moduleCosNcmd) generator->loadCosmicNeutronModule();
    else if(command == &moduleCf252cmd) generator->loadCf252Module();
    else if(command == &moduleSimpleBGcmd) generator->loadSimpleBGModule();
    else if(command == &moduleGPScmd) generator->loadGPSModule();

    else G4cout << "Command not found." << G4endl;
}
