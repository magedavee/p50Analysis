// Unrestricted Use - Property of AECL
//
// IonisationScorer.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Ionizing Radiation Scorer
//      Contains definitions for functions in header file
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "IonisationScorer.hh"

#include "IonisationHit.hh"
#include "LogSession.hh"
#include "RootIO.hh"

#include "G4UnitsTable.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4THitsMap.hh"
#include "G4ParticleTypes.hh"
#include "G4VSolid.hh"
#include "G4RunManager.hh"	
#include "RunAction.hh"
#include "G4TouchableHandle.hh"


#include "G4ios.hh"

#include "globals.hh"

IonisationScorer::IonisationScorer(G4String HCname): G4VPrimitiveScorer(HCname), time_gap(20*ns), edep_threshold(100*keV), nclusters(0) {
    HCIDIon = -1;
    ionise_collection = NULL;
}

IonisationScorer::~IonisationScorer() {
    if(ionise_collection) delete ionise_collection;
}

void IonisationScorer::Initialize(G4HCofThisEvent* HCE) {
    
    // initialize new hits collection for this event
    G4String name = GetMultiFunctionalDetector()->GetName();
    ionise_collection = new IonisationHitsCollection(name,primitiveName);
    if(HCIDIon<0) HCIDIon = G4SDManager::GetSDMpointer()->GetCollectionID(ionise_collection);
    HCE->AddHitsCollection(HCIDIon,ionise_collection);
    
    hit_history.clear();
    nclusters = 0;
}

G4bool IonisationScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    
    // check whether this is ionizing process
    if(!(   aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.0
    //    || aStep->GetTrack()->GetDefinition() == G4Neutron::NeutronDefinition()
        || aStep->GetTrack()->GetDefinition() == G4Gamma::GammaDefinition() ) ) return false;
    
    // Generate new hit and initialize basic values
    IonisationHit* aHit = new IonisationHit();
    G4TouchableHandle hitVol = aStep->GetPreStepPoint()->GetTouchableHandle();
    aHit->SetVolume(hitVol->GetCopyNumber(2));
    //for(uint i=0; i<4; i++) G4cerr << "\t" << hitVol->GetCopyNumber(i); G4cerr << G4endl;
    
    // Record total energy deposit if particle is charged (e-, e+, etc.)
    //if(aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.0)
    aHit->E = aStep->GetTotalEnergyDeposit();
    aHit->t = aStep->GetPreStepPoint()->GetGlobalTime()+0.5*aStep->GetDeltaTime();
    G4ThreeVector pos = aStep->GetPreStepPoint()->GetPosition() + aStep->GetPostStepPoint()->GetPosition();
    for(uint i=0; i<3; i++) aHit->x[i] = 0.5*pos[i];
    
    /* TODO: Need to implement direct energy deposit by gamma in case of high production threshold for e- */
    
    // If step leaves volume, record the particle energy at point of exit as the escape energy
    /*
    if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) { 
        if(aStep->GetTrack()->GetDefinition() == G4Positron::PositronDefinition())
            aHit->SetEnergyEscaped(aStep->GetPostStepPoint()->GetTotalEnergy());
        else
            aHit->SetEnergyEscaped(aStep->GetPostStepPoint()->GetKineticEnergy());
    }
    */
    
    // record hits to volume listing
    aHit->record();
    hit_history[aHit->GetVolume()].push_back(aHit);
    
    return true;
}

bool compare_hit_times(const IonisationHit* a, const IonisationHit* b) { return a->GetTime() < b->GetTime(); }

void IonisationScorer::EndOfEvent(G4HCofThisEvent*) {
    
    G4cerr << "Processing ionization hits in " << hit_history.size() << " volumes." << G4endl;
    for(std::map< G4int, std::vector<IonisationHit*> >::iterator it = hit_history.begin(); it != hit_history.end(); it++) {
        
        // time-order hit events
        std::sort(it->second.begin(), it->second.end(), compare_hit_times);
        
        // group into timing clusters
        std::vector<IonisationHit*>::iterator ihit = it->second.begin();
        IonisationHit* prevHit = *ihit;
        ihit++;
        uint nclusters = 0;
        for(; ihit != it->second.end(); ihit++) {
            if((*ihit)->GetTime() > prevHit->GetTime() + time_gap) {
                if(prevHit->GetEnergyDeposit() > edep_threshold) {
                    ionise_collection->insert(prevHit);
                    prevHit->Display();
                    nclusters++;
                } else delete prevHit;
                prevHit = *ihit;
            } else {
                *prevHit += **ihit;
                delete *ihit;
            }
        }
        if(prevHit->GetEnergyDeposit() > edep_threshold) {
            ionise_collection->insert(prevHit);
            prevHit->Display();
            nclusters++;
        } else delete prevHit;
        G4cerr << "\t" << it->second.size() << " hits in " << nclusters << " clusters." << G4endl;
    }
    
    HCIDIon = -1;
    ionise_collection = NULL;
}

