// Unrestricted Use - Property of AECL
//
// IonisationScorer.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Ionizing Radiation Scorer
//      Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef IonisationScorer_H
/// Assure header file is only loaded once
#define IonisationScorer_H

#include "G4VPrimitiveScorer.hh"

#include "IonisationHit.hh"

#include "G4THitsMap.hh"

#include "globals.hh"

class G4Step;

/// PrimitiveScorer associates an amount of ionization with each detector volume
class IonisationScorer : public G4VPrimitiveScorer {
public:
    
    /// Constructor - name is VERY important in activating the detector, know naming conventions!
    IonisationScorer(G4String);
    /// Destructor
    ~IonisationScorer();

    /// Initializes detector, creates hits collection of detector which is permanent until end of run
    void Initialize(G4HCofThisEvent* HCE);
    /// Called each step to determine what qualifies as a "hit"
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);
    /// Some processing to be done when an event is finished, as new event will erase old event data
    void EndOfEvent(G4HCofThisEvent*);

    G4int GetHCID() const { return HCIDIon; };
    IonisationHitsCollection* GetCollection() { return ionise_collection; };

private:

    IonisationHitsCollection* ionise_collection;
    G4int HCIDIon;
};

#endif
