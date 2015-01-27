#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "CosmicMuonGenerator.hh"
#include "CosmicCosineGenerator.hh"
#include "InverseBetaKinematics.hh"
#include "DetectorConstruction.hh"

#include "G4ios.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* gen_action): generator(gen_action), 
genDir("/generator/"),
modDir("/generator/module/"),
posDir("/generator/vertex/"),
verbCmd("/generator/verbose",this),
moduleGuncmd("/generator/module/gun",this),
moduleCRYcmd("/generator/module/CRY",this),
moduleIBDcmd("/generator/module/IBD",this),
moduleCosMucmd("/generator/module/CosMu",this),
moduleCosNcmd("/generator/module/CosN",this),
moduleCf252cmd("/generator/module/Cf252",this),
moduleSimpleBGcmd("/generator/module/SimpleBG",this),
moduleThermalNcmd("/generator/module/ThermalN",this),
moduleGPScmd("/generator/module/gps",this),
moduleDecaySrccmd("/generator/module/decaysrc",this),
moduleHistocmd("/generator/module/histogram",this),
ptPosCmd("/generator/vertex/isotpt", this),
isotFluxCmd("/generator/vertex/isotworld", this),
srcTargCmd("/generator/vertex/srctarg", this),
scintSrcCmd("/generator/vertex/scintvol",this),
cosFluxCmd("/generator/vertex/cosx", this),
dirFluxCmd("/generator/vertex/directional", this) {
        
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
    
    moduleCosMucmd.SetGuidance("Use cosmic muon event generator");
    moduleCosMucmd.AvailableForStates(G4State_Idle);
    
    moduleCosNcmd.SetGuidance("Use cosmic neutron event generator");
    moduleCosNcmd.AvailableForStates(G4State_Idle);
    
    moduleCf252cmd.SetGuidance("Use Cf252 neutron event generator");
    moduleCf252cmd.AvailableForStates(G4State_Idle);

    moduleSimpleBGcmd.SetGuidance("Use SimpleBG gamma event generator");
    moduleSimpleBGcmd.AvailableForStates(G4State_Idle);

    moduleThermalNcmd.SetGuidance("Use thermal neutron event generator");
    moduleThermalNcmd.AvailableForStates(G4State_Idle);

    moduleGPScmd.SetGuidance("Use G4GeneralParticleSource generator");
    moduleGPScmd.AvailableForStates(G4State_Idle);
    
    moduleDecaySrccmd.SetGuidance("Use nuclear decay source event generator");
    moduleDecaySrccmd.AvailableForStates(G4State_Idle);
    
    moduleHistocmd.SetGuidance("Use histogram event generator");
    moduleHistocmd.AvailableForStates(G4State_Idle);
    
    ptPosCmd.SetGuidance("Generate events with isotropic momenta from specified point");
    ptPosCmd.AvailableForStates(G4State_Idle);
    
    isotFluxCmd.SetGuidance("Generate vertices for isotropic flux from world volume surface");
    isotFluxCmd.AvailableForStates(G4State_Idle);
    
    srcTargCmd.SetGuidance("Generate vertices from source to target volume");
    srcTargCmd.AvailableForStates(G4State_Idle);
    
    scintSrcCmd.SetGuidance("Generate vertices uniformly in scintillator volume");
    scintSrcCmd.AvailableForStates(G4State_Idle);
    
    cosFluxCmd.SetGuidance("Generate cos^x-weighted hemispherical flux from world volume");
    cosFluxCmd.AvailableForStates(G4State_Idle);
    
    dirFluxCmd.SetGuidance("Generate directional flux from world volume");
    dirFluxCmd.AvailableForStates(G4State_Idle);
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &verbCmd) generator->SetVerbosity(verbCmd.GetNewIntValue(newValue));
    else if(command == &moduleGuncmd) generator->loadGunModule();
    else if(command == &moduleCRYcmd) generator->loadCRYModule();
    else if(command == &moduleIBDcmd) generator->loadIBDModule();
    else if(command == &moduleCosMucmd) generator->loadCosmicMuonModule();
    else if(command == &moduleCosNcmd) generator->loadCosmicNeutronModule();
    else if(command == &moduleCf252cmd) generator->loadCf252Module();
    else if(command == &moduleSimpleBGcmd) generator->loadSimpleBGModule();
    else if(command == &moduleThermalNcmd) generator->loadThermalNModule();
    else if(command == &moduleGPScmd) generator->loadGPSModule();
    else if(command == &moduleDecaySrccmd) generator->loadDecaySourceModule();
    else if(command == &moduleHistocmd) generator->loadHistogramModule();
    
    else if(command == &ptPosCmd) {
        generator->myPositioner = &generator->myIsotPt;
        generator->myIsotPt.x0 = ptPosCmd.GetNew3VectorValue(newValue);
    } else if(command == &isotFluxCmd || command == &cosFluxCmd) {
        generator->myPositioner = generator->GetCosineThrower();
        generator->GetCosineThrower()->setSourceTarget(NULL,NULL);
        generator->GetCosineThrower()->setExponent(command == &cosFluxCmd? cosFluxCmd.GetNewDoubleValue(newValue) : 0);
    } else if(command == &srcTargCmd) {
        generator->myPositioner = generator->GetCosineThrower();
        generator->GetCosineThrower()->fromVolume = true;
        generator->GetCosineThrower()->setSourceTarget(generator->GetDetector()->ptclSrc, generator->GetDetector()->ptclTrg);
        generator->GetCosineThrower()->setExponent(0);
    } else if(command == &scintSrcCmd) {
        ScintSegVol* V = generator->GetDetector()->getScint();
        G4VPhysicalVolume* scph = V? V->scint_phys : NULL;
        if(!scph) { G4cout << "Scintillator not defined in this geometry!" << G4endl; return; }
        generator->myPositioner = generator->GetCosineThrower();
        generator->GetCosineThrower()->fromVolume = true;
        generator->GetCosineThrower()->setSourceTarget(scph, NULL);
        generator->GetCosineThrower()->setExponent(0);
    } else if(command == &dirFluxCmd) {
        generator->myPositioner = generator->GetDirectionThrower();
        generator->GetDirectionThrower()->setDirection(dirFluxCmd.GetNew3VectorValue(newValue));
    }

    else G4cout << "Command not found." << G4endl;
}
