// Unrestricted Use - Property of AECL
//
// antinu.cc - created on 2010/06/09 by Aaron Ho
// GEANT4 - geant4.4.9.3.p01
//
// Main file for Template GEANT4 Simulation
// 	Contains instructions to build/run the Geant4 code
//
// ------------------------------------------------------------------
//	GEANT4 - Short Baseline Anti-Neutrino Detector
// ------------------------------------------------------------------
//

#include "G4RunManager.hh"			// Includes the class which controls the operation of the program
#include "G4UImanager.hh"			// Includes the class which controls the user command prompt of the program (run-time control is optional though)

#include "Randomize.hh"

#include "DetectorConstruction.hh"		// These are the user-defined classes describing geometry, physics, and initial kinetics
#include "PhysicsList.hh"			//	|
#include "PrimaryGeneratorAction.hh"		//	|
#include "RunAction.hh"				//	|
#include "EventAction.hh"			//	|
#include "TrackingAction.hh"			//	|
#include "SteppingAction.hh"			//	|
#include "SteppingVerbose.hh"			//	|
#include "LogSession.hh"			//	V

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <iostream>
#include <fstream>
#include "globals.hh"
#include "TSystem.h"

int main(int argc,char** argv) // Detailed Instructions on how to run program
{
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

  G4VUserPhysicsList* physics = new PhysicsList();	// G4VUser type class is used to initialize here because there are no unique functions defined belonging only to this user-defined class
  run_manager->SetUserInitialization(physics);

	// Set mandatory user action classes
  PrimaryGeneratorAction* generator = new PrimaryGeneratorAction();
  run_manager->SetUserAction(generator); 

	// Set optional user action classes
  RunAction* run_action = new RunAction();
  run_manager->SetUserAction(run_action);

  EventAction* event_action = new EventAction();
  run_manager->SetUserAction(event_action); 

  TrackingAction* tracking_action = new TrackingAction();
  run_manager->SetUserAction(tracking_action);

  SteppingAction* stepping_action = new SteppingAction();
  run_manager->SetUserAction(stepping_action);

	// Initialize G4 kernel
  run_manager->Initialize();

#ifdef G4VIS_USE
	// Initialize visualization
  G4VisManager* vis_manager = new G4VisExecutive;
  vis_manager->Initialize(); 
#endif

	// Get the pointer to the UI manager and creates new log session (overwrites old one!)
  G4UImanager* UI = G4UImanager::GetUIpointer(); 
  UI->ApplyCommand("/scint/setOpticalProcesses false");
  UI->ApplyCommand("/hits/inactivate");

	// Execute the argument macro file if it exists
  if(argc == 1)		// Condition checks if there is any arguments to program execution command
  {
    //    UI->ApplyCommand("/control/saveHistory False");
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
  else
    {
      G4String command = "/control/execute ";
      G4String macfile_name = argv[1];
      G4cerr<<"starting macro"<<G4endl;
      UI->ApplyCommand(command+macfile_name); 
    }
    
  // Job termination and freeing of storage space
#ifdef G4VIS_USE
  delete vis_manager;
#endif
  delete run_manager;	// All other processes owned and deleted by G4RunManager, do NOT delete them again!
  return 0;
}

// EOF
