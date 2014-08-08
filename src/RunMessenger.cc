// Unrestricted Use - Property of AECL
//
// RunMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Run Action Messenger
//      Contains definitions for functions in header file
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "RunMessenger.hh"

#include "RunAction.hh"
#include "RootIO.hh"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4ios.hh>


RunMessenger::RunMessenger(RunAction* run_action): run(run_action) {
    outputDir = new G4UIdirectory("/output/");
    outputDir->SetGuidance("Simulation output settings.");
    outputDir->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);

    outNumCmd = new G4UIcmdWithAnInteger("/output/setRunNum",this);
    outNumCmd->SetGuidance("Set the run number");
    outNumCmd->SetDefaultValue(1);
    outNumCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
    
    recLevCmd = new G4UIcmdWithAnInteger("/output/setRecordLevel",this);
    recLevCmd->SetGuidance("Set the level at which events are recorded:\n0: when a photon is detected by the PMTs\n1: when energy is deposited in the scintillator\n2: When a particle enters the shield\n3: All events");
    recLevCmd->SetDefaultValue(0);
    recLevCmd->AvailableForStates(G4State_PreInit,G4State_Init, G4State_Idle);    
    
    outFileName = new G4UIcmdWithAString("/output/filename",this);
    outFileName->SetGuidance("ROOT data output filename");
    outFileName->AvailableForStates(G4State_Init, G4State_Idle);
}

RunMessenger::~RunMessenger() {
    delete outputDir;
    delete outNumCmd;
    delete recLevCmd;
    delete outFileName;
}

void RunMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == outNumCmd) {
        G4int rnum = outNumCmd->GetNewIntValue(newValue); 
        run->SetRunNumber(rnum); 
        G4cout << "Run number is " << rnum << G4endl;
    } else if(command == recLevCmd) {
        G4int recL = recLevCmd->GetNewIntValue(newValue);
        run->SetRecordLevel(recL);
        G4cout << "Record Level is " << recL << G4endl;
    } else if(command == outFileName) {
        RootIO::GetInstance()->SetFileName(newValue);
    } else G4cout << "Command not found." << G4endl;
}
