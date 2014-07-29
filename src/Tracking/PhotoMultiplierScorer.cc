// Unrestricted Use - Property of AECL
//
// PhotoMultiplierScorer.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for PMT Optical Photon Scorer
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "PhotoMultiplierScorer.hh"		// Specifies the file which contains the class structure

#include "OpticalHit.hh"					// Specifies user-defined classes which are called upon in this class
#include "LogSession.hh"
#include "TrackInformation.hh"

#include <G4SystemOfUnits.hh>
#include "G4Track.hh"
#include "G4RunManager.hh"					// Specifies classes which contain structures called upon in this class
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4THitsMap.hh"
#include "G4ParticleTypes.hh"
#include "G4VSolid.hh"
#include "RunAction.hh"

#include "G4ios.hh"					// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"					// Specifies class defining all global constants and variable types
#include <map>

	// ****** Constructor ****** //
PhotoMultiplierScorer::PhotoMultiplierScorer(G4String HCname)
 : G4VPrimitiveScorer(HCname)
{
  HCIDOpt = -1;
  oHit_collection = 0;
  QuantumEfficiency = new std::map<G4double,G4double>();
}

	// ****** Destructor ****** //
PhotoMultiplierScorer::~PhotoMultiplierScorer()
{
  delete oHit_collection;
  delete QuantumEfficiency;
}

	// ****** Initialize Event ****** //
void PhotoMultiplierScorer::Initialize(G4HCofThisEvent* HCE)
{
  G4String name = GetMultiFunctionalDetector()->GetName();
  oHit_collection = new OpticalHitsCollection(name,primitiveName);
  if(HCIDOpt<0)
  {
    HCIDOpt = G4SDManager::GetSDMpointer()->GetCollectionID(oHit_collection);
  }
  HCE->AddHitsCollection(HCIDOpt,oHit_collection);
}

	// ****** Process Event Hit ****** //
G4bool PhotoMultiplierScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  
  // Specifies criteria for what will be counted as a hit for a collection, called per step
  if(aStep->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
      // Check if photon passes quantum efficiency sampling
      G4double ePhoton = aStep->GetPostStepPoint()->GetKineticEnergy();
      if(G4UniformRand() <= CalculateQuantumEfficiency(ePhoton))
	{
	  // If it does, generate new hit and initialize basic values
	  G4double tPhoton = aStep->GetPostStepPoint()->GetGlobalTime();
	  OpticalHit* aHit = new OpticalHit();
	  aHit->SetVolume(aStep->GetTrack()->GetVolume()->GetName());
	  //	  G4cerr << "PH SCORE\t" <<aStep->GetTrack()->GetVolume()->GetName()<< G4endl;
	  aHit->SetEnergy(ePhoton);
	  aHit->SetTime(tPhoton);
	  
	  // Compare produced hit to all stored hits and combine if any are equal
	  G4bool PMTHitAlready = false;
	  G4int n = oHit_collection->entries();
	  for(int i = 0; i < n; i++)
	    {
	      // Comparative operator checks volume name and combines hit details if the hits have the same volume name
	      OpticalHit* theHit = (*oHit_collection)[i];
	      if(*aHit == *theHit)
		{
		  theHit->SetEnergy(ePhoton);
		  theHit->SetTime(tPhoton);
		  PMTHitAlready = true;
		  break;
		}
	    }
	  
	  // If comparison does not find an equality, register a new hit into the collection
	  if(!PMTHitAlready) { oHit_collection->insert(aHit); }
	  
	  // Flag trajectory to be drawn at end of event - saves memory as it only draws accepted photon tracks
	  TrackInformation* info = (TrackInformation*)(aStep->GetTrack()->GetUserInformation());
	  info->SetTrackStatusFlag(true);
	  
	  // Kill photon as it is absorbed by PMT via photoelectric effect
	  aStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
	  return true;
	}
      else { aStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries); }	// Kill photon as it is absorbed by PMT without electronic emission
    }
  return false;
}

	// ****** Terminate Event ****** //
void PhotoMultiplierScorer::EndOfEvent(G4HCofThisEvent*)
{
	// Additional processing that can occur at the end of every event
  HCIDOpt = -1;
  oHit_collection = 0;
}

	// ****** Change Quantum Efficiency Vector ****** //
void PhotoMultiplierScorer::SetQuantumEfficiency(std::vector<G4double> E, std::vector<G4double> QE)
{
	// Ensures both vectors conatin an equal amount of entries
  if(E.size() != QE.size()) { G4cout << "*** WARNING: Energy vector not equal to QE vector. PMT Quantum Efficiency not set. ***" << G4endl; return; }
  for(int i = 0; i < (int)(E.size()); i++)
  {
    (*QuantumEfficiency)[E[i]] = QE[i];
  }
}

	// ****** Calculate Quantum Efficiency ****** //
G4double PhotoMultiplierScorer::CalculateQuantumEfficiency(G4double photonE)
{
	// Defines user-defined limits of QE vectors
  std::map<G4double,G4double>::iterator btr = QuantumEfficiency->begin();
  std::map<G4double,G4double>::iterator ftr = QuantumEfficiency->end(); ftr--;

	// Return 100% efficiency if neither vectors have entries in them
  if(QuantumEfficiency->empty()) { return 100.0; }
	// Return null effiency if photon energy is below lowest energy entry
  else if(photonE < btr->first) { return 0.0; }
	// Return null efficiency if photon energy is above highest energy entry
  else if(photonE > ftr->first) { return 0.0; }

	// Find corresponding bin for incident photon energy
  std::map<G4double,G4double>::iterator itr = QuantumEfficiency->begin();
  std::map<G4double,G4double>::iterator ittr = QuantumEfficiency->begin();
  while(photonE > itr->first) { itr++; ittr++; } ittr--;

	// Uses linear interpolation to determine quantum efficiency from vectors
  G4double slope = (itr->second - ittr->second) / (itr->first - ittr->first);
  G4double QE = slope*(photonE - ittr->first) + ittr->second;
  return QE;
}

// EOF
