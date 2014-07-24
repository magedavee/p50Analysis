// Unrestricted Use - Property of AECL
//
// RunAction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Custom Run Controller and Processing
//	Contains definitions of functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "RunAction.hh"			// Specifies the file which contains the class structure

#include "RunMessenger.hh"		// Specifies user-defined classes which are called upon in this class
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "LogSession.hh"
#include "RootIO.hh"

#include "G4Run.hh"			// Specifies all the classes which contain structures called upon in this class
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output
#include "globals.hh"			// Specifies the class defining all global parameters and variable types
#include <map>
#include <vector>

RunAction::RunAction() {
    gen = NULL;
    run_messenger = new RunMessenger(this);
}

RunAction::~RunAction() {
   delete run_messenger;
}

void RunAction::SetRunNumber(G4int rnum) {
    nRunNumber = rnum;
    G4RunManager* runManager = G4RunManager::GetRunManager();
    runManager->SetRunIDCounter(nRunNumber);
}

void RunAction::BeginOfRunAction(const G4Run* aRun) {
    G4cerr << "~~~~~ Run Number " << aRun->GetRunID() << " Initiated ~~~~~\n" << G4endl;
    if(!gen) gen = (PrimaryGeneratorAction*)G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
    CLHEP::HepRandom::setTheSeed(aRun->GetRunID()); // set unique random seed for run
}

void RunAction::EndOfRunAction(const G4Run* aRun) {
    G4int numOfEvents = aRun->GetNumberOfEvent();
    if (numOfEvents == 0) return;
    RootIO::GetInstance()->WriteFile();
}
    

