#ifndef OPTCOUNTERSD_HH
#define OPTCOUNTERSD_HH

#include <G4VSensitiveDetector.hh>
#include <set>
#include "IoniSD.hh"

/// Sensitive detector for recording particles entering/leaving a volume
class OptCounterSD: public IoniSD, public G4VSensitiveDetector {
public:
    /// Constructor
    OptCounterSD(G4String name);
    
    /// Initializes detector at start of event
    void Initialize(G4HCofThisEvent*) { }
    /// Called each step to determine what qualifies as a "hit"
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* H);
    /// Some processing to be done when an event is finished, as new event will erase old event data
    void EndOfEvent(G4HCofThisEvent*) { }
};

#endif
