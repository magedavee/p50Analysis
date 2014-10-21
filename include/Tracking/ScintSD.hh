#ifndef SCINTSD_HH
/// Assure header file is only loaded once
#define SCINTSD_HH

#include "IoniSD.hh"
#include "ScintSegVol.hh"

#include <G4VSensitiveDetector.hh>
class G4VPhysicalVolume;

/// Sensitive detector for tracking ionizing energy deposition
class ScintSD : public IoniSD, public G4VSensitiveDetector {
public:
    
    /// Constructor
    ScintSD(G4String name, ScintSegVol& T, G4VPhysicalVolume* W);

    /// Initializes detector at start of event
    void Initialize(G4HCofThisEvent*);
    /// Called each step to determine what qualifies as a "hit"
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* H);
    /// Some processing to be done when an event is finished, as new event will erase old event data
    void EndOfEvent(G4HCofThisEvent*);
    
protected:
    
    /// process neutron capture events
    G4bool ProcessNeutronHits(G4Step* aStep, G4TouchableHistory*);
    
    G4int verbose;                                      ///< output verbosity level
    map<pair<G4int,G4int>, vector<IonisationHit*> > hit_history;   ///< ionization hits collection by detector
    map<G4int, G4int> secondaries_counter;              ///< count of previously-observed secondaries in each track
    
    ScintSegVol& myScint;                               ///< scintillator geometry reference
};

#endif
