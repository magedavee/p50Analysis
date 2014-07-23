// Unrestricted Use - Property of AECL
//
// IonisationSD.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Ionizing Radiation Scorer
//      Contains definitions for functions in header file
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "IonisationSD.hh"

#include "LogSession.hh"
#include "RootIO.hh"
#include "RunAction.hh"

#include "G4UnitsTable.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ParticleTypes.hh"
#include "G4VSolid.hh"
#include "G4RunManager.hh"	
#include "G4TouchableHandle.hh"

#include "G4ios.hh"

#include "globals.hh"

void IonisationHit::Display() const {
    G4cerr << "Hit E=" << G4BestUnit(sum_w,"Energy") 
    << " at t=" << G4BestUnit(GetTime(),"Time") << "( " << G4BestUnit(GetDTime(),"Time")
    << ")\tx=[ " << G4BestUnit(GetPos(),"Length") << "] { " << G4BestUnit(GetDPos(),"Length") << "}" << G4endl;
}
    
////////////////////////////////////////////////////////////////   

void IonisationSD::Initialize(G4HCofThisEvent*) {    
    hit_history.clear();
    nclusters = 0;
}

G4bool IonisationSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    
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


void IonisationSD::RegisterHit(IonisationHit* h) {
    h->Display();
    nclusters++;
    
    // add to ROOT output
    EventIoniCluster c;
    c.E = h->GetEnergyDeposit();
    c.t = h->GetTime();
    c.dt = h->GetDTime();
    for(uint i=0; i<3; i++) { c.x[i] = h->GetPos()[i]; c.dx[i] = h->GetDPos()[i]; }
    c.vol = h->GetVolume();
    RootIO::GetInstance()->GetEvent().AddIoniCluster(c);
}

bool compare_hit_times(const IonisationHit* a, const IonisationHit* b) { return a->GetTime() < b->GetTime(); }

void IonisationSD::EndOfEvent(G4HCofThisEvent*) {
    
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
                if(prevHit->GetEnergyDeposit() > edep_threshold) RegisterHit(prevHit);
                else delete prevHit;
                prevHit = *ihit;
            } else {
                *prevHit += **ihit;
                delete *ihit;
            }
        }
        if(prevHit->GetEnergyDeposit() > edep_threshold) RegisterHit(prevHit);
        else delete prevHit;
        G4cerr << "\t" << it->second.size() << " hits in " << nclusters << " clusters." << G4endl;
    }
}

