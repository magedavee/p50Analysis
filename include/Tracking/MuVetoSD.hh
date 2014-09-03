#ifndef MUVETOSD_HH
#define MUVETOSD_HH

#include "IoniSD.hh"
#include <G4VSensitiveDetector.hh>
#include <G4LogicalVolume.hh>

/// Sensitive detector for ionization in muon veto
class MuVetoSD : public IoniSD, public G4VSensitiveDetector {
public:
    
    /// Constructor
    MuVetoSD(G4String name, G4LogicalVolume* V);
    
    /// Initializes detector at start of event
    void Initialize(G4HCofThisEvent*);
    /// Called each step to determine what qualifies as a "hit"
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* H);
    /// End-of-event processing
    void EndOfEvent(G4HCofThisEvent*);
    
protected:
    vector<IonisationHit*> hit_history; ///< ionization hits collection by detector
    G4LogicalVolume* myVol;             ///< logical volume to track
};

#endif
