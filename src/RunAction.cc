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

#include "RunAction.hh"

#include "RunMessenger.hh"
#include "EventAction.hh"
#include "RootIO.hh"
#include "XMLProvider.hh"

#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4ios.hh>

RunAction::RunAction(PrimaryGeneratorAction* g, DetectorConstruction* d):
XMLProvider("Run"), gen(g), det(d) {
    run_messenger = new RunMessenger(this);
}

RunAction::~RunAction() {
   delete run_messenger;
}

void RunAction::SetRunNumber(G4int rnum) {
    nRunNumber = rnum;
    G4RunManager::GetRunManager()->SetRunIDCounter(nRunNumber);
}

void RunAction::BeginOfRunAction(const G4Run* aRun) {
    G4cerr << "~~~~~ Run Number " << aRun->GetRunID() << " Initiated ~~~~~\n" << G4endl;
    CLHEP::HepRandom::setTheSeed(aRun->GetRunID()); // set unique random seed for run
}

void RunAction::EndOfRunAction(const G4Run* aRun) {
    G4int numOfEvents = aRun->GetNumberOfEvent();
    if (numOfEvents == 0) return;
    
    RootIO* R = RootIO::GetInstance();
    
    // set up XML output
    addChild(det);
    writeToFile(R->GetFileName()+".xml");
    
    R->WriteFile();
}
    

