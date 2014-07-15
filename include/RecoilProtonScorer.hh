// Unrestricted Use - Property of AECL
//
// RecoilProtonScorer.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Recoil Proton Scorer
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef RecoilProtonScorer_H		// Only creates if object is not yet defined
#define RecoilProtonScorer_H 1		// Defines object

#include "G4VPrimitiveScorer.hh"		// Specifies the base or parent class

#include "ProtonHit.hh"				// Specifies user-defined classes which are called upon in this class

#include "G4THitsMap.hh"			// Specifies the classes which contain structures called upon in this class

#include "globals.hh"				// Specifies class defining all global parameters and variable types

class G4Step;					// Structures necessary for class definition

/* -------- Class Definition --------- */

class RecoilProtonScorer : public G4VPrimitiveScorer
{
  public:	// Constructors and Destructors

    RecoilProtonScorer(G4String);		// Constructor - name is VERY important in activating the detector, know naming conventions!
    ~RecoilProtonScorer();			// Destructor

  public:	// Accesible Methods

    void Initialize(G4HCofThisEvent* HCE);				// Initializes detector, creates hits collection of detector which is permanent until end of run
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ro_hist);	// This function defines what a "hit" consists of and what data to store when a "hit" occurs
    void EndOfEvent(G4HCofThisEvent*);					// Some processing to be done when an event is finished, as new event will erase old event data

    G4int GetHCID() const { return HCIDProt; };
    ProtonHitsCollection* GetCollection() { return pHit_collection; };

  private:	// Member Data

    ProtonHitsCollection* pHit_collection;
    G4int HCIDProt;
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
