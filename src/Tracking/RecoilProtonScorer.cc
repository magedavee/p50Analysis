// Unrestricted Use - Property of AECL
//
// RecoilProtonScorer.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Recoil Proton Scorer
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "RecoilProtonScorer.hh"		// Specifies the file which contains the class structure

#include "ProtonHit.hh"					// Specifies user-defined classes which are called upon in this class
#include "LogSession.hh"

#include "G4Track.hh"					// Specifies classes which contain structures called upon in this class
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4THitsMap.hh"
#include "G4ParticleTypes.hh"
#include "G4VSolid.hh"

#include "G4ios.hh"					// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"					// Specifies class defining all global constants and variable types

	// ****** Constructor ****** //
RecoilProtonScorer::RecoilProtonScorer(G4String HCname)
 : G4VPrimitiveScorer(HCname)
{
  HCIDProt = -1;
  pHit_collection = 0;
}

	// ****** Destructor ****** //
RecoilProtonScorer::~RecoilProtonScorer()
{
  delete pHit_collection;
}

	// ****** Initialize Event ****** //
void RecoilProtonScorer::Initialize(G4HCofThisEvent* HCE)
{
  G4String name = GetMultiFunctionalDetector()->GetName();
  pHit_collection = new ProtonHitsCollection(name,primitiveName);
  if(HCIDProt<0)
  {
    HCIDProt = G4SDManager::GetSDMpointer()->GetCollectionID(pHit_collection);
  }
  HCE->AddHitsCollection(HCIDProt,pHit_collection);
}

	// ****** Process Event Hit ****** //
G4bool RecoilProtonScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
	// Specifies criteria for what will be counted as a hit for a collection, called per step
  if(aStep->GetTrack()->GetDefinition() == G4Proton::ProtonDefinition())
  {
	// Generate new hit and initialize basic values
    ProtonHit* aHit = new ProtonHit();
    G4bool IsNew = true;
    aHit->SetEnergy(aStep->GetTrack()->GetVertexKineticEnergy());
    aHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  	// Compare produced hit to all stored hits and combine if any are equal
    for(int i = 0; i < pHit_collection->entries(); i++)
    {
	// Comparative operator checks track ID
      if(*((*pHit_collection)[i]) == *aHit) { IsNew = false; break; }
    }

	// If comparison does not find an equality, register a new hit into the collection
    if(IsNew)
    {
      pHit_collection->insert(aHit);
    }
    return true;
  }
  return false;
}

	// ****** Terminate Event ****** //
void RecoilProtonScorer::EndOfEvent(G4HCofThisEvent*)
{
	// Additional hit processing at end-of-event can be placed here
  HCIDProt = -1;
  pHit_collection = 0;
}

// EOF
