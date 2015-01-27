#include "InverseBetaMessenger.hh"
#include "InverseBetaKinematics.hh"

#include <cassert>

#include <G4Tokenizer.hh>
#include <G4UImanager.hh>
#include <G4ios.hh>


InverseBetaMessenger::InverseBetaMessenger(InverseBetaKinematics* inv_beta): generator(inv_beta) {
    
    fissNuDir = new G4UIdirectory("/generator/module/fissionAntinu/");
    fissNuDir->SetGuidance("Reactor Antineutrino Module settings.");
    fissNuDir->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);
    
    G4UIparameter* param;               // Custom command definition
    
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
    
    fisDwyerCmd = new G4UIcmdWithABool("/generator/module/fissionAntinu/DwyerSpectrum",this);
    fisDwyerCmd->SetGuidance("Enable/Disable Dan Dwyer's antineutrino spectra.");
    fisDwyerCmd->SetParameterName("toggle",true);
    fisDwyerCmd->SetDefaultValue(true);
    fisDwyerCmd->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);
    
    fisPrintCmd = new G4UIcmdWithoutParameter("/generator/module/fissionAntinu/printParameters",this);
    fisPrintCmd->SetGuidance("Prints all user-defined parameters for the Reactor Antineutrino Module.");
    fisPrintCmd->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);
    
    
    invBetaDir = new G4UIdirectory("/generator/module/inverseBeta/");
    invBetaDir->SetGuidance("Antineutrino Inverse Beta Decay Product Module commands.");
    invBetaDir->AvailableForStates(G4State_Idle,G4State_GeomClosed,G4State_EventProc);
    
    invMonoECmd = new G4UIcmdWithADoubleAndUnit("/generator/module/inverseBeta/setAntinuEnergy",this);
    invMonoECmd->SetGuidance("Set energy of mono-energetic antineutrinos.");
    invMonoECmd->SetParameterName("nuEnergy",true);
    invMonoECmd->SetDefaultValue(2.0);
    invMonoECmd->SetDefaultUnit("MeV");
    invMonoECmd->AvailableForStates(G4State_Idle);
    
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

InverseBetaMessenger::~InverseBetaMessenger() {
    ResetModuleCommands();
    
    delete fissNuDir;
    delete fisProdCmd;
    delete fisPrintCmd;
    
    delete invBetaDir;
    delete invMonoECmd;
    delete invProdCmd;
    delete invDirectCmd;
    delete invVerbCmd;
    delete invPrintCmd;
}

// ****** Input Command Instructions ****** //
void InverseBetaMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == invMonoECmd) generator->SetAntiNeutrinoMonoEnergy(invMonoECmd->GetNewDoubleValue(newValue));
    else if(command == invProdCmd) {
        G4Tokenizer next(newValue); // Tokenizer splits input string into delimited sections
        G4double setU = StoD(next()); G4double setUr = StoD(next()); G4double setPu = StoD(next()); G4double setPU = StoD(next());
        G4cout << "Simulation to be set to use antineutrino spectrum from fissions in the following split \n" 
        << "--- U235: " << setU << " U238: " << setUr << " Pu239: " << setPu << " Pu241: " << setPU << " ---" << G4endl;
        generator->SetAntiNeutrinoSpectrum(setU,setUr,setPu,setPU); 
        G4cout << "Simulation set to use antineutrino spectrum from fissions in the following split \n" 
        << "--- U235: " << setU << " U238: " << setUr << " Pu239: " << setPu << " Pu241: " << setPU << " ---" << G4endl;
    } else if(command == invDirectCmd) assert(false); // TODO generator->SetAntiNeutrinoDirection(invDirectCmd->GetNew3VectorValue(newValue));
    else if(command == invVerbCmd) generator->SetVerbosity(invVerbCmd->GetNewIntValue(newValue));
    else if(command == invPrintCmd) generator->PrintAllParameters();
    else if(command == fisProdCmd) {
        G4Tokenizer next(newValue);
        G4double setU = StoD(next()); G4double setUr = StoD(next()); G4double setPu = StoD(next()); G4double setPU = StoD(next());
        generator->SetAntiNeutrinoSpectrum(setU,setUr,setPu,setPU); 
    } else if(command == fisDwyerCmd) {
        G4cerr<<"Using Dwyer/Langford fission spectra"<<G4endl;
        generator->SetDwyer(fisDwyerCmd->GetNewBoolValue(newValue));
    }  else if(command == fisPrintCmd) generator->PrintAllParameters();
    else G4cout << "Command not found." << G4endl;
}

void InverseBetaMessenger::ResetModuleCommands() {
    G4UImanager* UI_manager = G4UImanager::GetUIpointer();
    
    UI_manager->RemoveCommand(invMonoECmd);
    UI_manager->RemoveCommand(invProdCmd);
    UI_manager->RemoveCommand(invDirectCmd);
    UI_manager->RemoveCommand(invVerbCmd);
    UI_manager->RemoveCommand(invPrintCmd);
    UI_manager->RemoveCommand(invBetaDir);
}
