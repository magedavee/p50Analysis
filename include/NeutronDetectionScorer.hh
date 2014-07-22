// Unrestricted Use - Property of AECL
//
// NeutronDetectionScorer.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Neutron Detection Scorer
//      Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef NeutronDetectionScorer_H
/// Assure header is only loaded once
#define NeutronDetectionScorer_H

#include "G4VPrimitiveScorer.hh"

#include "NeutronHit.hh"

#include "G4THitsMap.hh"

#include "globals.hh"

class G4Step;

class NeutronDetectionScorer : public G4VPrimitiveScorer {
public:	// Constructors and Destructors
    /// Constructor. Name is important in activating the detector --- make sure it's unique!
    NeutronDetectionScorer(G4String);
    /// Destructor
    ~NeutronDetectionScorer();

    /// Initializes detector, creates hits collection of detector which is permanent until end of run
    void Initialize(G4HCofThisEvent* HCE);
    /// Defines what a "hit" consists of and what data to store when a "hit" occurs
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ro_hist);
    /// Some processing to be done when an event is finished, as new event will erase old event data
    void EndOfEvent(G4HCofThisEvent*);
    
    G4int GetHCID() const { return HCIDNeut; };
    NeutronHitsCollection* GetCollection() { return nHit_collection; };
    
private:

    NeutronHitsCollection* nHit_collection;
    G4int HCIDNeut;
};

#endif
