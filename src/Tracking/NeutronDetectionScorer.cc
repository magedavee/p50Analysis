// Unrestricted Use - Property of AECL
//
// NeutronDetectionScorer.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Neutron Detection Scorer
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "NeutronDetectionScorer.hh"

#include "NeutronHit.hh"
#include "RootIO.hh"

#include <G4SystemOfUnits.hh>
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4THitsMap.hh"
#include "G4ParticleTypes.hh"
#include "G4VSolid.hh"
#include <G4ios.hh>

#include <cassert>

NeutronDetectionScorer::NeutronDetectionScorer(G4String HCname): G4VPrimitiveScorer(HCname) {
    HCIDNeut = -1;
    nHit_collection = NULL;
}

NeutronDetectionScorer::~NeutronDetectionScorer() {
    delete nHit_collection;
}

void NeutronDetectionScorer::Initialize(G4HCofThisEvent* HCE) {
    G4String name = GetMultiFunctionalDetector()->GetName();
    nHit_collection = new NeutronHitsCollection(name,primitiveName);
    if(HCIDNeut < 0) HCIDNeut = G4SDManager::GetSDMpointer()->GetCollectionID(nHit_collection);
    HCE->AddHitsCollection(HCIDNeut,nHit_collection);
}

G4bool NeutronDetectionScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    // only process neutron tracks
    if(aStep->GetTrack()->GetDefinition() != G4Neutron::NeutronDefinition()) return false;
    
    // Generate new hit and initialize basic values
    NeutronHit* aHit = new NeutronHit();
    aHit->SetTrackID(aStep->GetTrack()->GetTrackID());
    aHit->SetEnergy(aStep->GetPreStepPoint()->GetKineticEnergy());
    aHit->SetInitialPosition(aStep->GetPreStepPoint()->GetPosition());
    aHit->SetEntryTime(aStep->GetPreStepPoint()->GetLocalTime());
    aHit->SetTrackTime(aStep->GetPostStepPoint()->GetLocalTime());
    aHit->SetThermalizationTime(aStep->GetPostStepPoint()->GetLocalTime());
    aHit->SetLeft(false);
    aHit->SetCaptured(false);
    aHit->SetForeign(false);
    
    // Record an entry track if particle generation LOGICAL volume is not the same the current LOGICAL volume
    if(!(aStep->GetTrack()->GetLogicalVolumeAtVertex() == aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume())) {
        aHit->SetForeign(true); // Set entry flag
        
        // Record the zenith angle if current step is the first step in the sensitive volume
        if(aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary) {
            aHit->SetZenithAngle(std::fabs((aStep->GetPreStepPoint()->GetMomentumDirection()).angle(G4ThreeVector(0.,-1.,0.))));
        }
    }
    
    // Record a neutron capture if the end of step process is nCapture
    const G4VProcess* PDS = aStep->GetPostStepPoint()->GetProcessDefinedStep();
    if(PDS && PDS->GetProcessName() == "nCapture") {
        
        aHit->SetCaptured(true);
        aHit->SetFinalPosition(aStep->GetPostStepPoint()->GetPosition());
        
        // Record capture gamma production if the capture event produces secondary gamma particles
        G4TrackVector* secondaries = NULL; assert(false); // = aStep->GetSecondary(); TODO
        if(!secondaries->empty()) {
            G4int nGamma = 0;           // number of gammas produced
            G4double totGammaE = 0.0;   // total energy of gammas produced
            for(std::vector<G4Track*>::iterator itrTrack = secondaries->begin() ; itrTrack != secondaries->end(); itrTrack++) {
                //G4int Z = (*itrTrack)->GetDefinition()->GetAtomicNumber();
                //if(1 <= Z && Z <= 100)  RootIO::GetInstance()->GetEvent()->fNcap = Z;
                if((*itrTrack)->GetDefinition() == G4Gamma::GammaDefinition()) {
                    totGammaE += (*itrTrack)->GetKineticEnergy();
                    nGamma++;
                }
            }
            aHit->SetGammasGenerated(nGamma);
        }
    }
    
    // Record an escape track if the particle leaves the sensitive volume
    if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
        aHit->SetFinalPosition(aStep->GetPostStepPoint()->GetPosition());
        aHit->SetLeft(true);
    }
    
    // Stop tallying thermalization time if kinetic energy of neutron is < 1 eV
    if(aStep->GetPreStepPoint()->GetKineticEnergy() < 1.0*eV) aHit->SetThermalizationTime(-1.*s);
    
    // Compare produced hit to all stored hits and combine if any are equal
    G4bool isNew = true;
    for(int i = 0; i < nHit_collection->entries(); i++) {
        // Comparative operator checks track ID
        if(*((*nHit_collection)[i]) == *aHit) {
            *((*nHit_collection)[i]) += *aHit;
            isNew = false;
            break;
        }
    }
    // If comparison does not find an equality, register a new hit into the collection
    if(isNew) {
        // Set thermalization time to 0 s for new entries
        if(aHit->GetThermalizationTime() < 0.*s) { aHit->SetThermalizationTime(0.*s); }
        nHit_collection->insert(aHit);
    }
    
    return true;
}

void NeutronDetectionScorer::EndOfEvent(G4HCofThisEvent*) {
    HCIDNeut = -1;
    nHit_collection = 0;
}
