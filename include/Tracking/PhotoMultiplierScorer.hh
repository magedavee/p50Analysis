// Unrestricted Use - Property of AECL
//
// PhotoMultiplierScorer.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for PMT Optical Photon Scorer
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef PhotoMultiplierScorer_H
#define PhotoMultiplierScorer_H

#include "OpticalHit.hh"

#include <G4VPrimitiveScorer.hh>
#include <G4THitsMap.hh>

class G4Step;					// Structures necessary for class definition

/* -------- Class Definition --------- */

class PhotoMultiplierScorer : public G4VPrimitiveScorer
{
  public:	// Constructors and Destructors

    PhotoMultiplierScorer(G4String);		// Constructor - name is VERY important in activating the detector, know naming conventions!
    ~PhotoMultiplierScorer();			// Destructor

  public:	// Accesible Methods

    void Initialize(G4HCofThisEvent* HCE);					// Initializes detector, creates hits collection of detector which is permanent until end of run
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ro_hist);		// This function defines what a "hit" consists of and what data to store when a "hit" occurs
    void EndOfEvent(G4HCofThisEvent*);						// Some processing to be done when an event is finished, as new event will erase old event data

    void SetQuantumEfficiency(vector<G4double>, vector<G4double>);

    G4int GetHCID() const { return HCIDOpt; };
    OpticalHitsCollection* GetCollection() { return oHit_collection; };

  protected:	// Internal Methods

    G4double CalculateQuantumEfficiency(G4double);

  private:	// Member Data

    OpticalHitsCollection* oHit_collection;
    G4int HCIDOpt;
    map<G4double,G4double>* QuantumEfficiency;	// Quantum efficiency vectors
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
