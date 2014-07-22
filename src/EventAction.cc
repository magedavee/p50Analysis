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
#include "EventMessenger.hh"
#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "InverseBetaKinematics.hh"
#include "NeutronHit.hh"
#include "OpticalHit.hh"
#include "IonisationHit.hh"
#include "ProtonHit.hh"
#include "LogSession.hh"
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

#include "globals.hh"

EventAction::EventAction() {
    // Pointers
    event_messenger = new EventMessenger(this);
    
    // Event-by-event statistics
    EventTime = 0;
    
    // Print Per Event Setting
    fPrintPerEvent = 0;
    
    // Print Modifiers
    fFirstNeutronRawPrint = true;
    fFirstLSPhotonRawPrint = true;
}

EventAction::~EventAction() {
    if(EventTime) delete EventTime;
    delete event_messenger;
}

// ****** Pre-Event Processing ****** //
void EventAction::BeginOfEventAction(const G4Event* anEvent) {
    
    // Displays event number and random seed for select events - gives user a progress report
    G4int eventNumber = anEvent->GetEventID();
    if(!(eventNumber % 1000)) {
        G4cout << "   Beginning of event: " <<eventNumber << G4endl;
        CLHEP::HepRandom::showEngineStatus();
    }

    // Reset print modifiers if this is the first event in a run
    if(!eventNumber) {
        fFirstNeutronRawPrint = true;
        fFirstLSPhotonRawPrint = true;
    }

    // Initialize HC pointers at the start of every event
    G4SDManager* sd_manager = G4SDManager::GetSDMpointerIfExist(); // This function refers to a Geant4 global variable so no need to use new operator
    G4String colName;

    if(sd_manager) {
        InnerHCIDNHit = sd_manager->GetCollectionID(colName="scintHitInner/NHitCollection");
        PMTHCIDPhotHit = sd_manager->GetCollectionID(colName="SegmentPMT/SegmentHitCollection");
        //  VetoHCIDPhotHit = sd_manager->GetCollectionID(colName="vetoPMT/HitCollection");
        InnerHCIDEDep = sd_manager->GetCollectionID(colName="scintHitInner/IoniseCollection");
        //   VetoHCIDEDep = sd_manager->GetCollectionID(colName="scintHitVeto/IoniseCollection");
        InnerHCIDPHit = sd_manager->GetCollectionID(colName="scintHitInner/PHitCollection");
    }

    // Initialize event time histogram at the start of every event if specified
    if(EventTime) {
        RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
        G4double tBin = run_action->GetPhotonTravelTimeBinWidth();
        G4double tRef = run_action->GetPhotonTravelTimeLowerRef();
        G4double tNum = run_action->GetPhotonTravelTimeBinCount();

        for(int j = 1; j <= tNum; j++) {
            G4double tUpper = tRef + j*tBin;
            (*EventTime)[tUpper] = 0;
        }
        (*EventTime)[999.*s] = 0;
    }
}

// ****** Post-Event Processing ****** //
void EventAction::EndOfEventAction(const G4Event* anEvent) {
   
    DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    G4HCofThisEvent* HCE = anEvent->GetHCofThisEvent();
    G4SDManager* sd_manager = G4SDManager::GetSDMpointerIfExist();
    RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
    
    // Initialize hit collection pointers for event analysis
    NeutronHitsCollection* InnerNHitHCE = 0;
    OpticalHitsCollection* PMTPhotHitHCE = 0;
    //OpticalHitsCollection* VetoPhotHitHCE = 0;
    IonisationHitsCollection* InnerEDepHCE = 0;
    // IonisationHitsCollection* VetoEDepHCE = 0;
    ProtonHitsCollection* InnerPHitHCE = 0;
    
    // Retrieve run modifiers for output file naming
    G4String pre = run_action->GetFilePrefix();
    G4String suf = run_action->GetFileSuffix();
    
    // Acquire hit collections from event processing
    if(HCE && sd_manager) {
        InnerNHitHCE = (NeutronHitsCollection*)(HCE->GetHC(InnerHCIDNHit));
        PMTPhotHitHCE = (OpticalHitsCollection*)(HCE->GetHC(PMTHCIDPhotHit));
        //  VetoPhotHitHCE = (OpticalHitsCollection*)(HCE->GetHC(VetoHCIDPhotHit));
        InnerEDepHCE = (IonisationHitsCollection*)(HCE->GetHC(InnerHCIDEDep));
        //   VetoEDepHCE = (IonisationHitsCollection*)(HCE->GetHC(VetoHCIDEDep));
        InnerPHitHCE = (ProtonHitsCollection*)(HCE->GetHC(InnerHCIDPHit));
    }
  
    /* --------------- Neutron Statistics ---------------- */
    if(InnerNHitHCE && InnerNHitHCE->entries()) {
        G4int nCountInner = InnerNHitHCE->entries(); // Counter for neutron population in sensitive volume
        G4int nCapCountInner = nCountInner;
        for(int i = 0; i < nCountInner; i++) {
            G4double energy = (*InnerNHitHCE)[i]->GetEnergy();
            G4double time = (*InnerNHitHCE)[i]->GetTrackTime();
            G4ThreeVector initPos = (*InnerNHitHCE)[i]->GetInitialPosition();
            //      G4bool left = (*InnerNHitHCE)[i]->GetLeft();
        
            // Determines if neutron was captured
            if((*InnerNHitHCE)[i]->GetCaptured()) {
            
                // Record initial neutron energy and start tracking track time (entry into tank)
                G4int nGammas = (*InnerNHitHCE)[i]->GetGammasGenerated();
                run_action->PassNCEnergy(energy);
                run_action->PassNCTime(time);
                
                // Record neutron thermalization time - accounts for re-entry into sensitive volume
                G4double therm = (*InnerNHitHCE)[i]->GetThermalizationTime();
                run_action->PassNThermTime(therm);
                
                // Record gamma generation by each capture - only direct emission by Gd
                if(nGammas > 0) { run_action->PassGammasCreated(nGammas); }
                
                // Record absolute distance to capture - does not account for direction
                G4ThreeVector finPos = (*InnerNHitHCE)[i]->GetFinalPosition();
                G4ThreeVector delta = finPos - initPos;
                G4double dist = delta.mag();
                run_action->PassNCDistance(dist);
                run_action->WriteNCPosition(finPos, run_action->GetEventNumberOffset() + anEvent->GetEventID());
            
                // Outputs raw data if requested
                if(run_action->GetNeutronCaptureRawDataOutput()) {
                    LogSession* log = LogSession::GetLogSessionPointer();
                    log->SetOutputFileName(pre+"NeutronCapturePerEvent"+suf+".txt");
                    log->OpenFile(fFirstNeutronRawPrint,!fFirstNeutronRawPrint);
                    (*log) << "Event Number: " << run_action->GetEventNumberOffset() + anEvent->GetEventID()
                        << "\tNeutron Energy:\t" << (energy/MeV) << " MeV"
                        << "\tCapture Time:\t" << (time/microsecond) << " us"
                        << "\tThermalization Time:\t" << (therm/microsecond) << " us" << "\n"
                        << "\tCapture Position:\t" << (initPos.x()/cm) << " " << (initPos.y()/cm) << " " << (initPos.z()/cm) << " cm"
                        << std::endl;
                    log->CloseFile();
                    if(fFirstNeutronRawPrint) { fFirstNeutronRawPrint = false; }
                }
            } else { nCapCountInner--; }	// Decrement capture counter if capture was not detected
        
            // Determines if neutron entered into sensitive volume from external source
            if((*InnerNHitHCE)[i]->GetForeign()) {
                G4double nAngle = (*InnerNHitHCE)[i]->GetZenithAngle();
                run_action->PassNEnergy(energy);
                run_action->WriteNEPosition(initPos, run_action->GetEventNumberOffset() + anEvent->GetEventID());
                run_action->PassNZenithAngle(nAngle);
            }
        
#ifdef G4VIS_USE
            // Create hit visualization if visualization is activated
            (*InnerNHitHCE)[i]->Draw();
#endif
        }
        run_action->AddNCount(nCountInner);         // Records total neutron count
        run_action->AddNCapCount(nCapCountInner);   // Records neutron capture count
    }
  
  
    /* --------------- Photon Statistics ---------------- */
    if(PMTPhotHitHCE && PMTPhotHitHCE->entries()) {
        G4int n = PMTPhotHitHCE->entries();		// Counter for photon population in sensitive volume
        //     G4cerr << "n = " <<n<< G4endl;
        //      G4cerr << "<><>><><><><><><><><>    PMTPhotHitHCE->entries(): " <<n<< "<><>><><><><><><><><>" << G4endl;
      
#ifdef G4VIS_USE
        for(int i = 0; i < n; i++) // activates visualization process for photon sensitive volumes (code located in OpticalHit.cc)
            (*PMTPhotHitHCE)[i]->Draw();
#endif
      
        // Retrieve photon separation parameter specified by user
        G4double EvtSeparationBin = run_action->GetLSPhotonCountEventCut();
        std::map<G4int,G4int>* copyPhotons = new std::map<G4int,G4int>();
        G4int numPhotons = 0;
        G4int EvtSeparationCount = 0;
        // This loop used to record photon statistics
        for(int i = 0; i < n; i++) {
            OpticalHit* aHit = (*PMTPhotHitHCE)[i];
            std::vector<G4double>* time = aHit->GetTime();
            G4int copyNo = aHit->GetCopyNumber();
            (*copyPhotons)[copyNo] = (G4int)(time->size());
            numPhotons += (G4int)(time->size());
            std::vector<G4double>::iterator ittr = time->begin();
            while(ittr != time->end()) {
                // Records event-by-event time distribution
                if(EventTime) {
                    std::map<G4double,G4int>::iterator itr = EventTime->begin();
                    while(*ittr > itr->first && itr != EventTime->end()) { itr++; }
                    (*EventTime)[itr->first]++;
                }
                // Records photon count separation in time
                if(*ittr > EvtSeparationBin) { EvtSeparationCount++; }
                ittr++;
            }
        }
        
        if(EventTime) { run_action->PassEventTiming(EventTime); }
        
        if(numPhotons) {
            run_action->PassPhotonCount(numPhotons,EvtSeparationCount);
            Event* ev = RootIO::GetInstance()->GetEvent();

            ev->fEventNumber =  run_action->GetEventNumberOffset() + anEvent->GetEventID();

            std::map<G4int,G4int>::iterator itzr; // = copyPhotons->find(z*100+zz);
            std::map<G4int,G4int>::iterator itzr2; // = copyPhotons->find(10000+z*100+zz);
            for(int z = 0; z < detector->GetNSegX(); z++) {
                for(int zz=0;zz<detector->GetNSegY();zz++) {
                    itzr = copyPhotons->find(z*100+zz); 
                    itzr2 = copyPhotons->find(10000+z*100+zz);
                    if(itzr != copyPhotons->end()) { 
                        ev->fNpmt[z*detector->GetNSegY() + zz] = itzr->second;
                        ev->fSpmt[z*detector->GetNSegY() + zz] = itzr2->second;
                    } 
                }
            }
            
            if(fFirstLSPhotonRawPrint) { fFirstLSPhotonRawPrint = false; }
            RootIO::GetInstance()->FillTree(); 
        }
        
    } else if(RootIO::GetInstance()->GetFill() || run_action->GetRecordLevel()==3) {
        Event* ev = RootIO::GetInstance()->GetEvent();
        ev->fEventNumber =  run_action->GetEventNumberOffset() + anEvent->GetEventID();	
        RootIO::GetInstance()->FillTree(); 
    }

  /* ---------------- Veto Statistics ----------------- */
  /*  if(VetoPhotHitHCE && VetoPhotHitHCE->entries())
      {
      G4int n = VetoPhotHitHCE->entries();		// Counter for photon population in sensitive volume
      
      #ifdef G4VIS_USE
      for(int i = 0; i < n; i++)			// This loop activates visualization process for photon sensitive volumes (code located in OpticalHit.cc)
      {
      (*VetoPhotHitHCE)[i]->Draw();
      }
      #endif
      
      G4int numPhotons = 0;
      for(int i = 0; i < n; i++)				// This loop used to record veto photon statistics
      {
      OpticalHit* aHit = (*VetoPhotHitHCE)[i];
      std::vector<G4double>* time = aHit->GetTime();
      numPhotons += (G4int)(time->size());
      }
      // Records number of photons detected by veto panels
      if(numPhotons) { run_action->PassVetoCount(numPhotons); }
      }*/
  
    /* --------------- Proton Statistics ---------------- */
    if(InnerPHitHCE && InnerPHitHCE->entries()) {
        G4int pCountInner = InnerPHitHCE->entries(); // Counter for proton population in sensitive volume
        for(int i = 0; i < pCountInner; i++) {
            G4double energy = (*InnerPHitHCE)[i]->GetEnergy();
            // Records initial proton energy 
            run_action->PassProtonEnergy(energy);
        }
        run_action->AddPCount(pCountInner); // Records proton count in sensitive volume
    }
  
  
    /* ------------- Ionisation Statistics -------------- */
    // Scintillator Volume
    if(InnerEDepHCE && InnerEDepHCE->entries()) {
        // Coded for a single entry only - no loop required
        G4double energy_deposit = (*InnerEDepHCE)[0]->GetEnergyDeposit();
        G4double energy_escaped = (*InnerEDepHCE)[0]->GetEnergyEscaped();
        run_action->PassIonEDeposit(energy_deposit); // Records energy deposited in sensitive volume
        run_action->PassIonEEscape(energy_escaped);  // Records energy escaped from sensitive volume
    }
  
  /* if(VetoEDepHCE && VetoEDepHCE->entries())	// Veto Panels
     {
     // Coded for a single entry only - no loop required
     G4double energy_deposit = (*VetoEDepHCE)[0]->GetEnergyDeposit();
     run_action->PassIonVDeposit(energy_deposit);
     }*/
  
    // Forces output of all specified run data every N events
    if(anEvent->GetEventID() != 0 && fPrintPerEvent && anEvent->GetEventID() % fPrintPerEvent == 0)
        run_action->PrintCurrentData();
    
    // Clears the histograms for next event - possibly a redundant feature, but saves memory
    if(EventTime) EventTime->clear();
    RootIO::GetInstance()->Clear();
}

// -------------------------------------------------------------------------------------------------- //

// ****** Event-By-Event Photon Time Output Switch ****** //
void EventAction::SetPhotonTravelTimeTable(G4bool activate) {
    DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    if(EventTime) { delete EventTime; EventTime = 0; }
    if(activate) {
        EventTime = new std::map<G4double,G4int>();
        G4cout << "Photon travel time from generation will be recorded in future runs." << G4endl;
        if(!detector->IsOpticalTransported() && G4RunManager::GetRunManager()->GetVerboseLevel() > 0)
        { G4cout << "*** CAUTION: Optical photon generation/transport not specified. Requested data table will be null until optical processes are set. ***" << G4endl; }
    } else {
        G4cout << "Photon travel time from generation will no longer be recorded in future runs." << G4endl;
    }
}

// ****** Change Print Checkpoint ****** //
void EventAction::SetPrintPerNEvents(G4int num) {
    if(num <= 0 && G4RunManager::GetRunManager()->GetVerboseLevel() > 0) {
        G4cout << "*** CAUTION: Simulation will no longer print data at event checkpoints. All data will be printed at the end of run. ***" << G4endl;
        fPrintPerEvent = 0;
    } else {
        fPrintPerEvent = num;
        G4cout << "Requested simulation data will now be printed into respective files every " << fPrintPerEvent << " events." << G4endl;
    }
}
