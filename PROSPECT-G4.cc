#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4PhysListFactory.hh>
#include <Randomize.hh>
#include <globals.hh>
#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif
#ifdef G4UI_USE
#include <G4UIExecutive.hh>
#endif

#include <TSystem.h>
#include <iostream>
#include <fstream>

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"

int main(int argc,char** argv) {
    
    // load event classes for ROOT
    gSystem->Load("libEventLib.so");
    
    // Sets random engine as Ranecu Engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine); 
    
    // User Verbose Class
    G4VSteppingVerbose::SetInstance(new SteppingVerbose); 
    
    // Construct the default run manager
    G4RunManager* run_manager = new G4RunManager; 
    run_manager->SetVerboseLevel(1);
    
    // Set mandatory initialization classes
    DetectorConstruction* detector = new DetectorConstruction();
    run_manager->SetUserInitialization(detector);
    
    // Set physics list
    G4PhysListFactory factory;
    G4VModularPhysicsList* physList = factory.GetReferencePhysList("QGSP_BERT_HP");
    run_manager->SetUserInitialization(physList);

    // Set mandatory user action classes
    PrimaryGeneratorAction* generator = new PrimaryGeneratorAction();
    run_manager->SetUserAction(generator); 
    
    ///////////////////////////////////
    // Set optional user action classes
    
    RunAction* run_action = new RunAction(generator, detector);
    run_manager->SetUserAction(run_action);
    
    EventAction* event_action = new EventAction();
    run_manager->SetUserAction(event_action); 
    
    TrackingAction* tracking_action = new TrackingAction();
    run_manager->SetUserAction(tracking_action);
    
    SteppingAction* stepping_action = new SteppingAction();
    run_manager->SetUserAction(stepping_action);
    
#ifdef G4VIS_USE
    // Initialize visualization
    G4VisManager* vis_manager = new G4VisExecutive;
    vis_manager->Initialize(); 
#endif
    
    // Get the pointer to the UI manager
    G4UImanager* UI = G4UImanager::GetUIpointer();
    
    if(argc > 1) { // Execute the argument macro file if specified in arguments
        G4String command = "/control/execute ";
        G4String macfile_name = argv[1];
        G4cerr << "starting macro" << G4endl;
        UI->ApplyCommand(command+macfile_name); 
    } else { // otherwise, apply default initialization
        UI->ApplyCommand("/control/verbose 0");
        UI->ApplyCommand("/run/verbose 0");
        UI->ApplyCommand("/event/verbose 0");
        UI->ApplyCommand("/tracking/verbose 0");
        
        // List all sensitive detector
        UI->ApplyCommand("/hits/list"); 
        UI->ApplyCommand("/output/list");
#ifdef G4UI_USE
        G4UIExecutive* ui = new G4UIExecutive(argc,argv);
        ui->SessionStart();
        delete ui;
#endif
    }

    // Job termination and freeing of storage space
#ifdef G4VIS_USE
    delete vis_manager;
#endif
    delete run_manager;
    // All other processes owned and deleted by G4RunManager
    
    return 0;
}
