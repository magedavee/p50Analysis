// Unrestricted Use - Property of AECL
//
// EventAction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Custom Event Controller and Processing
// 	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
//  Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "EventAction.hh"

// User-defined classes which are called upon in this class
#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RootIO.hh"
#include "Event.hh"

// Geant4 structures called upon in this class
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"
#include "G4HCofThisEvent.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4THitsMap.hh"
#include "Randomize.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleGun.hh"

#include "G4ios.hh"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>

#include "globals.hh"

void EventAction::BeginOfEventAction(const G4Event* anEvent) {
    
    // Displays event number and random seed for select events - gives user a progress report
    G4int eventNumber = anEvent->GetEventID();
    if(!(eventNumber % 1000)) {
        G4cout << "   Beginning of event: " << eventNumber << G4endl;
        CLHEP::HepRandom::showEngineStatus();
    }

    // Initialize HC pointers at the start of every event
    G4SDManager* sd_manager = G4SDManager::GetSDMpointerIfExist();
    if(sd_manager) {
        //InnerHCIDEDep = sd_manager->GetCollectionID("scintHitInner/IoniseCollection");
    }
    
    // Clear prior event data
    Event& evt =  RootIO::GetInstance()->GetEvent();
    evt.Clear();
    
    // record event primaries information
    evt.N = eventNumber;
    for(G4int i=0; i<anEvent->GetNumberOfPrimaryVertex(); i++) {
        const G4PrimaryVertex* v = anEvent->GetPrimaryVertex(i); 
        assert(v);
        EventPrimaryPtcl p;
        for(uint j=0; j<3; j++) p.x[j] = v->GetPosition()[j];
        p.t = v->GetT0();
        for(G4int pn = 0; pn < v->GetNumberOfParticle(); pn++) {
            const G4PrimaryParticle* pp = v->GetPrimary(pn);
            assert(pp);
            G4ThreeVector mom = pp->GetMomentum();
            G4double m = pp->GetMass();
            p.E = sqrt(mom.mag2()+m*m)-m;
            mom = mom.unit();
            for(uint j=0; j<3; j++) p.p[j] = mom[j];
            p.PID = pp->GetPDGcode();
            evt.AddPrimary(p);
        }
    }
}

void EventAction::EndOfEventAction(const G4Event*) {
    // Save event data
    RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
    G4int reclevel = run_action->GetRecordLevel();
    Event& evt = RootIO::GetInstance()->GetEvent();
    if(reclevel >= 3 || (reclevel >= 2 && evt.nIoniClusts > 0))
        RootIO::GetInstance()->FillTree();
}
