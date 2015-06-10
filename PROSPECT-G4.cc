/// \file PROSPECT-G4.cc \brief main() function for PROSPECT-G4 executable.
#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <Randomize.hh>
#include <globals.hh>

#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#include "VisSetup.hh"
#endif

#ifdef G4UI_USE
#include <G4UIExecutive.hh>
#endif

#include <TSystem.h>
#include <iostream>
#include <fstream>

#include "DetectorConstruction.hh"
#include "FaNS2DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingVerbose.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"
#include "PhysicsList.hh"
#include "XSDump.hh"
#include "FileIO.hh"

/// main() function for PROSPECT-G4 simulation executable
int main(int argc,char** argv) {
    
    // load event classes for ROOT
    gSystem->Load("libEventLib.so");
    
    // User Verbose Class
    G4VSteppingVerbose::SetInstance(new SteppingVerbose); 
    
    // Construct the default run manager
    G4RunManager* run_manager = new G4RunManager; 
    run_manager->SetVerboseLevel(1);
    
    // Set mandatory initialization classes
    DetectorConstruction* detector = new DetectorConstruction();
    run_manager->SetUserInitialization(detector);
    
    // Set physics list
    PhysicsList* physics = new PhysicsList();
    run_manager->SetUserInitialization(physics);

    // Set mandatory user action classes
    PrimaryGeneratorAction* generator = new PrimaryGeneratorAction();
    run_manager->SetUserAction(generator); 
    
    ///////////////////////////////////
    // Set optional user action classes
    
    RunAction* run_action = new RunAction(generator, detector, physics);
    run_manager->SetUserAction(run_action);
    
    EventAction* event_action = new EventAction();
    run_manager->SetUserAction(event_action);
    
    TrackingAction* tracking_action = new TrackingAction();
    run_manager->SetUserAction(tracking_action);
    
    SteppingAction* stepping_action = new SteppingAction();
    run_manager->SetUserAction(stepping_action);
    
    XSDump myXSDump;
    
#ifdef G4VIS_USE
    // Initialize visualization
    G4VisManager* vis_manager = new G4VisExecutive;
    vis_manager->Initialize();
    VisSetup VS(vis_manager);
#endif
    
    // Get the pointer to the UI manager
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->SetMaxHistSize(1000);
    
    if(argc > 1) { // Execute the argument macro file if specified in arguments
        G4String command = "/control/execute ";
        G4String macfile_name = argv[1];
        G4cout << "Starting macro" << G4endl;
        UI->ApplyCommand(command+macfile_name); 
    } else { // otherwise, apply default initialization
#ifdef G4UI_USE
        UI->ApplyCommand("/control/verbose 0");
        UI->ApplyCommand("/run/verbose 0");
        UI->ApplyCommand("/event/verbose 0");
        UI->ApplyCommand("/tracking/verbose 0");
        
        G4cout << "Starting UI session" << G4endl;
        G4UIExecutive* ui = new G4UIExecutive(argc,argv);
        ui->SessionStart();
        delete ui;
        // TODO: figure out why program segfaults if exiting here before /run/initialize
#else
        G4cout << "Not compiled with UI available; please specify a macro." << G4endl;
#endif
    }
    
    myXSDump.makeXSdata();

    // Job termination and freeing of storage space
#ifdef G4VIS_USE
    delete vis_manager;
#endif
    delete run_manager;
    // All other processes owned and deleted by G4RunManager
    
    FileIO::GetInstance()->WriteFile();
    
    return 0;
}
